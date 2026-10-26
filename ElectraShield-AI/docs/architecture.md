# ElectraShield AI — Technical Design / Architecture

## 1. Purpose

ElectraShield AI is proposed as an IoT-based monitoring and protection prototype for abnormal conditions in low-voltage AC distribution systems. This document describes the starter implementation and separates current code from future work.

## 2. Logical architecture

```text
[Voltage sensor module] ─┐
[Current sensor module] ─┤
[DHT11 temperature] ─────┼──> [ESP32 acquisition + threshold logic]
[Smoke/gas sensor] ──────┘                   │
                         ┌───────────────────┼──────────────────┐
                         v                   v                  v
                    [Relay output]      [Buzzer/LED]      [Wi-Fi + Firebase]
                                                                  │
                                               ┌──────────────────┴─────────┐
                                               v                            v
                                      [Current snapshot]             [Reading history]
                                               │                            │
                                               └──────────────┬─────────────┘
                                                              v
                                                     [Web dashboard]
```

OLED is named in the project presentation but is not yet implemented in the starter firmware.

## 3. Main components

- **ESP32:** reads configured sensor inputs, applies threshold checks, controls local outputs, and publishes JSON data.
- **Voltage sensor:** must provide isolated/conditioned output appropriate for the ESP32 ADC. Exact module and calibration are still to be selected.
- **Current sensor:** must provide a safe, conditioned output. Exact module and calibration are still to be selected.
- **DHT11:** reads ambient temperature and humidity; it is not a winding or contact temperature sensor.
- **Smoke/gas module:** raw ADC output is compared with an example threshold. Sensor type, warm-up, calibration, and environmental limitations must be documented.
- **Relay:** prototype switching output only. Contact ratings, load type, protective coordination, and safe state must be validated.
- **Firebase Realtime Database:** stores the current snapshot and appended readings.
- **Web dashboard:** subscribes to the current snapshot and displays recent history.

## 4. Data flow

1. Firmware samples inputs every five seconds.
2. Firmware derives boolean flags from example thresholds.
3. The starter logic switches the relay output and activates the buzzer/LED if any configured fault flag is true.
4. If Wi-Fi and Firebase authentication are available, firmware writes a current JSON snapshot to `/electrashield/current`.
5. Firmware appends the same sample under `/electrashield/readings`.
6. The browser dashboard listens for changes and renders the latest snapshot and recent history.

## 5. Example data schema

```json
{
  "timestamp_ms": 123456,
  "voltage_v": 230.0,
  "current_a": 1.2,
  "temperature_c": 31.0,
  "humidity_percent": 50.0,
  "smoke_adc": 400,
  "low_voltage": false,
  "high_temperature": false,
  "smoke_detected": false
}
```

`timestamp_ms` is uptime in milliseconds in this starter, not a wall-clock timestamp. Add NTP or a trusted time source for real date/time logging.

## 6. Current implementation vs planned work

### Present in starter code
- ESP32 firmware scaffold and periodic sampling.
- DHT11 and smoke ADC reads.
- Configurable ADC inputs for voltage/current with placeholder conversion factors.
- Example threshold logic and relay/buzzer/LED outputs.
- Firebase current snapshot and history writes.
- Basic browser dashboard source.

### Planned / requires validation
- Sensor-specific voltage/current calibration and accuracy results.
- OLED rendering.
- Firebase offline queue, retry strategy, and data retention.
- Phone push notifications.
- Machine-learning anomaly detection / predictive maintenance.
- Automated test harness and hardware-in-the-loop testing.
- Production-grade electrical safety design and independent validation.

## 7. Security and safety

- Use Firebase Authentication and restrictive Realtime Database rules.
- Do not commit Wi-Fi passwords, Firebase user passwords, service-account keys, or private credentials.
- Never connect mains AC directly to ESP32 ADC/GPIO. Use appropriately rated isolated sensors and safe enclosures.
- A relay module driven by student firmware is not a certified safety device. Retain correctly rated fuses/MCBs/RCDs and other required protections.
- Test with a low-voltage, current-limited source and a safe test load first.

## 8. Validation plan

Record sensor model, calibration procedure, reference instrument, measurement range, error, threshold, response time, and test evidence. Do not report performance metrics until measured on the actual assembled prototype.
