#include <WiFiManager.h>

void WiFiManager::begin() {
  connected = false;

  // WIFI Access Point
  if(EEPROM.read(0) == 0xFF) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  } 
  
  // WIFI Station
  else {
    WiFi.mode(WIFI_STA);
    String ep_ssid = EEPROM.readString(1);
    String ep_password = EEPROM.readString(1 + ep_ssid.length() + 1);
    WiFi.begin(ep_ssid.c_str(), ep_password.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 5) {
      attempts++;
      delay(500);
      Serial.print(".");
    }

    // WIFI Station failed
    if(attempts == 5) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ap_ssid, ap_password);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
    }

    // WIFI Station connected
    else {
      connected = true;
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request){
    String ssid = request->arg("ssid");
    String password = request->arg("password");
    EEPROM.write(0, 0x00);
    EEPROM.writeString(1, ssid);
    EEPROM.writeString(1 + ssid.length() + 1, password);
    EEPROM.commit();
    WiFi.begin(ssid.c_str(), password.c_str());
    request->send_P(200, "text/plain", "Connecting...");
    ESP.restart();
  });

  server.begin();
}


bool WiFiManager::isConnected() {
  return connected;
}


String WiFiManager::getSSID() {
  if(connected) {
    return WiFi.SSID();
  }
  else {
    return EEPROM.readString(1);
  }
}


String WiFiManager::getPassword() {
  if(connected) {
    return WiFi.psk();
  }
  else {
    return EEPROM.readString(1 + EEPROM.readString(1).length() + 1);
  }
}