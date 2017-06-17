#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "SH1106Wire.h"
#include "OLEDDisplayUi.h"
#include "Wire.h"

// If you are visiting my home, feel free to use my wifi.
const char* ssid     = "sdinet";
const char* password = "boomheadshot";
const char* host = "api.coinbase.com";
const int httpsPort = 443;

// SHA1 fingerprint of the certificate
// echo | openssl s_client -connect api.coinbase.com:443 | openssl x509 -fingerprint -noout | sed 's/:/ /g'
const char* fingerprint = "B3 9C 3E 29 EF 32 9B 20 80 6E AC 2C F0 7C D9 D4 24 1A 2D C6";

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c;
const int SDA_PIN = D1;
const int SDC_PIN = D2;

SH1106Wire      display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
OLEDDisplayUi   ui( &display );

String getExchange(String pair) {
  
  
    WiFiClientSecure client;
    
    if (!client.connect(host, httpsPort)) {
      return String("Error");
    }

    String url = "/v2/prices/" + pair + "/sell";

    client.print("GET " + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(500);
    String json = "";
    boolean httpBody = false;
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      if (!httpBody && line.charAt(1) == '{') {
        httpBody = true;
      }
      if (httpBody) {
        json += line;
      }
    }
    StaticJsonBuffer<400> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    String data = root["data"]["amount"];

    return data;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // initialize dispaly
  display.init();
  display.clear();
  display.display();

  //display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);

  display.clear();
  display.drawString(64, 10, "Coinbase Feed");
  display.display();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

}

void loop()
{
    String btc, eth;

    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 50, "Updating...");
    display.display();

    eth = getExchange("ETH-USD");
    btc = getExchange("BTC-USD");
    
    display.clear();
    
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0,10,"ETH");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 10, "$" + eth);

    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0,30,"BTC");
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 30, "$" + btc);
    
    display.display();
  
    delay(30000);
  
}
