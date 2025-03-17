#include "WebServerManager.h"
#include "Globals.h"

WebServerManager::WebServerManager(ConfigManager &config, WiFiManager &wifi, AsyncWebServer &server, Events &event)
    : configManager(config), wifiManager(wifi), webServer(server), event(event) {}

void WebServerManager::begin(bool useAPMode)
{
  setupRoutes();

  if (useAPMode)
  {
    setupAPRoutes();
  }
  else
  {
    setupSTARoutes();
  }

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS, DELETE");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

  _updateServer.setup(&webServer);
  webServer.begin();
  Serial.println("Web server started.");
}

void WebServerManager::setupAPRoutes()
{
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/wifimanager.html", "text/html"); });

  webServer.serveStatic("/", LittleFS, "/");

  webServer.on("/wifi-networks", HTTP_GET, [this](AsyncWebServerRequest *request)
               { handleGetWiFiNetworks(request); });

  Serial.println("Web server started in AP mode");
}

void WebServerManager::setupSTARoutes()
{
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.html", "text/html", false); });

  webServer.serveStatic("/", LittleFS, "/");

  webServer.onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                          {
    if (request->url() == "/api/relay")
    {
      handlePostRelayState(request, data);
    } });

  webServer.on("/api/relay", HTTP_GET, [this](AsyncWebServerRequest *request)
               { handleGetRelayState(request); });

  webServer.on("/reboot", HTTP_GET, [this](AsyncWebServerRequest *request)
               { handleReboot(request); });

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Web server started in station mode");
}

void WebServerManager::setupRoutes()
{

  webServer.onNotFound([](AsyncWebServerRequest *request)
                       {
      if (request->method() == HTTP_OPTIONS)
      {
          request->send(200);
      } });

  webServer.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest *request)
               { handleGetConfiguration(request); });

  webServer.on("/api/config", HTTP_POST, [this](AsyncWebServerRequest *request)
               { handlePostConfiguration(request); });

  webServer.on("/api/config", HTTP_DELETE, [this](AsyncWebServerRequest *request)
               { handleClearConfiguration(request); });
}

void WebServerManager::handleGetWiFiNetworks(AsyncWebServerRequest *request)
{
  WiFi.scanNetworksAsync([request](int numNetworks)
                         {
    JsonDocument doc;
    JsonArray networks = doc["networks"].to<JsonArray>();

    for (int i = 0; i < numNetworks; i++)
    {
      JsonObject network = networks.add<JsonObject>();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      network["channel"] = WiFi.channel(i);
      switch(WiFi.encryptionType(i)) {
      case ENC_TYPE_NONE:
      network["encryption"] = "Open";
      break;
      case  ENC_TYPE_WEP:
      network["encryption"] = "WEP";
      break;
      case ENC_TYPE_TKIP:
      network["encryption"] = "WPA";
      break;
      case  ENC_TYPE_CCMP:
      network["encryption"] = "WPA2";
      break;
      case ENC_TYPE_AUTO:
      network["encryption"] = "Auto";
      break;
      default:
      network["encryption"] = "Unknown";
      }
    }

  String jsonResponse;
  serializeJson(doc, jsonResponse);
  request->send(200, "application/json", jsonResponse); });
}

void WebServerManager::handlePostRelayState(AsyncWebServerRequest *request, uint8_t *data)
{
  bool relayState = false;

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, data);

  if (error)
  {
    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    return;
  }

  String state = doc["state"];
  if (state == "on")
  {
    relayState = true;
    digitalWrite(RELAY_PIN, HIGH);
    configManager.setRelayState(relayState);
    Serial.println("Set relay on");
    request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"Relay turned ON\"}");
  }
  else if (state == "off")
  {
    relayState = false;
    digitalWrite(RELAY_PIN, LOW);
    configManager.setRelayState(relayState);
    Serial.println("Set relay off");
    request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"Relay turned OFF\"}");
  }
  else
  {
    request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid relay state\"}");
  }
}

void WebServerManager::handleGetRelayState(AsyncWebServerRequest *request)
{
  String status = configManager.isRelayOn() ? "on" : "off";
  String response = "{\"relay_status\":\"" + status + "\"}";
  request->send(200, "application/json", response);
}

void WebServerManager::handleReboot(AsyncWebServerRequest *request)
{
  Serial.println("Rebooting...");
  event.reboot();
  request->send(200);
}

void WebServerManager::handleClearConfiguration(AsyncWebServerRequest *request)
{
  configManager.clear();
  event.reboot();
  request->send(200);
}

void WebServerManager::handleGetConfiguration(AsyncWebServerRequest *request)
{
  char jsonString[256];
  JsonDocument doc;
  doc["swvers"] = SWVERS;
  doc["hwid"] = WiFi.macAddress();
  doc["uptime"] = millis();
  doc["dhcp"] = configManager.isDHCPEnabled();
  doc["ip"] = configManager.getIP();
  doc["gateway"] = configManager.getGateway();
  doc["netmask"] = configManager.getNetmask();
  serializeJson(doc, jsonString);
  request->send(200, "application/json", jsonString);
}

void WebServerManager::handlePostConfiguration(AsyncWebServerRequest *request)
{
  int params = request->params();
  for (int i = 0; i < params; i++)
  {
    const AsyncWebParameter *p = request->getParam(i);
    if (p->isPost())
    {
      // HTTP POST ssid value
      if (p->name() == PARAM_SSID)
      {
        String ssid = p->value().c_str();
        Serial.print("SSID set to: ");
        Serial.println(ssid);
        configManager.setSSID(ssid);
      }

      if (p->name() == PARAM_PASSWORD)
      {
        String pass = p->value().c_str();
        Serial.print("Password set to: ");
        Serial.println(pass);
        configManager.setPassword(pass);
      }

      if (p->name() == PARAM_IP)
      {
        String ip = p->value().c_str();
        Serial.print("IP Address set to: ");
        Serial.println(ip);
        configManager.setIP(ip);
      }

      if (p->name() == PARAM_GATEWAY)
      {
        String gateway = p->value().c_str();
        Serial.print("Gateway set to: ");
        Serial.println(gateway);
        configManager.setGateway(gateway);
      }

      if (p->name() == PARAM_DHCP)
      {
        bool dhcp = p->value().equals("true");
        Serial.println(dhcp);
        configManager.setDHCP(dhcp);
      }

      if (p->name() == PARAM_NETMASK)
      {
        String netmask = p->value().c_str();
        Serial.print("Subnet mask set to: ");
        Serial.println(netmask);
        configManager.setNetmask(netmask);
      }
      Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }
  Serial.println("Parameters saved!");
  request->send(200);
  event.reboot();
}
