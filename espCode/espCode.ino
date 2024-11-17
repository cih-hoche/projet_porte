#include <WiFi.h>

const char *ssid = "ssid";
const char *password = "password";
const char *host = "example.com";

char *users = "root_id;9924-11-15 20:26:00\nsecret123;2024-11-15 20:26:00\naaa6875;2024-11-18 20:43:00\n123;9924-11-15 20:26:00";

TaskHandle_t wifiTaskHandle = NULL;
unsigned long previousMillis = 0;
const long interval = 10000;

struct WifiParams {
  String exec;
};

WifiParams wifiParams;

// example.com cert
const char *root_ca = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIEyDCCA7CgAwIBAgIQDPW9BitWAvR6uFAsI8zwZjANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0yMTAzMzAwMDAwMDBaFw0zMTAzMjkyMzU5NTlaMFkxCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh
bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD
ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV
cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy
FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc
3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8
osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT
zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAYIwggF+MBIGA1Ud
EwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHSFgMBmx9833s+9KTeqAx2+7c0XMB8G
A1UdIwQYMBaAFE4iVCAYlebjbuYP+vq5Eu0GF485MA4GA1UdDwEB/wQEAwIBhjAd
BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYIKwYBBQUHAQEEajBoMCQG
CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG
NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH
Mi5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t
L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA9BgNVHSAENjA0MAsGCWCGSAGG/WwC
ATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgGBmeBDAECAzANBgkqhkiG
9w0BAQsFAAOCAQEAkPFwyyiXaZd8dP3A+iZ7U6utzWX9upwGnIrXWkOH7U1MVl+t
wcW1BSAuWdH/SvWgKtiwla3JLko716f2b4gp/DA/JIS7w7d7kwcsr4drdjPtAFVS
slme5LnQ89/nD/7d+MS5EHKBCQRfz5eeLjJ1js+aWNJXMX43AYGyZm0pGrFmCW3R
bpD0ufovARTFXFZkAdl9h6g4U5+LXUZtXMYnhIHUfoyMo5tS58aI7Dd8KvvwVVo4
chDYABPPTHPbqjc1qCmBaZx2vN4Ye5DUys/vZwP9BFohFrH/6j/f3IL16/RZkiMN
JCqVJUzKoZHm1Lesh3Sz8W2jmdv51b2EQJ8HmA==
-----END CERTIFICATE-----
)string_literal";

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  WiFi.mode(WIFI_STA);
}

void wifiConnectTask(void *parameter) {
  WifiParams *params = (WifiParams *)parameter;
  wifiConnect(params);
  vTaskDelete(NULL);  // Delete the task when done
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
  int numberOfTries = 10;

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
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
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
  Serial.print("Exec: ");
  Serial.println(params->exec);
}

void getUsers() {
  HTTPClient http;
  http.begin("http://" + host + "/api/users/get");  //, root_ca); // Specify the URL and certificate
  int httpCode = http.GET();                        //Make the request
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    users = (char *)payload.c_str();
    Serial.println(users);
  } else {
    Serial.printf("[HTTPS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    Serial.println("$ Error on HTTP request");
  }
  http.end();  //Free the resources
}
bool isDateBeforeNow(const char *dateStr) {
  struct tm tm;
  strptime(dateStr, "%Y-%m-%d %H:%M:%S", &tm);
  time_t date = mktime(&tm);
  time_t now = time(0);
  return difftime(now, date) > 0;
}

bool isAuthorized(char *username) {
  char *usersCopy = strdup(users);
  char *line = strtok(usersCopy, "\n");
  while (line != NULL) {
    if (strncmp(line, username, strlen(username)) == 0) {
      char *date = strchr(line, ';');
      if (date != NULL) {
        date++;  // Move past the ';' character
        if (isDateBeforeNow(date)) {
          return true;
        } else {
          return false;
        }
      }
    }
    line = strtok(NULL, "\n");
  }
  free(usersCopy);

  return false;
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Set parameters
    wifiParams.exec = "42";

    // Create a task to connect to WiFi
    xTaskCreatePinnedToCore(
      wifiConnectTask,    // Function to implement the task
      "WiFiConnectTask",  // Name of the task
      4096,               // Stack size in words
      &wifiParams,        // Task input parameter
      1,                  // Priority of the task
      &wifiTaskHandle,    // Task handle
      0                   // Core where the task should run
    );
    isAuthorized("root_id");
    isAuthorized("secret123");
    isAuthorized("aaa6875");
    isAuthorized("123");
  }

  // Your main code here
  Serial.println("Loop");
  delay(500);
}