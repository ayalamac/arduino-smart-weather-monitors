#include "DHT.h"

#define PORT_FREQUENCY  115200

#define BATTERY_PIN     A0
#define DHT_SENSORS_PIN 9
#define RED_LIGHT_PIN   6
#define BUZZER_PIN      23


#define DHTTYPE DHT22

DHT dht(DHT_SENSORS_PIN, DHTTYPE);

const String ORIGIN_NAME = "Main";
const unsigned long MEASUREMENT_TIME = 1000;
const unsigned long ALERT_TIME = 500;
const unsigned long BUZZ_FREQ = 4500;
const unsigned long BUZZ_TIME = 100;
static unsigned long lastTime = 0;

bool temperatureIsNormal,
  temperatureIsHigh,
  alertIsOn,
  shouldActivateAlert,
  shouldDeactivateAlert;

struct SensorData {
  float temperature;
  float humidity;
  float batteryLevel;
};
SensorData data;

enum AlertState {
  NORMAL,
  ALERT
};

struct NotificationPlan {
  AlertState state;
  bool active;
  int budget;
  unsigned long startTime;
  unsigned long interval;
};
NotificationPlan plan;


void setInitialValues() {
  data = { -1.0, -1.0, 0.0 };
  temperatureIsNormal = true;
  temperatureIsHigh = false;
  alertIsOn = false;
  shouldActivateAlert = false;
  plan = {AlertState::NORMAL, false, 0, 0, 0};
  pinMode(BUZZER_PIN, OUTPUT);
}

void evaluateState() {
  temperatureIsNormal   = data.temperature < 27;
  temperatureIsHigh     = data.temperature > 30;
  shouldDeactivateAlert = temperatureIsNormal && alertIsOn;
  shouldActivateAlert   = temperatureIsHigh && !alertIsOn;
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
    lastAlertTime = currentAlertTime;
    isRedLedOn = !isRedLedOn;

    tone(BUZZER_PIN, BUZZ_FREQ, BUZZ_TIME);
    digitalWrite(RED_LIGHT_PIN, isRedLedOn ? HIGH : LOW);
  }

  if (!isAlertActive && isRedLedOn) {
    digitalWrite(RED_LIGHT_PIN, LOW);
    isRedLedOn = false;
  }
}

NotificationPlan createNotificationPlan(AlertState state) {
    NotificationPlan plan;
    plan.interval = 1000;
    plan.budget = 3;
    plan.state = state;
    plan.startTime = millis();
    plan.active = true;
    return plan;
}

void scheduleNotifications(const String& originName, NotificationPlan& plan) {
  if (!plan.active) {
    return;
  }

  long currentTime = millis();

  if (plan.budget > 0 && currentTime - plan.startTime >= plan.interval) {
      sendNotification(originName, plan.state);
      plan.budget--;
      plan.startTime = currentTime;
  }

  if (plan.budget == 0) {
      plan.active = false;
  }
}

void sendNotification(const String& originName, AlertState state) {
  String message = "[" + originName + "] - ";
  message += state == AlertState::ALERT ? "A" : "O";
  Serial.println(message);
}

void setup() {
  dht.begin();
  Serial.begin(PORT_FREQUENCY);
  setInitialValues();
}

void loop() {
  evaluateState();
  operateWeatherMonitor(MEASUREMENT_TIME);

  if (shouldActivateAlert) {
    plan = createNotificationPlan(AlertState::ALERT);
    alertIsOn = true;
  }

  if (shouldDeactivateAlert) {
    plan = createNotificationPlan(AlertState::NORMAL);
    alertIsOn = false;
  }

  scheduleNotifications(ORIGIN_NAME, plan);
  manageLocalAlert(alertIsOn);
}
