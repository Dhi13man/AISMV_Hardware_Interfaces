#include <WiFi.h>
#include <WebServer.h>

#include "aismv_operation.cpp"

class WebServerBackendInterface {
  private:
  WebServer server;
  AISMVOperator *aismv;

  // Define routing
  void restServerRouting() {
      server.on("/", HTTP_GET, [this]() {
          this->server.send(200, F("text/html"),
              F("Welcome to the REST Web Server"));
      });
      server.on(F("/helloWorld"), HTTP_GET, [this] () {
        this->getSoilParameters();
      });
  }

  // Serving Hello world
  void getSoilParameters() {
      server.send(200, "text/json", "{\"targetMoisture\": " + String(aismv->soilController.getSoilParameter('t')) + 
      ", \"moisturePerMilliliter\": " + String(aismv->soilController.getSoilParameter('m')) +  "}");
  }

  // Manage not found URL
  void handleNotFound() {
    String message = "Error 404: Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
  }
 
  public:
  WebServerBackendInterface(AISMVOperator &aismvOperator) {
    // To extract from and set properties into AISMV
    aismv = &aismvOperator;
    // Set Static IP address
    IPAddress local_IP(192, 168, 1, 184);
    // Set Gateway IP address
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 0, 0);
    IPAddress primaryDNS(8, 8, 8, 8);
    IPAddress secondaryDNS(8, 8, 4, 4);
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
      Serial.println("STA Failed to configure");
    WiFi.begin("AISMV_Network", "eminence123");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    // Set up and begin Server
    startServer();
  }

  void startServer() {
    // Set server routing
    restServerRouting();
    // Set not found response
    server.onNotFound([this] () {
      this->handleNotFound();
    });
    // Start server
    server.begin(80);
    Serial.println("HTTP server started");
  }

};
