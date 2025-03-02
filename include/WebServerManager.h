#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESPAsyncHTTPUpdateServer.h>
#include "LittleFS.h"
#include "ConfigManager.h"
#include "WiFiManager.h"
#include "Events.h"

class WebServerManager
{
public:
    WebServerManager(ConfigManager &config, WiFiManager &wifi, AsyncWebServer &server, Events &event);
    void begin(bool useAPMode);
    void setupAPRoutes();
    void setupSTARoutes();

private:
    static constexpr const char *PARAM_SSID = "ssid";
    static constexpr const char *PARAM_PASSWORD = "pass";
    static constexpr const char *PARAM_IP = "ip";
    static constexpr const char *PARAM_DHCP = "dhcp";
    static constexpr const char *PARAM_GATEWAY = "gateway";
    static constexpr const char *PARAM_NETMASK = "netmask";
    static constexpr const char *PARAM_STATUS = "status";

    ESPAsyncHTTPUpdateServer _updateServer;

    ConfigManager &configManager;
    WiFiManager &wifiManager;
    AsyncWebServer &webServer;
    Events &event;

    void setupRoutes();
    void handleConfigRequest(AsyncWebServerRequest *request);
    void handlePostConfiguration(AsyncWebServerRequest *request);
    void handleReboot(AsyncWebServerRequest *request);
    void handleGetConfiguration(AsyncWebServerRequest *request);
    void handleClearConfiguration(AsyncWebServerRequest *request);
    void handleGetRelayState(AsyncWebServerRequest *request);
    void handlePostRelayState(AsyncWebServerRequest *request, uint8_t *data);
    void handleGetWiFiNetworks(AsyncWebServerRequest *request);
};

#endif