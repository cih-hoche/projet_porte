#include <WiFi.h>

const char *ssid = "ssid";
const char *password = "password";

TaskHandle_t wifiTaskHandle = NULL;
unsigned long previousMillis = 0;
const long interval = 10000; // 10 seconds

struct WifiParams {
  int param1;
  String param2;
};

WifiParams wifiParams;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  WiFi.mode(WIFI_STA);
}

void wifiConnectTask(void * parameter) {
  WifiParams *params = (WifiParams *)parameter;
  wifiConnect(params);
  vTaskDelete(NULL); // Delete the task when done
}

bool getWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifiConnect(WifiParams *params) {
  if (getWifiConnected()) {
    Serial.println("[WiFi] Already connected");
    onWifiConnected(params);
    return;
  }

  Serial.println();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int tryDelay = 500;
  int numberOfTries = 20;

  while (true) {
    if (getWifiConnected()) {
      Serial.println("[WiFi] WiFi is connected!");
      Serial.print("[WiFi] IP address: ");
      Serial.println(WiFi.localIP());
      onWifiConnected(params);
      return;
    }

    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL: Serial.println("[WiFi] SSID not found"); break;
      case WL_CONNECT_FAILED: Serial.println("[WiFi] Failed - WiFi not connected!"); return;
      case WL_CONNECTION_LOST: Serial.println("[WiFi] Connection was lost"); break;
      case WL_DISCONNECTED: Serial.println("[WiFi] WiFi is disconnected"); break;
      default: Serial.print("[WiFi] WiFi Status: "); Serial.println(WiFi.status()); break;
    }

    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.println("[WiFi] Failed to connect to WiFi!");
      WiFi.disconnect();
      return;
    } else {
      numberOfTries--;
    }
  }
}

void onWifiConnected(WifiParams *params) {
  // Your code to execute when WiFi is connected
  Serial.println("WiFi connected, executing additional function...");
  Serial.print("Param1: ");
  Serial.println(params->param1);
  Serial.print("Param2: ");
  Serial.println(params->param2);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Set parameters
    wifiParams.param1 = 42;
    wifiParams.param2 = "Hello";

    // Create a task to connect to WiFi
    xTaskCreatePinnedToCore(
      wifiConnectTask,   // Function to implement the task
      "WiFiConnectTask", // Name of the task
      4096,              // Stack size in words
      &wifiParams,       // Task input parameter
      1,                 // Priority of the task
      &wifiTaskHandle,   // Task handle
      0                  // Core where the task should run
    );
  }

  // Your main code here
  Serial.println("Loop");
  delay(500);
}