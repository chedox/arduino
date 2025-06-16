#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Create a web server on port 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for Serial Monitor to open

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 15000; // 15 seconds timeout

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi.");
    // Optional: ESP.restart(); or enter deep sleep
    return;
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  Serial.println("New client connected.");

  String request = "";
  unsigned long timeout = millis() + 1000; // 1 second timeout for incoming data

  while (client.connected() && millis() < timeout) {
    if (client.available()) {
      char c = client.read();
      request += c;
      if (c == '\n' && request.endsWith("\r\n\r\n")) {
        break;
      }
    }
  }

  Serial.println("Request received:");
  Serial.println(request);

  // Send the HTTP response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html>");
  client.println("<head><title>ESP32 Web Server</title></head>");
  client.println("<body><h1>Hello from ESP32!</h1></body></html>");
  client.println();

  delay(10); // Short delay before closing
  client.stop();
  Serial.println("Client disconnected.");
}
