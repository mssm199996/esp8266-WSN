#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

// -------------------------------------------------------------------------------------------------------------------------------------------------

#define NETWORK_SSID      "Mssm network"
#define NETWORK_PASSWORD  "mssm1996"

// -------------------------------------------------------------------------------------------------------------------------------------------------

const uint8_t bufferSize = 10;
const uint8_t ledController = D5;

String humidityData[bufferSize];
String temperatureData[bufferSize];

uint8_t humidityIndex = 0;
uint8_t temperatureIndex = 0;

// -------------------------------------------------------------------------------------------------------------------------------------------------

ESP8266WebServer server(80);

// -------------------------------------------------------------------------------------------------------------------------------------------------

void onTemperatureCollectedCallback() {
  String value = onObservationRequestCallback();

  if(!value.equals("")) {
    Serial.print("Saving new temperature: ");
    Serial.println(value);
    
    if(temperatureIndex >= bufferSize)
      temperatureIndex = 0;

    temperatureData[temperatureIndex] = value;

    temperatureIndex += 1;
  }
}

void onHumidityCollectedCallback() {
  String value = onObservationRequestCallback();

  if(!value.equals("")) {
    Serial.print("Saving new humidity: ");
    Serial.println(value);
    
    if(humidityIndex >= bufferSize)
      humidityIndex = 0;

    humidityData[humidityIndex] = value;

    humidityIndex += 1;
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void getTemperatureDatasetCallback() {
  server.send(200, "application/json", arrayAsJson(temperatureData, temperatureIndex));
}

void getHumidityDatasetCallback() {
  server.send(200, "application/json", arrayAsJson(humidityData, humidityIndex));
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void onNotFoundCallback() {
  server.send(404, "text/html", "MSSM-MUS Server: You're trying to access an unavailable resource ! <br/>Please try again later !");
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  pinMode(ledController, OUTPUT);

  WiFi.begin (NETWORK_SSID, NETWORK_PASSWORD);

  Serial.begin (115200);  
  Serial.println("Connecting to Wifi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay (500); 
    
    Serial.print(".");
  }

  Serial.println("");
  
  Serial.print("Connected to: "); 
  Serial.println(NETWORK_SSID);
  
  Serial.print("Server IP: "); 
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
  }
  
  server.serveStatic("/", SPIFFS, "/index.html");
  
  server.on("/onTemperatureCollectedCallback", HTTP_POST, onTemperatureCollectedCallback);
  server.on("/onHumidityCollectedCallback", HTTP_POST, onHumidityCollectedCallback);

  server.on("/getTemperatureDataset", HTTP_GET, getTemperatureDatasetCallback);
  server.on("/getHumidityDataset", HTTP_GET, getHumidityDatasetCallback);
  
  server.onNotFound(onNotFoundCallback);

  server.begin();
  
  Serial.println("HTTP Server Started");
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  server.handleClient();
 
  delay(100);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

String onObservationRequestCallback() {
  if(server.hasArg("value")) {
    server.send(200, "text/plain", "MSSM-MUS Server: OK !");
    
    return server.arg("value");
  } else server.send(400, "text/plain", "MSSM-MUS Server: Error... bad request !");

  return "";
}

String arrayAsJson(String dataset[], uint8_t datasize) {
  String response = "[\n";

  for(int i = 0; i < datasize; i++) {
    response += "\t{\n\t\t\"index\": \"" + String(i + 1) + "\",\n\t\t\"value\": \"" + dataset[i] + "\"\n\t},\n";
  }

  response = response.substring(0, response.length() - 2);
  
  return response + "]";
}
