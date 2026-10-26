# Manual Test Cases

**Status:** Test cases are proposed. No pass/fail status is claimed until each test is performed and evidence is recorded.

| ID | Scenario | Setup / stimulus | Expected result | Actual result |
|---|---|---|---|---|
| TC-01 | Normal readings | Use safe test setup with values above configured low-voltage threshold and below other thresholds | Fault flags false; relay output follows configured normal logic; buzzer/LED off | Not run |
| TC-02 | Low voltage | Apply a safe, known sensor input that converts below the configured threshold | `low_voltage=true`; relay output changes to configured fault state; buzzer/LED on | Not run |
| TC-03 | High temperature | Warm DHT11 environment safely above the example threshold | `high_temperature=true`; fault indication active | Not run |
| TC-04 | Smoke threshold | Use a safe, controlled sensor test method appropriate to the exact sensor; avoid open flame/toxic exposure | `smoke_detected=true` above calibrated threshold | Not run |
| TC-05 | Wi-Fi unavailable | Disable access point or use incorrect Wi-Fi credentials | Local sampling/indicators continue; cloud writes unavailable; no offline queue is claimed | Not run |
| TC-06 | Firebase credentials invalid | Use an intentionally invalid test account | Firebase writes fail and diagnostic message appears; no credentials are exposed in logs | Not run |
| TC-07 | DHT read failure | Disconnect DHT only in a low-voltage bench test | Invalid temperature/humidity is not mistaken for valid readings; review current starter behavior and improve sensor-failure handling before real use | Not run |
| TC-08 | Voltage calibration | Compare voltage conversion against a suitable reference instrument using an approved isolated test source | Document error across the intended measurement range; placeholder factor replaced | Not run |
| TC-09 | Current calibration | Compare current conversion against a reference meter and selected sensor datasheet | Document error across the intended measurement range; placeholder factor replaced | Not run |
| TC-10 | Dashboard update | Publish test data to Firebase | Current values and recent readings appear in browser dashboard | Not run |
| TC-11 | Relay polarity / safe state | Use a low-voltage test load; verify relay contact state during boot, fault, and reset | Actual contact behavior matches documented safe-state design | Not run |
| TC-12 | Data history | Publish several samples | New samples appear in `/electrashield/readings` and dashboard history | Not run |

## Test record template

For each test, record:
- Date and tester
- Hardware revision and sensor model
- Firmware commit / version
- Test setup and stimulus
- Expected result
- Actual result
- Pass / fail
- Evidence (photo, serial log, screenshot, or measurement table)
- Follow-up action

## Important notes

- Use low-voltage, current-limited test setups for initial testing.
- Do not simulate smoke using hazardous chemicals or open flame.
- Never connect mains directly to the ESP32 or breadboard.
- The example voltage/current conversions are placeholders and must not be used to make safety decisions.
