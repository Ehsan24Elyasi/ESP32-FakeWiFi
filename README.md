# ESP32 Wi-Fi Captive Portal

This project creates a Wi-Fi captive portal using an ESP32 microcontroller. It sets up a fake Wi-Fi access point (AP) that prompts users to enter credentials via a login page, mimicking a legitimate network. Captured credentials are stored in the ESP32's EEPROM and can be viewed or cleared through specific endpoints. **This tool is for educational purposes only and must not be used for malicious activities.**

## Features
- Creates a Wi-Fi access point with a customizable SSID.
- Hosts a responsive login page with Persian and English language support.
- Stores usernames and passwords in EEPROM.
- Provides endpoints to view (`/PassList`) and clear (`/clear`) captured credentials.
- Displays a fake "updating" page after credential submission.
- Blinks the built-in LED to indicate activity during submission.

- ## Hardware Requirements
- ESP32 development board (e.g., ESP32 DevKitC).
- USB cable for programming and power.
- Computer with Arduino IDE or PlatformIO.

- ## Endpoints
- `GET /`: Displays the login page.
- `POST /login`: Stores credentials and shows a fake "updating" page.
- `GET /PassList`: Lists all captured credentials.
- `GET /clear`: Clears stored credentials and resets EEPROM.

- ## Disclaimer
**This project is for educational and research purposes only.** Unauthorized use to capture credentials without consent is illegal and unethical. The developer is not responsible for any misuse or damage caused by this code. Always obtain explicit permission before testing on any network or device.
