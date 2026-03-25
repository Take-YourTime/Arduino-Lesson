//  post

#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <Arduino_JSON.h>

#define DHTPIN 27         //  degital pin connected to sensor
#define DHTTYPE DHT11     //  define DHT type
DHT dht(DHTPIN, DHTTYPE); //  initia;ize DHT sensor

#define BUTTONPIN 26      //  button pin
#define LEDPIN 14         //  LED pin

const char* ssid = "yuchen";           // your network SSID
const char* password = "Samis666"; // your network password

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";

// write API Key provided by thingspeak
String apiKey = "2ZYB6W8W453ZU8N6";    // your write API key

bool isLED = false;
int buttonstate = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  dht.begin();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(LEDPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
  digitalWrite(LEDPIN, LOW);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {  //Check the current connection status
    WiFiClient client;
    HTTPClient http;
    delay(10000); // wait for 10 seconds

    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();  //  temperature
    float h = dht.readHumidity();     //  humidity
    buttonstate = digitalRead(BUTTONPIN);

    if (isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor! t"));
      return;
    }

    if(isnan(h))
    {
      Serial.println(F("Failed to read from DHT sensor! h"));
      return;
    }

    

    //  detect LED
    /*
    if(buttonstate == HIGH)
    {
      Serial.println("save\n");
      digitalWrite(LEDPIN, HIGH);
      isLED = true;
    }*/
    if(t > 30.0 || h < 40.0)
    {
      digitalWrite(LEDPIN, HIGH);
      isLED = true;
    }
    else
    {
      digitalWrite(LEDPIN, LOW);
      isLED = false;
    }
    
    if(buttonstate == LOW)
    {
      if(isLED)
      {      
        digitalWrite(LEDPIN, LOW);
        isLED = false;
      }
      else
      {
        digitalWrite(LEDPIN, HIGH);
        isLED = true;

      }

      http.begin("https://api.thingspeak.com/channels/2713622/fields/1/last.json?api_key=TMVV03M26AKD8862"); //Specify the URL
      
      int Response = http.GET();  //Make the request
      if (Response > 0) { //Check for the returning code
  
        String payload = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(Response);
        Serial.println(payload);  // Print JSON response

        // Parse JSON response using Arduino_JSON library
        JSONVar jsonData = JSON.parse(payload);
        // Check if parsing was successful
        if (JSON.typeof(jsonData) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }
        // Extract the value of field1
        String field1Value = jsonData["field1"];
        Serial.print("Field1 value temperature: ");
        Serial.println(field1Value);  // Print the value of field1
      }
      http.end();

      http.begin("https://api.thingspeak.com/channels/2713622/fields/2/last.json?api_key=TMVV03M26AKD8862"); //Specify the URL
      
      Response = http.GET();  //Make the request
      if (Response > 0) { //Check for the returning code
        String payload = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(Response);
        Serial.println(payload);  // Print JSON response

        // Parse JSON response using Arduino_JSON library
        JSONVar jsonData = JSON.parse(payload);
        // Check if parsing was successful
        if (JSON.typeof(jsonData) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }
        // Extract the value of field1
        String field2Value = jsonData["field2"];
        Serial.print("Field2 value humnity: ");
        Serial.println(field2Value);  // Print the value of field1
      }
      http.end();

      http.begin("https://api.thingspeak.com/channels/2713622/fields/3/last.json?api_key=TMVV03M26AKD8862"); //Specify the URL
      
      Response = http.GET();  //Make the request
      if (Response > 0) { //Check for the returning code
  
        String payload = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(Response);
        Serial.println(payload);  // Print JSON response

        // Parse JSON response using Arduino_JSON library
        JSONVar jsonData = JSON.parse(payload);
        // Check if parsing was successful
        if (JSON.typeof(jsonData) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }
        // Extract the value of field1

        String field3Value = jsonData["field3"];
        Serial.print("Field3 value LED: ");
        Serial.println(field3Value);  // Print the value of field1
      }
      http.end();
    }
    else {
      Serial.println("Error on HTTP request");
    }

    
    Serial.println(t);
    Serial.println(h);
    Serial.println(isLED);

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Data to send with HTTP POST
    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(t);
    String httpRequestData_h = "api_key=" + apiKey + "&field2=" + String(h);
    String httpRequestData_LED = "api_key=" + apiKey + "&field3=" + String(isLED);

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    int hResponse = http.POST(httpRequestData_h);
    int LEDResponse = http.POST(httpRequestData_LED);

    if (httpResponseCode > 0) { //Check for the returning code
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else{
      Serial.println("Error on HTTP request");
    }

    http.end();  //Free the resources
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}
