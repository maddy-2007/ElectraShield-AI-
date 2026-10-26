/*
  ElectraShield AI — starter ESP32 firmware
  Status: starter code, not validated on physical hardware.
  Libraries: DHT sensor library, Adafruit Unified Sensor, Firebase ESP Client.

  SAFETY:
  - Never connect mains directly to ESP32 GPIO/ADC pins.
  - Use properly rated isolated voltage/current sensing modules.
  - Start with a low-voltage, current-limited test setup.
  - This is a student prototype, not certified protective equipment.
*/

#include <WiFi.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ---------- EDIT THESE LOCALLY; DO NOT COMMIT REAL CREDENTIALS ----------
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
#define API_KEY         "YOUR_FIREBASE_WEB_API_KEY"
#define DATABASE_URL    "https://YOUR_PROJECT-default-rtdb.REGION.firebasedatabase.app/"
#define USER_EMAIL      "YOUR_FIREBASE_TEST_USER_EMAIL"
#define USER_PASSWORD   "YOUR_FIREBASE_TEST_USER_PASSWORD"

// ---------- GPIO CONFIGURATION: VERIFY FOR YOUR EXACT BOARD ----------
constexpr int PIN_DHT = 4;
constexpr int PIN_SMOKE_ADC = 34;  // input-only ADC pin on many classic ESP32 boards
constexpr int PIN_VOLTAGE_ADC = 35; // conditioned/isolated sensor output only
constexpr int PIN_CURRENT_ADC = 32; // conditioned/isolated sensor output only
constexpr int PIN_RELAY = 26;
constexpr int PIN_BUZZER = 25;
constexpr int PIN_STATUS_LED = 2;

#define DHT_TYPE DHT11
#define RELAY_ACTIVE_LOW true

// ---------- THRESHOLDS / CALIBRATION ----------
constexpr float LOW_VOLTAGE_THRESHOLD_V = 200.0f; // example only; set for your system
constexpr float HIGH_TEMPERATURE_THRESHOLD_C = 55.0f; // example only
constexpr int SMOKE_ADC_THRESHOLD = 1800; // example only; calibrate for exact sensor
constexpr unsigned long SAMPLE_INTERVAL_MS = 5000;

// These conversions are PLACEHOLDERS. Calibrate using selected sensors and a reference meter.
constexpr float ADC_REFERENCE_V = 3.3f;
constexpr float ADC_MAX_COUNT = 4095.0f;
constexpr float VOLTAGE_SCALE_FACTOR = 100.0f; // placeholder: sensor-specific calibration required
constexpr float CURRENT_SCALE_FACTOR = 10.0f;  // placeholder: sensor-specific calibration required

DHT dht(PIN_DHT, DHT_TYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSampleMs = 0;
bool firebaseReady = false;

void setRelay(bool enabled) {
  // enabled=true means relay coil is energized. Verify contact wiring and module polarity.
  digitalWrite(PIN_RELAY, RELAY_ACTIVE_LOW ? (enabled ? LOW : HIGH)
                                         : (enabled ? HIGH : LOW));
}

float readVoltagePlaceholder() {
  const int raw = analogRead(PIN_VOLTAGE_ADC);
  const float adcVolts = (raw / ADC_MAX_COUNT) * ADC_REFERENCE_V;
  return adcVolts * VOLTAGE_SCALE_FACTOR;
}

float readCurrentPlaceholder() {
  const int raw = analogRead(PIN_CURRENT_ADC);
  const float adcVolts = (raw / ADC_MAX_COUNT) * ADC_REFERENCE_V;
  return adcVolts * CURRENT_SCALE_FACTOR;
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  const unsigned long started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) {
    delay(300);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? "\nWi-Fi connected" : "\nWi-Fi unavailable");
}

void initializeFirebase() {
  if (String(API_KEY).startsWith("YOUR_") ||
      String(DATABASE_URL).indexOf("YOUR_PROJECT") >= 0 ||
      String(USER_EMAIL).startsWith("YOUR_")) {
    Serial.println("Firebase placeholders not configured; cloud writes disabled.");
    firebaseReady = false;
    return;
  }
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  firebaseReady = true;
}

void publishReading(float voltage, float current, float temperature,
                    float humidity, int smokeRaw, bool lowVoltage,
                    bool temperatureHigh, bool smokeDetected) {
  if (!firebaseReady || WiFi.status() != WL_CONNECTED || !Firebase.ready()) return;

  FirebaseJson reading;
  reading.set("timestamp_ms", (double)millis());
  reading.set("voltage_v", voltage);
  reading.set("current_a", current);
  reading.set("temperature_c", temperature);
  reading.set("humidity_percent", humidity);
  reading.set("smoke_adc", smokeRaw);
  reading.set("low_voltage", lowVoltage);
  reading.set("high_temperature", temperatureHigh);
  reading.set("smoke_detected", smokeDetected);

  if (Firebase.RTDB.setJSON(&fbdo, "/electrashield/current", &reading)) {
    Serial.println("Firebase current snapshot updated.");
  } else {
    Serial.printf("Firebase current write failed: %s\n", fbdo.errorReason().c_str());
  }

  // Push a separate history record. For production, add retention/size management.
  if (!Firebase.RTDB.pushJSON(&fbdo, "/electrashield/readings", &reading)) {
    Serial.printf("Firebase history write failed: %s\n", fbdo.errorReason().c_str());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_STATUS_LED, LOW);

  // Conservative startup: de-energize relay coil. The actual safe state depends on wiring.
  setRelay(false);
  analogReadResolution(12);
  dht.begin();

  connectWiFi();
  initializeFirebase();
  Serial.println("ElectraShield AI starter initialized.");
}

void loop() {
  connectWiFi();
  if (millis() - lastSampleMs < SAMPLE_INTERVAL_MS) {
    delay(20);
    return;
  }
  lastSampleMs = millis();

  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();
  const int smokeRaw = analogRead(PIN_SMOKE_ADC);
  const float voltage = readVoltagePlaceholder();
  const float current = readCurrentPlaceholder();

  const bool dhtValid = !isnan(temperature) && !isnan(humidity);
  const bool lowVoltage = voltage < LOW_VOLTAGE_THRESHOLD_V;
  const bool temperatureHigh = dhtValid && temperature >= HIGH_TEMPERATURE_THRESHOLD_C;
  const bool smokeDetected = smokeRaw >= SMOKE_ADC_THRESHOLD;
  const bool fault = lowVoltage || temperatureHigh || smokeDetected;

  // Demonstration logic only. Verify whether your real load should trip on each condition.
  // For an actual safety design, include sensor-failure detection and engineered protection.
  setRelay(!fault);
  digitalWrite(PIN_BUZZER, fault ? HIGH : LOW);
  digitalWrite(PIN_STATUS_LED, fault ? HIGH : LOW);

  Serial.printf("V=%.2f V, I=%.2f A, T=%.1f C, H=%.1f%%, smoke_raw=%d, fault=%s\n",
                voltage, current, temperature, humidity, smokeRaw,
                fault ? "YES" : "NO");

  publishReading(voltage, current, temperature, humidity, smokeRaw,
                 lowVoltage, temperatureHigh, smokeDetected);
}
