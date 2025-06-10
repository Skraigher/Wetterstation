/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server
 */
#include "DHT.h"                //DHT11 Sensor
#include <Adafruit_NeoPixel.h>  // BUILD-in LED
#include <WiFi.h>               // Internet
#include <ESPAsyncWebServer.h>  //WEbserver
//#include <NTPClient.h> //UHrzeit
#include <WiFiUdp.h>  //Uhrzeit
#include <time.h>
#include <HTTPClient.h>

#include "index.h"    // Include the index.h file

#define LED_PIN 8  // für ESP32-C3: GPIO 8 ist oft mit RGB-LED verbunden
#define LED_COUNT 1
#define DHTPIN 2
#define DHTTYPE DHT11
//ADC pin für Gas-Sensor
#define SENSOR_PIN 4

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "IOT";  // CHANGE IT
const char* password = "20tgmiot18";             // CHANGE IT

//const char* ssid = "FRITZ!Box 7590 FK";  // CHANGE IT
//const char* password = "94190408373391741977";             // CHANGE IT

//const char* ssid = "MoTiKr_Net_Stock1_2GEXT";  // CHANGE IT
//const char* password = "42458180";             // CHANGE IT

// Zeitzone (z. B. Österreich: GMT+1 → 3600 Sekunden, im Sommer GMT+2 → 7200 Sekunden)
const long gmtOffset_sec = 3600;        // Standardzeit
const int daylightOffset_sec = 3600;    // Sommerzeit

const char* serverName = "http://127.0.0.1:8080/upload.php"; 

//http://127.0.0.1:8080/show_data.php



#define COLOR_RED pixel.Color(255, 0, 0)
#define COLOR_ORANGE pixel.Color(255, 165, 0)
#define COLOR_BLUE pixel.Color(0, 0, 255)
#define COLOR_OFF pixel.Color(0, 0, 0)

//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);


bool ledState = true;



AsyncWebServer server(80);




void setup() {
  Serial.begin(9600);
  // LED initialisieren


  pixel.begin();
  pixel.setBrightness(50);
  pixel.show();


  // Sensor initialisieren
  dht.begin();
  //Zeit

  



  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN...");

  int timeout = 10000;
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
    pixel.setPixelColor(0, COLOR_RED);  // Rot: kein WLAN
    pixel.show();
    delay(500);
    pixel.setPixelColor(0, COLOR_OFF);  // Blinken
    pixel.show();
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Verbunden!");
  } else {
    Serial.println("WLAN-Verbindung fehlgeschlagen.");
  }

  // Print the ESP32's IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());


  //timeClient.begin();
  //timeClient.setTimeOffset(3600);
  // Zeit mit NTP-Server synchronisieren
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  Serial.println("Warte auf Zeitdaten...");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Fehler beim Abrufen der Zeit");
    delay(1000);
  }

  Serial.println("Zeit erfolgreich abgerufen!");
  
  

  // Serve the HTML page from the file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");                                    // for debugging

    request->send(200, "text/html", webpage);
  });

  // Define a route to get the temperature data
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
    // for debugging
    // for debugging
    float temperature = dht.readTemperature();

    String temperatureStr = String(temperature, 2);
    request->send(200, "text/plain", temperatureStr);
  });
  //luftfeuchtigkeit ausgeben
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest* request) {
    // for debugging
    // for debugging
    float humidity = dht.readHumidity();

    String humidityStr = String(humidity, 2);
    request->send(200, "text/plain", humidityStr);
  });

  server.on("/sensorValue", HTTP_GET, [](AsyncWebServerRequest* request) {
    // for debugging
    // for debugging
    float sensorValue = getSensorValue();
    String humidityStr = String(sensorValue, 2);
    request->send(200, "text/plain", humidityStr);
  });
  server.on("/sensorVolt", HTTP_GET, [](AsyncWebServerRequest* request) {
    // for debugging
    // for debugging
    float sensorVolt = getSensorVolt();
    String humidityStr = String(sensorVolt, 2);
    request->send(200, "text/plain", humidityStr);
  });
  server.on("/sensorRatio", HTTP_GET, [](AsyncWebServerRequest* request) {
    // for debugging
    // for debugging
    float sensorRatio = getSensorRatio();
    String humidityStr = String(sensorRatio, 2);
    request->send(200, "text/plain", humidityStr);
  });
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest* request) {
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        request->send(500, "text/plain", "Zeit konnte nicht geladen werden");
        return;
    }

    char timeString[64];
    strftime(timeString, sizeof(timeString), "Datum: %d.%m.%Y  Zeit: %H:%M:%S", &timeinfo);

    request->send(200, "text/plain", timeString);
});
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasParam("state")) {
      String state = request->getParam("state")->value();

      if (state == "on") {

        ledState = true;
      } else {

        ledState = false;
      }

      pixel.show();
      request->send(200, "text/plain", ledState ? "LED ist AN" : "LED ist AUS");
    } else {
      request->send(400, "text/plain", "Fehlender Parameter");
    }
  });

  // LED-Status abrufen
  server.on("/ledStatus", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", ledState ? "LED ist AUS" : "LED ist AN");
  });

  // Start the server
  server.begin();


}
/*
String printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Zeit konnte nicht abgerufen werden.");
    
  }

  return (&timeinfo, "Datum: %d.%m.%Y  Zeit: %H:%M:%S");
}*/



