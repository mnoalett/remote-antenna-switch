#include "ConfigManager.h"

ConfigManager::ConfigManager() {}

void ConfigManager::begin()
{
    prefs.begin(NAMESPACE, false);
}

String ConfigManager::getSSID()
{
    return prefs.getString(KEY_SSID, "");
}

String ConfigManager::getPassword()
{
    return prefs.getString(KEY_PASS, "");
}

bool ConfigManager::isDHCPEnabled()
{
    return prefs.getBool(KEY_DHCP, true);
}

String ConfigManager::getIP()
{
    return prefs.getString(KEY_IP, "");
}

String ConfigManager::getGateway()
{
    return prefs.getString(KEY_GATEWAY, "");
}

String ConfigManager::getNetmask()
{
    return prefs.getString(KEY_NETMASK, "");
}

bool ConfigManager::isRelayOn()
{
    return prefs.getBool(KEY_RELAY_STATE, false);
}

void ConfigManager::setSSID(const String &ssid)
{
    prefs.putString(KEY_SSID, ssid);
}

void ConfigManager::setPassword(const String &password)
{
    prefs.putString(KEY_PASS, password);
}

void ConfigManager::setDHCP(bool dhcp)
{
    prefs.putBool(KEY_DHCP, dhcp);
}

void ConfigManager::setIP(const String &ip)
{
    prefs.putString(KEY_IP, ip);
}

void ConfigManager::setGateway(const String &gateway)
{
    prefs.putString(KEY_GATEWAY, gateway);
}

void ConfigManager::setNetmask(const String &netmask)
{
    prefs.putString(KEY_NETMASK, netmask);
}

void ConfigManager::setRelayState(bool state)
{
    prefs.putBool(KEY_RELAY_STATE, state);
}

void ConfigManager::clear()
{
    prefs.remove(KEY_SSID);
    prefs.remove(KEY_PASS);
    prefs.remove(KEY_DHCP);
    prefs.remove(KEY_IP);
    prefs.remove(KEY_GATEWAY);
    prefs.remove(KEY_NETMASK);
}