#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define DIR_PIN 16
#define SPEED_PIN 4

WiFiServer server(80);  // Set up the server on port 80

const char* ssid = "我心繫著你卻新細明體";    // your network SSID
const char* password = "113040045";    // your network password

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";

// write API Key provided by thingspeak
String apiKey = "0CRZ6QTXATQD4OX2";    // your write API key

/* Global variable */
int speed = 0;
int vector = HIGH;
bool isOn = false;

void post(bool on, int vector, int speed);

void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(SPEED_PIN, OUTPUT);

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
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Client connected.");
      String httpRequest = "";  // Store the client's request

      // Wait for the client to send data
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          httpRequest += c;
          if (c == '\n') break;  // Stop reading at newline character
        }
      }

      

      //  speed:0(fast) → 255(slow)
      if (httpRequest.indexOf("GET /?on") >= 0) {
        //  motor on
        isOn = true;
        if(vector == HIGH)  //  正向
        {
          if(speed >= 255)
            speed = 155;
        }
        else
        {
          if(speed <= 0)
            speed = 100;
        }
        digitalWrite(DIR_PIN, vector);
        analogWrite(SPEED_PIN, speed);
        Serial.println("motor ON");
      }
      else if (httpRequest.indexOf("GET /?off") >= 0) {
        //  motor off
        if(vector == HIGH)
          speed = 255;
        else
          speed = 0;
        isOn = false;
        digitalWrite(DIR_PIN, vector);
        analogWrite(SPEED_PIN, speed);
        Serial.println("motor OFF");
      }
      else if (httpRequest.indexOf("GET /?turnAround") >= 0) {
        //  turn around
        if(vector == HIGH)
        {
          vector = LOW;
          digitalWrite(DIR_PIN, LOW);
        }
        else
        {
          vector = HIGH;
          digitalWrite(DIR_PIN, HIGH);
        }
        analogWrite(SPEED_PIN, speed);
        Serial.println("motor TURN AROUND");
      }
      else if (httpRequest.indexOf("GET /?speedUp") >= 0) {
        //  speed up
        if(vector == HIGH)
        {
          if(speed >= 50)
            speed -= 50;
          else
            speed = 0;
        }
        else
        {
          if(speed <= 205)
            speed += 50;
          else
            speed = 255;
        }
        digitalWrite(DIR_PIN, vector);
        analogWrite(SPEED_PIN, speed);
        Serial.println("motor SPEED UP");
      }
      else if (httpRequest.indexOf("GET /?slowDown") >= 0) {
        //  slow down
        if(vector == HIGH)
        {
          if(speed <= 205)
            speed += 50;
          else
            speed = 255;
        }
        else
        {
          if(speed >= 50)
            speed -= 50;
          else
            speed = 0;
        }
          
        digitalWrite(DIR_PIN, vector);
        analogWrite(SPEED_PIN, speed);
        Serial.println("motor SLOW DOWN");
      } 

      //  post to ThingSpeak
      post(isOn, vector, speed);

      
      // HTML response content
      String htmlContent ="<html>"
                          "<title>ESP Web Server</title>"
                          "<body> <h1>ESP32 Web Server</h1>"
                          "<p>motor Status = " + String(digitalRead(DIR_PIN)) + "</p>"
                          "<p>Speed: " + String(speed) + "</p>"
                          "<p> <input type=\"button\" value=\"on\" onclick=\"location.href='/?on'\"> </p>"
                          "<p> <input type=\"button\" value=\"off\" onclick=\"location.href='/?off'\"> </p>"
                          "<p> <input type=\"button\" value=\"turn around\" onclick=\"location.href='/?turnAround'\"> </p>"
                          "<p> <input type=\"button\" value=\"speed up\" onclick=\"location.href='/?speedUp'\"> </p>"
                          "<p> <input type=\"button\" value=\"slow down\" onclick=\"location.href='/?slowDown'\"> </p>"
                          "</body></html>";

      // Send HTTP response
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println();
      client.println(htmlContent);
      // Close the client connection
      client.stop();
      Serial.println("Client disconnected.");
    }
  }
  else {
    Serial.println("WiFi Disconnected");
  }

}



void post(bool on, int vector, int speed)
{
  /* Write to server */
  // Your Domain name with URL path or IP address with path
  WiFiClient client;
  HTTPClient http;
  http.begin(client, serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Data to send with HTTP POST
  String httpRequestData = "api_key=" + apiKey + "&field1=" + String(isOn) + "&field2=" + String(vector) + "&field3=" + String(speed);

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) { //Check for the returning code
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  }
  else{
    Serial.println("Error on HTTP request");
  }
  http.end();  //Free the resources
}