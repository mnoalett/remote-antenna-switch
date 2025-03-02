#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class ConfigManager
{
public:
    ConfigManager();
    void begin();

    String getSSID();
    String getPassword();
    bool isDHCPEnabled();
    String getIP();
    String getGateway();
    String getNetmask();
    bool isRelayOn();

    void setSSID(const String &ssid);
    void setPassword(const String &password);
    void setDHCP(bool dhcp);
    void setIP(const String &ip);
    void setGateway(const String &gateway);
    void setNetmask(const String &netmask);
    void setRelayState(bool isOn);

    void clear();

private:
    Preferences prefs;

    static constexpr const char *NAMESPACE = "app";
    static constexpr const char *KEY_SSID = "ssid";
    static constexpr const char *KEY_PASS = "pass";
    static constexpr const char *KEY_DHCP = "dhcp";
    static constexpr const char *KEY_IP = "ip";
    static constexpr const char *KEY_GATEWAY = "gateway";
    static constexpr const char *KEY_NETMASK = "netmask";
    static constexpr const char *KEY_RELAY_STATE = "status";
};

#endif