#include <Arduino.h>
#include "Events.h"
#include "ConfigManager.h"
#include "WiFiManager.h"
#include "WebServerManager.h"
#include "Globals.h"

ConfigManager config;
Events &event = Events::getInstance();
AsyncWebServer server(80);
WiFiManager wifiManager(server);
WebServerManager webServer(config, wifiManager, server, event);

bool scanComplete = false;

void printInfo(String ssid, bool dhcp, String ip, String gateway, String netmask)
{
  Serial.println();
  Serial.print("ssid: ");
  Serial.println(ssid);
  Serial.print("dhcp: ");
  Serial.println(dhcp);
  Serial.print("ip: ");
  Serial.println(ip);
  Serial.print("gateway: ");
  Serial.println(gateway);
  Serial.print("netmask: ");
  Serial.println(netmask);
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  config.begin();

  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  String ssid = config.getSSID();
  String pass = config.getPassword();
  bool dhcp = config.isDHCPEnabled();
  String ip = config.getIP();
  String gateway = config.getGateway();
  String netmask = config.getNetmask();

  printInfo(ssid, dhcp, ip, gateway, netmask);
  Serial.print("Mac address: ");
  Serial.println(WiFi.macAddress());

  if (ssid == "" || pass == "")
  {
    Serial.println("Undefined SSID or IP address.");
    event.startTimerLedBlink(TIMER_LED_BLINK_500MS);
    wifiManager.startAPMode();
    webServer.begin(true);
  }
  else
  {
    if (wifiManager.connect(ssid, pass, dhcp, ip, gateway, netmask))
    {
      webServer.begin(false);

      String hostname = wifiManager.getHostname();

      if (MDNS.begin(hostname))
      {
        Serial.println("mDNS started");
        Serial.printf("http://%s.local\n", hostname.c_str());
      }
      else
      {
        Serial.println("Error setting up mDNS!");
      }
    }
    else
    {
      event.startTimerLedBlink(TIMER_LED_BLINK_500MS);
      wifiManager.startAPMode();
    }
  }
}

void loop()
{
  MDNS.update();
  event.handleEvent();
}