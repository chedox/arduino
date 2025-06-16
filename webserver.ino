#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Create a web server on port 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // listen for incoming clients

  if (client) {
    Serial.println("New client connected.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Web page content
            client.println("<!DOCTYPE html><html>");
            client.println("<head><title>ESP32 Web Server</title></head>");
            client.println("<body><h1>Hello from ESP32!</h1></body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    // Close the connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
