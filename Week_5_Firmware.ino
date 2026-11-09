#include <Arduino.h>
#include <Wire.h>
#include <LittleFS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>
#include "Audio.h"

// 1. Pin appointing

// MAX98357A I2S Amplifier (U3)
#define I2S_BCLK      15  // Pin 8 on U1 -> I2S_BCLK
#define I2S_LRC       16  // Pin 9 on U1 -> I2S_LRC
#define I2S_DOUT      17  // Pin 10 on U1 -> I2S_DIN

// Rotary Encoder (SW1)
#define ENC_A         38  // Pin 31 on U1 -> ENC_A
#define ENC_B         39  // Pin 32 on U1 -> ENC_B
#define ENC_SW        40  // Pin 33 on U1 -> ENC_SW

// Cherry MX Mechanical Switch (SW2)
#define MX_BTN_PIN    10  // Pin 18 on U1 -> MX_BTN

// I2C OLED Display (J3)
#define OLED_SDA      8   // Pin 12 on U1 -> OLED_SDA
#define OLED_SCL      9   // Pin 13 on U1 -> OLED_SCL
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_ADDR     0x3C

// 5 Capacitive Touch Pads (SW3 - SW7)
const int TOUCH_PINS[5] = {1, 2, 3, 4, 5}; // IO1..IO5
const char* SOUND_FILES[5] = {
  "/sound1.mp3",
  "/sound2.mp3",
  "/sound3.mp3",
  "/sound4.mp3",
  "/sound5.mp3"
};
const char* SOUND_NAMES[5] = {
  "Pad 1: Track 1",
  "Pad 2: Track 2",
  "Pad 3: Track 3",
  "Pad 4: Track 4",
  "Pad 5: Track 5"
};

// 2. Objects
Audio audio;
ESP32Encoder encoder;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int currentVolume = 12;          // Range: 0 to 21
int savedVolume = 12;            // Stores pre-mute volume
bool isMuted = false;
const int MAX_VOLUME = 21;
long lastEncoderPos = 0;

String currentTrack = "Ready";
bool oledAvailable = false;

uint32_t touchBaseline[5];
const uint32_t TOUCH_DELTA = 15000; // Adjust after checking Serial Monitor
bool padPressed[5] = {false, false, false, false, false};

unsigned long lastBtnTime = 0;

// 3. OLED UI

void updateOLED() {
  if (!oledAvailable) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32-S3 SOUNDPAD");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // Active Track Status
  display.setCursor(0, 18);
  display.print("Status: ");
  display.println(audio.isRunning() ? "Playing" : "Idle");

  display.setCursor(0, 30);
  display.println(currentTrack);

  // Volume Label & Percentage
  display.setCursor(0, 44);
  if (isMuted) {
    display.print("Vol: MUTED");
  } else {
    int volPct = map(currentVolume, 0, MAX_VOLUME, 0, 100);
    display.printf("Vol: %d%%", volPct);
  }

  // Volume Progress Bar
  display.drawRect(0, 54, 128, 10, SSD1306_WHITE);
  if (!isMuted && currentVolume > 0) {
    int barWidth = map(currentVolume, 0, MAX_VOLUME, 0, 124);
    display.fillRect(2, 56, barWidth, 6, SSD1306_WHITE);
  }

  display.display();
}

// 4. SETUP

void setup() {
  Serial.begin(115200);
  delay(500);

  // I2C & OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oledAvailable = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 24);
    display.println("Calibrating Pads...");
    display.display();
  } else {
    Serial.println("OLED not detected on J3 (continuing without screen)");
  }

  // Internal Flash Filesystem (LittleFS)
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed!");
    while (true);
  }

  // Switches (SW2 has no external pull-up, ENC_SW has internal pull-up)
  pinMode(MX_BTN_PIN, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  // Hardware Rotary Encoder (External 10k pull-ups on R6, R7)
  encoder.attachHalfQuad(ENC_A, ENC_B);
  encoder.setCount(currentVolume * 2);
  lastEncoderPos = currentVolume;

  // I2S Audio
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(currentVolume);

  // Auto-Calibrate 5 Touch Pads
  Serial.println("Calibrating capacitive touch pads...");
  for (int i = 0; i < 5; i++) {
    uint64_t sum = 0;
    for (int s = 0; s < 25; s++) {
      sum += touchRead(TOUCH_PINS[i]);
      delay(10);
    }
    touchBaseline[i] = sum / 25;
    Serial.printf("Touch_%d (IO%d) Baseline: %lu\n", i + 1, TOUCH_PINS[i], touchBaseline[i]);
  }

  updateOLED();
  Serial.println("System Ready!");
}

// 5. MAIN LOOP

void loop() {
  audio.loop();

  // 1. Rotary Encoder Volume Control
  long newPos = encoder.getCount() / 2;
  if (newPos != lastEncoderPos) {
    int diff = newPos - lastEncoderPos;
    if (isMuted) {
      isMuted = false;
      currentVolume = savedVolume;
    }
    currentVolume = constrain(currentVolume + diff, 0, MAX_VOLUME);
    encoder.setCount(currentVolume * 2);
    lastEncoderPos = currentVolume;

    audio.setVolume(currentVolume);
    Serial.printf("Volume: %d / %d\n", currentVolume, MAX_VOLUME);
    updateOLED();
  }

  // --- 2. Push Button (Mute / Unmute Toggle)
  if (digitalRead(ENC_SW) == LOW && (millis() - lastBtnTime > 250)) {
    lastBtnTime = millis();
    isMuted = !isMuted;
    if (isMuted) {
      savedVolume = currentVolume;
      audio.setVolume(0);
      Serial.println("Muted");
    } else {
      currentVolume = savedVolume;
      audio.setVolume(currentVolume);
      Serial.printf("Unmuted (Vol: %d)\n", currentVolume);
    }
    updateOLED();
  }

  // 3. Cherry MX Switch (Stop Playback)
  if (digitalRead(MX_BTN_PIN) == LOW && (millis() - lastBtnTime > 250)) {
    lastBtnTime = millis();
    if (audio.isRunning()) {
      audio.stopSong();
      currentTrack = "Stopped";
      Serial.println("Stopped via MX_BTN");
      updateOLED();
    }
  }

  // 4. 5 Capacitive Touch Pads
  for (int i = 0; i < 5; i++) {
    uint32_t val = touchRead(TOUCH_PINS[i]);

    if (val > (touchBaseline[i] + TOUCH_DELTA)) {
      if (!padPressed[i]) {
        padPressed[i] = true;
        currentTrack = SOUND_NAMES[i];
        Serial.printf("Touch_%d triggered! (Val: %lu) -> %s\n", i + 1, val, SOUND_FILES[i]);
        audio.connecttoFS(LittleFS, SOUND_FILES[i]);
        updateOLED();
      }
    } else if (val < (touchBaseline[i] + (TOUCH_DELTA / 2))) {
      padPressed[i] = false;
    }
  }
}

// Optional callback when a track finishes playing naturally
void audio_eof_mp3(const char *info) {
  currentTrack = "Finished";
  updateOLED();
}
