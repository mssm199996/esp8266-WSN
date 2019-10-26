#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>


const char* ssid = "Mssm network";
const char* password = "mssm1996";

float   t = 0 ;
float   h = 0 ;
String Stemp,Shum;

// Création des objets / create Objects
DHT dht(D9, DHT11);

String URLtemp, URLHum;


HTTPClient httpTemp;
HTTPClient httpHum;


void setup() {
 
  Serial.begin(115200);                 
  WiFi.begin(ssid, password); 
    
 
  httpTemp.begin("http://192.168.43.214:80/onTemperatureCollectedCallback");     
  httpTemp.addHeader("Content-Type", "application/x-www-form-urlencoded"); 

  httpHum.begin("http://192.168.43.214:80/onHumidityCollectedCallback");     
  httpHum.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
 
  while (WiFi.status() != WL_CONNECTED) {  
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }
 
}
 
void loop() {
 
 if(WiFi.status()== WL_CONNECTED){ 

   t = dht.readTemperature();
   h = dht.readHumidity();

   Serial.println(t);
   Serial.println(h);
   
   char str[30];
   Stemp = String(t);
   Shum = String(h);
  
   int httpCodeTemp = httpTemp.POST("value="+Stemp);
   int httpCodeHum = httpHum.POST("value="+Shum);
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(1000); 
 
}