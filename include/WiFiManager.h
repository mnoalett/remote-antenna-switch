#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>

class WiFiManager
{
public:
    WiFiManager(AsyncWebServer &server);

    bool connect(String ssid, String pass, bool dhcp, String ip, String gateway, String netmask);
    void startAPMode();
    bool isConnected();
    String getLocalIP();
    String getHostname();

private:
    AsyncWebServer &webServer;

    IPAddress localIP;
    IPAddress localGateway;
    IPAddress localNetmask;

    static constexpr long INTERVAL = 20000; // interval to wait for Wi-Fi connection (milliseconds)

    void setupAP();
    void setupStation();
};

#endif