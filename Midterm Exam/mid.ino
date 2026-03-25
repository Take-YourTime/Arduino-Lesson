#include <WiFi.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>

#define led 14
#define pin1 32 //  light sensor
#define pin2 33 //  button

WiFiServer server(80);  // Set up the server on port 80

const char* ssid = "我心繫著你卻新細明體";    // your network SSID
const char* password = "113040045";    // your network password

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";

// write API Key provided by thingspeak
String apiKey = "TZK50THANG5L4BLG";    // your write API key

int sensorValue = 0;
int lightValue = 0;
int buttonState = 0;
bool islight = true;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(pin2, INPUT);

  Serial.begin(115200);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  

  // Start the server
  server.begin();
  Serial.println("Server started.");
}

void loop() {
  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    // Check if a new client has connected   
    WiFiClient client;
    HTTPClient http;
    delay(500);
    //  detection
    buttonState = digitalRead(pin2);  //  read button
    if(buttonState == HIGH)  //  press
    {
      Serial.print("Press Button, ");
      islight = !islight;

      HTTPClient http;
      http.begin("https://api.thingspeak.com/channels/2746477/fields/1/last.json?api_key=QB5PQ8B7MGP0GSE3"); //Specify the URL
      
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

        String field1Value = jsonData["field1"];
        Serial.print("Field1 value lightSensor: ");
        Serial.println(field1Value);  // Print the value of field1

        HTTPClient http;
        http.begin("https://api.thingspeak.com/channels/2746477/fields/2/last.json?api_key=QB5PQ8B7MGP0GSE3"); //Specify the URL
        
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

          String field2Value = jsonData["field2"];
          Serial.print("Field2 value LED: ");
          Serial.println(field2Value);  // Print the value of field1

          HTTPClient http;
          http.begin("https://api.thingspeak.com/channels/2746477/fields/3/last.json?api_key=QB5PQ8B7MGP0GSE3"); //Specify the URL
          
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

            String field3Value = jsonData["field3"];
            Serial.print("Field3 value press: ");
            Serial.println(field3Value);  // Print the value of field1
          }
        }
      }
      http.end();
    }
    else
    {
      Serial.print("Unpress Button, ");
    }
    

    if(islight)
    {
      sensorValue = analogRead(pin1);
      Serial.print("pin1光敏電阻:");
      Serial.print(sensorValue);

      lightValue = map(sensorValue, 0, 4100, 0, 255);
      Serial.print(" ,led:");
      Serial.println(lightValue);
      analogWrite(led, lightValue);
    }
    else
    {
      //  trun off LED
      analogWrite(led, 0);
    }
    
    
    
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(sensorValue) + "&field2=" + String(lightValue) + "&field3=" + String(buttonState);
    
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode <= 0) { //Check for the returning code
      Serial.println("Error on HTTP request");
    }
    
    http.end();
    // Close the client connection
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}