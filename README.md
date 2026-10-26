# ElectraShield AI — Predict. Protect. Power.

An ESP32-based IoT prototype concept for monitoring electrical conditions, indicating abnormal events, and publishing readings/fault history to Firebase.

> **Implementation status:** This repository is a starter implementation created from the project presentation. It is not evidence that a physical prototype has been tested. The firmware includes basic sensor acquisition, threshold checks, local indicators, and Firebase Realtime Database writes. The web dashboard is a starter page. Predictive AI, robust offline synchronization, validated electrical measurement accuracy, and production-grade protection are **planned, not implemented**.

## Features

### Included in this starter repository
- ESP32 Arduino firmware structure.
- DHT11 temperature/humidity readings.
- MQ-series analog smoke/gas sensor reading (calibration required).
- Configurable ADC inputs for **properly conditioned and isolated** voltage/current sensor modules.
- Low-voltage threshold detection with a relay output, buzzer, and status LED.
- Firebase Realtime Database telemetry writes (requires project configuration).
- Basic browser dashboard for live values and recent readings.
- Sample CSV data and manual test cases.

### Planned / not yet implemented or validated
- A trained AI/ML model for anomaly detection or predictive maintenance.
- Automatic, reliable offline queue and synchronization.
- Production-grade component health monitoring.
- Push notifications to phones (dashboard currently updates from Firebase; no push notification service is configured).
- Certified mains protection, electrical safety validation, calibration, and field testing.
- Automated unit/integration tests on physical hardware.

## Repository structure

```text
ElectraShield-AI/
├── firmware/
│   └── ElectraShield_AI.ino
├── dashboard/
│   ├── index.html
│   └── firebase-config.example.js
├── docs/
│   └── architecture.md
├── data/
│   └── sample_readings.csv
├── tests/
│   └── test_cases.md
├── firebase/
│   └── database.rules.example.json
├── .gitignore
├── LICENSE
└── README.md
```

## Hardware assumptions

The presentation names an ESP32, relay, DHT11, smoke sensor, OLED, buzzer, and LED controller. It also promises voltage and current monitoring but does not identify the actual voltage/current sensor modules. This starter firmware therefore exposes configurable ADC pins; you must select, install, and calibrate appropriate sensor modules before enabling those measurements.

- ESP32 development board
- DHT11 temperature/humidity module
- MQ-series smoke/gas sensor module (exact model must be confirmed)
- Voltage sensor module with suitable isolation/conditioning for the intended circuit
- Current sensor module with suitable isolation/conditioning for the intended circuit
- Relay module rated for the intended load, plus appropriate upstream protection
- 0.96-inch I²C OLED (optional; display support is a TODO in this starter)
- Buzzer and LED/status indicator
- Wi-Fi and Firebase Realtime Database project

**Safety:** Do not connect mains AC directly to ESP32 pins or a breadboard. Use properly rated, isolated sensing hardware and a qualified supervisor for any mains-related testing. Begin with a low-voltage, current-limited test source. A hobby relay board and this example firmware are not a certified protective device and must not replace a fuse, MCB, RCD/RCCB, or engineered protection system.

## Firmware setup

1. Install Arduino IDE 2.x or PlatformIO.
2. Install the ESP32 board package.
3. Install libraries:
   - `DHT sensor library` by Adafruit
   - `Adafruit Unified Sensor`
   - `Firebase ESP Client` by Mobizt (library name commonly shown as `Firebase ESP Client`)
4. Copy `firmware/ElectraShield_AI.ino` into a sketch folder named `ElectraShield_AI`.
5. Edit the Wi-Fi/Firebase placeholders in the sketch. Never commit real passwords, API keys intended to be private, or database secrets.
6. Confirm every GPIO number and sensor output range for your exact ESP32 board and modules.
7. Keep `RELAY_ACTIVE_LOW` correct for your relay board. Test relay behavior using a low-voltage test load first.
8. Upload and open Serial Monitor at `115200` baud.

### Sensor calibration
The voltage and current ADC conversions are deliberately placeholders. Update the calibration constants and conversion functions using a known reference meter and the selected sensor modules. The analog input must remain within the ESP32 board's allowed input range. Do not assume the ADC input can accept a sensor's full output without conditioning.

## Firebase setup

1. Create a Firebase project and enable **Realtime Database**.
2. Create a database in the appropriate region.
3. Copy the database URL and web API key into the firmware/dashboard configuration.
4. For a quick private prototype, configure authentication appropriately and use restricted rules. Do **not** deploy public read/write rules.
5. The example firmware uses Firebase email/password authentication. Create a dedicated test user and enter its credentials locally in the firmware. Do not commit those credentials.
6. Review `firebase/database.rules.example.json` before adapting rules for your project.
7. For the dashboard, copy `dashboard/firebase-config.example.js` to `dashboard/firebase-config.js`, fill in your Firebase web app config, then serve the folder through a local static server or Firebase Hosting. The example rules require an authenticated user.

> Firebase web API keys identify a Firebase project and are not equivalent to database authorization. Access must be controlled by Firebase Authentication and Security Rules. Keep service-account private keys out of browser code and Git.

## Data format

Firmware writes a current snapshot to `/electrashield/current` and appends readings under `/electrashield/readings`. Each reading contains a timestamp, voltage, current, temperature, humidity, smoke ADC value, and low-voltage status.

The voltage/current values are placeholders until calibrated. Firebase writes require network access; this starter does not yet queue readings while offline.

## Test plan

Use `tests/test_cases.md` for manual checks. Record observed values and evidence from your actual hardware. Do not claim test cases passed until you have run them.

## License

Released under the MIT License; see `LICENSE`.

## Suggested next milestones

1. Confirm the exact voltage/current sensor modules and GPIO wiring.
2. Add and validate OLED display support.
3. Calibrate voltage/current conversion and document error against a reference meter.
4. Add reconnect/backoff and offline buffering.
5. Add authenticated push notifications.
6. Collect labelled measurements and implement/evaluate a real anomaly-detection model.
7. Test failure modes and protection behavior with safe, low-voltage loads before any supervised mains evaluation.
