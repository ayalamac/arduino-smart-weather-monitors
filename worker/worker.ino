#include "DHT.h"

#define PORT_FREQUENCY  115200

#define BATTERY_PIN     A0
#define DHT_SENSORS_PIN 9
#define RED_LIGHT_PIN   6

#define DHTTYPE DHT22

DHT dht(DHT_SENSORS_PIN, DHTTYPE);

const String ORIGIN_NAME = "Worker";
const unsigned long MEASUREMENT_TIME = 1000;
const unsigned long ALERT_TIME = 500;
static unsigned long lastTime = 0;
char val;

bool alertIsOn,
  shouldActivateAlert,
  shouldDeactivateAlert;

struct SensorData {
  float temperature;
  float humidity;
  float batteryLevel;
};
SensorData data;


void setInitialValues() {
  data = { -1.0, -1.0, 0.0 };
  alertIsOn = false;
  shouldActivateAlert = false;
}

void readSensors() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  data.temperature = isnan(temp) ? data.temperature : temp;
  data.humidity = isnan(hum) ? data.humidity : hum;
  data.batteryLevel = analogRead(BATTERY_PIN) * 9.2 / 1023.0;

  if (isnan(temp) || isnan(hum)) {
    Serial.println("[ERROR] Sensor readings invalid");
  }
}

void sendSensorsData(String originName, long currentTime, SensorData data) {
  String message = "[" + originName + "] - data: {";
  message += "temperature: " + String(data.temperature, 2) + ", ";
  message += "humidity: " + String(data.humidity, 2) + ", ";
  message += "batteryLevel: " + String(data.batteryLevel, 2);
  message += "}";

  Serial.println(message);
}

SensorData operateWeatherMonitor(long measurementTime) {
  unsigned long currentTime = millis();
  bool isTimeToOperate = currentTime - lastTime >= measurementTime;

  if (isTimeToOperate) {
    readSensors();
    sendSensorsData(ORIGIN_NAME, currentTime, data);
    lastTime = currentTime;
  }

  return data;
}

void manageLocalAlert(bool isAlertActive) {
  static unsigned long lastAlertTime = 0;
  static bool isRedLedOn = false;

  const unsigned long alertLedInterval = ALERT_TIME;

  unsigned long currentAlertTime = millis();

  if (isAlertActive && (currentAlertTime - lastAlertTime >= alertLedInterval)) {
    operateWeatherMonitor(MEASUREMENT_TIME);
    lastAlertTime = currentAlertTime;
    isRedLedOn = !isRedLedOn;
    digitalWrite(RED_LIGHT_PIN, isRedLedOn ? HIGH : LOW);
  }

  if (!isAlertActive && isRedLedOn) {
    digitalWrite(RED_LIGHT_PIN, LOW);
    isRedLedOn = false;
  }
}

void evaluateState() {
  if (Serial.available() > 0)
  {
    val = Serial.read();
  }

  if (val == 'A') {
    shouldActivateAlert = true;
  } else if (val == 'O') {
    shouldDeactivateAlert = true;
  } else {
    shouldActivateAlert = false;
    shouldDeactivateAlert = false;
  }
}

void setup() {
  dht.begin();
  Serial.begin(PORT_FREQUENCY);
  setInitialValues();
}

void loop() {
  evaluateState();

  if (shouldActivateAlert) {
    alertIsOn = true;
  }

  if (shouldDeactivateAlert) {
    alertIsOn = false;
  }

  manageLocalAlert(alertIsOn);
}
