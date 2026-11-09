# Custom-Audio-Player
Plays custom audio when you touch/click certain parts and things. The auido files it plays must be compressed as there is no external sd card slot, it is based of an esp32

# What is it?

This is a small audio file player with it's own standalone speaker based of an ESP-32-S3-WROOM-1. You can insert your own audio files however it has no external SD card slot meaning you only get the amount of local storage available on the ESP-32. This could be a potential downside because your file(s) will have to be heavily compressed. The reason I chose to design it like this is to keep the cost down and simpler to design.

# Images

## Schematic

### Full Schematic
<img width="808" height="655" alt="Screenshot 2026-11-08 at 8 13 44 PM" src="https://github.com/user-attachments/assets/e8b8921d-10ef-41d7-af78-a9019495a7c3" />

### Oled Screen
<img width="307" height="241" alt="Screenshot 2026-11-08 at 8 14 43 PM" src="https://github.com/user-attachments/assets/ac0d89a8-2295-4873-9a46-80601c870f82" />

### Rotary Encoder
<img width="576" height="486" alt="Screenshot 2026-11-08 at 8 15 10 PM" src="https://github.com/user-attachments/assets/955988c1-861f-45fb-a95a-0e03edf531d4" />

### Voltage Regulator
<img width="357" height="243" alt="Screenshot 2026-11-08 at 8 42 48 PM" src="https://github.com/user-attachments/assets/1c604958-57e3-414f-abaf-b17b47fd41c5" />

### Audio Amplifier
<img width="656" height="559" alt="Screenshot 2026-11-08 at 8 43 53 PM" src="https://github.com/user-attachments/assets/243fe2ee-21d2-4c26-9ae8-1caf18c8ad56" />

### ESP32
<img width="388" height="527" alt="Screenshot 2026-11-08 at 8 45 00 PM" src="https://github.com/user-attachments/assets/ff8b4423-5c2f-4f3a-951c-6dafe81fc237" />

### Receptacle
<img width="313" height="389" alt="Screenshot 2026-11-08 at 8 46 21 PM" src="https://github.com/user-attachments/assets/224c582f-79ed-42b3-89c8-fdd1c6eb7167" />

### Capacitive Touch Pads
<img width="543" height="175" alt="Screenshot 2026-11-08 at 8 46 37 PM" src="https://github.com/user-attachments/assets/652131ee-8451-4aba-b9fc-74896b7bbedb" />

### Cherry MX Button
<img width="384" height="269" alt="Screenshot 2026-11-08 at 8 47 12 PM" src="https://github.com/user-attachments/assets/eac8bdd7-8502-42fb-9eaf-6b3abed0e0d3" />

# BOM

|Item                       |Qty|Supplier  |Cost/item|Total Cost|link                                                 |
|---------------------------|---|----------|---------|----------|-----------------------------------------------------|
|ESP32-S3-WROOM1            |1  |AliExpress|$1.09    |$1.09     |https://www.aliexpress.com/item/1005010196291979.html|
|AP2112K-3.3TRG1            |1  |AliExpress|$1.89    |$1.89     |https://www.aliexpress.com/item/1005009023441155.html|
|MAX98357AETE+T             |1  |AliExpress|$6.44    |$6.44     |https://www.aliexpress.com/item/1005013004267095.html|
|USB Connector Type C Female|1  |AliExpress|$4.81    |$4.81     |https://www.aliexpress.com/item/1005013012638787.html|
|Rotary Encoder             |1  |AliExpress|$1.91    |$1.91     |https://www.aliexpress.com/item/1005007644083514.html|
|4 ohm Speaker              |1  |AliExpress|$2.28    |$2.28     |https://www.aliexpress.com/item/1005008626624201.html|
|Resistors                  |1  |AliExpress|$4.75    |$4.75     |https://www.aliexpress.com/item/1005012948787921.html|
|PCB                        |1  |JLCPCB    |$4.00    |$4.00     |https://jlcpcb.com                                   |
|OLED                       |1  |AliExpress|$2.55    |$2.55     |https://www.aliexpress.com/item/1005005692457058.html|
|Estimated total shipping   |1  |          |         |$1.50     |                                                     |
