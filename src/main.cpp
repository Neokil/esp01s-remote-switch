#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "variables.h"

const short int BUTTON_PIN = 2;
const short int SWITCH_PIN = 0;
const unsigned long BAUD = 115200;

ESP8266WebServer webServer(80);

int buttonState = LOW;
bool switchOn = false;

bool handleAuth()
{
  if (!webServer.authenticate(BASIC_AUTH_USERNAME, BASIC_AUTH_PASSWORD))
  {
    webServer.requestAuthentication();
    Serial.println("authentication failed");
    return false;
  }
  Serial.println("authentication success");
  return true;
}

void handleRoot()
{
  if (!handleAuth())
  {
    return;
  }

  long int startMillis = millis();
  Serial.println("webserver: /");
  webServer.send(200, "text/html", "<h1>ESP01-s Remote Switch</h1><table><tr><td><button onclick=\"fetch('switch-on')\">Switch ON</button></td><td><button onclick=\"fetch('switch-off')\">Switch OFF</button></a></td></tr></table>");
  Serial.print("webserver: request was handled in ");
  Serial.print(millis() - startMillis);
  Serial.println("ms");
}

void handleSwitchRequest(uint8_t val)
{
  if (!handleAuth())
  {
    return;
  }

  long int startMillis = millis();
  Serial.print("webserver: /switch");
  if (val == LOW)
  {
    Serial.println("-on");
    switchOn = true;
  }
  else
  {
    Serial.println("-off");
    switchOn = false;
  }
  digitalWrite(SWITCH_PIN, val);
  webServer.send(200, "", "");
  Serial.print("webserver: request was handled in ");
  Serial.print(millis() - startMillis);
  Serial.println("ms");
}

void flipSwitch()
{
  digitalWrite(SWITCH_PIN, switchOn ? HIGH : LOW);
  switchOn = !switchOn;
}

void handleButton()
{
  int currentButtonState = digitalRead(BUTTON_PIN);
  if (buttonState != currentButtonState)
  {
    buttonState = currentButtonState;

    if (buttonState == LOW)
    {
      flipSwitch();
    }
  }
}

void setup()
{
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(BAUD);
  Serial.println();

  WiFi.setHostname(WIFI_DEVICE_NAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  webServer.on("/", handleRoot);
  webServer.on("/switch-on", [&]()
               { handleSwitchRequest(LOW); });
  webServer.on("/switch-off", [&]()
               { handleSwitchRequest(HIGH); });
  webServer.begin();
  Serial.println("webserver: ready for connections");

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  Serial.print("WiFi Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  webServer.handleClient();
  handleButton();
}