#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>

#define EEPROM_SIZE 512


const char index_html[] PROGMEM = R"html(
<html>
  <head>
    <title>Wi-Fi Manager</title>
    <style>
      body {
        background-color: #eee;
        font-family: sans-serif;
        text-align: center;
        padding-top: 50px;
      }
      form {
        background-color: #fff;
        padding: 40px;
        border-radius: 10px;
        box-shadow: 0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23);
        width: 300px;
        margin: 0 auto;
      }
      input[type="text"], input[type="password"] {
        padding: 10px;
        font-size: 18px;
        width: 100%;
        margin-bottom: 20px;
        border-radius: 5px;
        border: none;
        box-shadow: 0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24);
      }
      input[type="submit"] {
        background-color: #4CAF50;
        color: #fff;
        font-size: 18px;
        padding: 10px 20px;
        border-radius: 5px;
        border: none;
        cursor: pointer;
      }
      input[type="submit"]:hover {
        background-color: #3e8e41;
      }
    </style>
  </head>
  <body>
    <form action="/connect" method="post">
      <input type="text" name="ssid" placeholder="Wi-Fi SSID" required><br>
      <input type="password" name="password" placeholder="Wi-Fi Password" required><br>
      <input type="submit" value="Connect">
    </form>
  </body>
</html>
)html";


class WiFiManager {
  private:
    const char* ap_ssid = "ESP32WiFiManager";
    const char* ap_password = "0000";
    bool connected = false;
    AsyncWebServer server;

  public:
    WiFiManager() : server(80) {}

    void begin();

    bool isConnected();

    String getSSID();

    String getPassword();
};

#endif