#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "variables.h"

const short int ONBOARD_LED = 2;
const unsigned long BAUD = 115200;
 
ESP8266WebServer webServer(80);

bool handleAuth() {
  if (!webServer.authenticate(BASIC_AUTH_USERNAME, BASIC_AUTH_PASSWORD)) {
    webServer.requestAuthentication();
    Serial.println("authentication failed");
    return false;
  }
  Serial.println("authentication success");
  return true;
}

void handleRoot() {
  if (!handleAuth()) { return; }

  long int startMillis = millis();
  Serial.println("webserver: /");
  webServer.send(200, "text/html", 
    "<h1>ESP01-s Remote Switch</h1><table>"
    "<tr><td><button onclick=\"fetch('gpio0-on')\">GPIO0 ON</button></td><td><button onclick=\"fetch('gpio0-off')\">GPIO0 OFF</button></a></td></tr>"
    "<tr><td><button onclick=\"fetch('gpio1-on')\">GPIO1 ON</button></td><td><button onclick=\"fetch('gpio1-off')\">GPIO1 OFF</button></a></td></tr>"
    "<tr><td><button onclick=\"fetch('gpio2-on')\">GPIO2 ON</button></td><td><button onclick=\"fetch('gpio2-off')\">GPIO2 OFF</button></a></td></tr></table>");
  Serial.print("webserver: request was handled in ");
  Serial.print(millis() - startMillis);
  Serial.println("ms");
}

void handleGPIORequest(uint8_t pin, uint8_t val) {
  if (!handleAuth()) { return; }
  
  long int startMillis = millis();
  Serial.print("webserver: /gpio");
  Serial.print(pin);
  if (val == LOW) {
    Serial.println("-on");
  }
  else {
    Serial.println("-off");
  }
  digitalWrite(pin, val);
  webServer.send(200, "", "");
  Serial.print("webserver: request was handled in ");
  Serial.print(millis() - startMillis);
  Serial.println("ms");
}

void setup() {
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);

  Serial.begin(BAUD);
  Serial.println();

  WiFi.setHostname(WIFI_DEVICE_NAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  webServer.on("/", handleRoot);
  webServer.on("/gpio0-on", [&](){ handleGPIORequest(0, LOW); });
  webServer.on("/gpio0-off", [&](){ handleGPIORequest(0, HIGH); });
  webServer.on("/gpio1-on", [&](){ handleGPIORequest(1, LOW); });
  webServer.on("/gpio1-off", [&](){ handleGPIORequest(1, HIGH); });
  webServer.on("/gpio2-on", [&](){ handleGPIORequest(2, LOW); });
  webServer.on("/gpio2-off", [&](){ handleGPIORequest(2, HIGH); });
  webServer.begin();
  Serial.println("webserver: ready for connections");

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(ONBOARD_LED, LOW);
    delay(500);
    digitalWrite(ONBOARD_LED, HIGH);
    delay(500);
  }
  Serial.println();

  Serial.print("WiFi Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  webServer.handleClient();
}