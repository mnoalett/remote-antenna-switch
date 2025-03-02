#include "WiFiManager.h"

WiFiManager::WiFiManager(AsyncWebServer &server) : webServer(server) {}

bool WiFiManager::connect(String ssid, String pass, bool dhcp, String ip, String gateway, String netmask)
{
    WiFi.mode(WIFI_STA);

    if (!dhcp)
    {
        Serial.println("Set static IP");
        localIP.fromString(ip.c_str());
        localGateway.fromString(gateway.c_str());
        localNetmask.fromString(netmask.c_str());

        if (!WiFi.config(localIP, localGateway, localNetmask))
        {
            Serial.println("STA Failed to configure");
            return false;
        }
    }

    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.println("Connecting to WiFi...");

    unsigned long previousMillis = millis();

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
        if (millis() - previousMillis >= INTERVAL)
        {
            Serial.println("Timeout reached. Failed to connect.");
            return false;
        }
    }
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
}

void WiFiManager::startAPMode()
{
    WiFi.mode(WIFI_AP_STA);
    Serial.println("Setting AP (Access Point)");

    uint8_t mac[6];
    WiFi.macAddress(mac);

    char apSsid[24];
    sprintf(apSsid, "%s%02X%02X", "ESP32-", mac[4], mac[5]);

    WiFi.softAP(apSsid, "12345678");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getLocalIP()
{
    return WiFi.localIP().toString();
}

String WiFiManager::getHostname()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);

    char hostname[24];
    sprintf(hostname, "%s%02X%02X", "antswitch-", mac[4], mac[5]);
    return hostname;
}