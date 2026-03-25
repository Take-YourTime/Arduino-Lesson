#include <WiFi.h>

#define LED_PIN 14


WiFiServer server(80);  // Set up the server on port 80

const char* ssid = "<your network SSID>";    // your network SSID
const char* password = "<your network password>";    // your network password

void setup() {
  pinMode(LED_PIN, OUTPUT);
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

      // Handle LED control requests
      if (httpRequest.indexOf("GET /?led1") >= 0) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      }
      if (httpRequest.indexOf("GET /?led0") >= 0) {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      }

      // HTML response content
      String htmlContent ="<html><title>ESP Web Server</title>"
                          "<body> <h1>ESP32 Web Server</h1>"
                          "<p>LED Status = " + String(digitalRead(LED_PIN)) + "</p>"
                          "<p> <input type=\"button\" value=\"LED ON\" onclick=\"location.href='/?led1'\">"
                          "<input type=\"button\" value=\"LED OFF\" onclick=\"location.href='/?led0'\"></body></html>";

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