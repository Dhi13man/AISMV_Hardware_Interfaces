#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "aismv_operation.cpp"

class WebServerBackendInterface {
  private:
  AsyncWebServer *server;
  
  AISMVOperator *aismv;

  // Define routing
  void restServerRouting() {
      // State of AISMV
      server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
          request->send(200, F("text/html"), "AISMV STATE: " + aismv->getState());
      });
      // Fetch current parameters
      server->on("/soil-parameters", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"targetMoisture\": " + String(aismv->soilController.getSoilParameter('t')) + 
      ", \"moisturePerMilliliter\": " + String(aismv->soilController.getSoilParameter('m')) +
      ", \"farmDirection\": \"" + String(aismv->soilController.getFarmDirection()) +  "\"}");
      });
      // Update parameters
      server->on("/soil-parameters", HTTP_PATCH, [this] (AsyncWebServerRequest *request) {
        if (request->hasParam("targetMoisture")) {
          AsyncWebParameter *targetMoistureParameter = request->getParam("targetMoisture", false);
          aismv->soilController.updateSoilParameters(targetMoistureParameter->value().toFloat(), 't');
        }
        if (request->hasParam("moisturePerMilliliter")) {
          AsyncWebParameter *moisturePerMilliliter = request->getParam("moisturePerMilliliter", false);
          aismv->soilController.updateSoilParameters(moisturePerMilliliter->value().toFloat(), 'm');
        }
        if (request->hasParam("farmDirection")) {
          AsyncWebParameter *farmDirection = request->getParam("farmDirection", false);
          aismv->soilController.updateFarmDirection((farmDirection->value().c_str())[0]);
        }
      });
  }

  ~WebServerBackendInterface() {
    server->end();
    server->~AsyncWebServer();
  }

  public:
  WebServerBackendInterface(AISMVOperator &aismvOperator) {
    // To extract from and set properties into AISMV
    aismv = &aismvOperator;
    // Set Static IP address
    IPAddress local_IP(192, 168, 1, 184);
    // Set Gateway IP address
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress primaryDNS(8, 8, 8, 8);
    IPAddress secondaryDNS(8, 8, 4, 4);
    // Configures static IP address
    if (!WiFi.softAPConfig(local_IP, gateway, subnet))
      Serial.println("STA Failed to configure");
    WiFi.softAP("AISMV_Network", "eminence123");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.softAPIP());

    // Set up and begin Server
    server = new AsyncWebServer(8080);
    startServer();
  }

  void startServer() { 
    // Set server routing
    restServerRouting();
    // Set not found response
    server->onNotFound([] (AsyncWebServerRequest *request) {
      request->send(404, "text/plain", "Not found");
    });
    // Start server
    server->begin();
    Serial.println("HTTP server started");
  }
};
