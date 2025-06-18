#include <WiFi.h>

// Replace with your actual WiFi credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";

WiFiServer server(80);

bool isRunning = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 15000;

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi.");
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
  unsigned long timeout = millis() + 1000;

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

  if (request.indexOf("GET /toggle") >= 0) {
    isRunning = !isRunning;
    Serial.print("isRunning: ");
    Serial.println(isRunning ? "true" : "false");
  }

  // Send the HTTP response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<style>");
  client.println("body { font-family: Arial; text-align: center; padding-top: 50px; }");
  client.println(".switch { position: relative; display: inline-block; width: 60px; height: 34px; }");
  client.println(".switch input { opacity: 0; width: 0; height: 0; }");
  client.println(".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;");
  client.println("background-color: #ccc; transition: .4s; border-radius: 34px; }");
  client.println(".slider:before { position: absolute; content: \"\"; height: 26px; width: 26px; left: 4px; bottom: 4px;");
  client.println("background-color: white; transition: .4s; border-radius: 50%; }");
  client.println("input:checked + .slider { background-color: #2196F3; }");
  client.println("input:checked + .slider:before { transform: translateX(26px); }");
  client.println("</style></head><body>");

  client.println("<h1>ESP32 Toggle Switch</h1>");
  client.println("<form action=\"/toggle\" method=\"GET\">");
  client.print("<label class=\"switch\">");
  client.print("<input type=\"submit\" name=\"toggle\" style=\"display:none\">");
  client.print("<input type=\"checkbox\" ");
  if (isRunning) client.print("checked");
  client.println(" onclick=\"this.form.submit()\">");
  client.println("<span class=\"slider\"></span>");
  client.println("</label>");
  client.println("</form>");

  client.print("<p>Status: <strong>");
  client.print(isRunning ? "Running" : "Stopped");
  client.println("</strong></p>");

  client.println("</body></html>");

  delay(10);
  client.stop();
  Serial.println("Client disconnected.");
}