void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    float temp = dht.readTemperature();
    float luftfeuchtigkeit = dht.readHumidity();
    float sensor = getSensorValue();

    // Prüfen ob Messwerte gültig sind, sonst Dummywerte setzen
    if (isnan(temp)) {
      temp = 25.0;  // Dummy Temperatur
    }
    if (isnan(luftfeuchtigkeit)) {
      luftfeuchtigkeit = 50.0;  // Dummy Luftfeuchtigkeit
    }
    if (isnan(sensor)) {
      sensor = 0.0;  // Dummy Sensorwert
    }

    String httpRequestData = "temperatur=" + String(temp, 2) +
                             "&luftfeuchtigkeit=" + String(luftfeuchtigkeit, 2) +
                             "&sensor=" + String(sensor, 2);

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  


  if (ledState == true) {


    if (WiFi.status() != WL_CONNECTED) {
      pixel.setPixelColor(0, COLOR_RED);      // Orange: Temperatur zu hoch
    } else if (dht.readTemperature() > 30) {  //dht.readTemperature()
      pixel.setPixelColor(0, COLOR_ORANGE);   // Orange: Temperatur zu hoch

    } else {
      pixel.setPixelColor(0, COLOR_BLUE);  // Blau: Messung normal
    }

    

    



  } else {
    pixel.setPixelColor(0, COLOR_OFF);  // LED aus
  }
  /**
  while(!timeClient.update()) {
  timeClient.forceUpdate();
  }
  **/







  pixel.show();

  delay(5000);
}
/**
String zeitauslesen(int i){
  if(i == 1){
      formattedDate = timeClient.getFormattedTime();
      return formattedDate;
  }else if(i == 2){
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    return dayStamp;
  }else if(i == 3){
    // Extract time
    int splitT = formattedDate.indexOf("T");
    
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    return timeStamp;
  }
  return "NULL";      
}**/

float getSensorValue() {
  float sensorValue = 0;
  sensorValue += analogRead(SENSOR_PIN);
  //sensorValue = sensorValue / 100.0;
  return sensorValue;
}
float getSensorVolt() {
  float sensor_volt;
  sensor_volt = getSensorValue() / 4096 * 3.3;
  return sensor_volt;
}
float getSensorRatio() {
  float RS_air;
  float R0;
  float RS_gas;
  float ratio;
  RS_air = (3.3 - getSensorVolt()) / getSensorVolt();
  R0 = RS_air / 9.8;  // Bestimme R0 (Referenzwert in Luft)
  RS_gas = (3.3 - getSensorVolt()) / getSensorVolt();
  ratio = RS_gas / R0;  // Berechne das Verhältnis Rs/R0
  return ratio;
}