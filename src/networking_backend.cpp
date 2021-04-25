#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "aismv_operation.cpp"

/// Interface Handling the IoT communication between application and Hardware. 
///
/// Utilizes Static IP.
class WebServerBackendInterface {
  private:
  AsyncWebServer *server;
  
  AISMVOperator *aismv;

  /// Define routing
  void restServerRouting() {
      // State of AISMV
      server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
          request->send(200, F("text/html"), "AISMV STATE: " + aismv->getState());
      });
      // Fetch current parameters
      server->on("/api/soil-parameters", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"targetMoisture\": " + String(aismv->soilController.getSoilParameter('t')) + 
      ", \"moisturePerMilliliter\": " + String(aismv->soilController.getSoilParameter('m')) +
      ", \"farmDirection\": \"" + String(aismv->soilController.getFarmDirection()) +  "\"}");
      });
      // Update parameters
      server->on("/api/update", HTTP_GET, [this] (AsyncWebServerRequest *request) {
        if (request->hasParam("targetMoisture")) {
          AsyncWebParameter *targetMoistureParameter = request->getParam("targetMoisture");
          aismv->soilController.updateSoilParameters(targetMoistureParameter->value().toFloat(), 't');

          request->send(200, "text/plain", "Target Moisture Updated!");
        }
        if (request->hasParam("moisturePerMilliliter")) {
          AsyncWebParameter *moisturePerMilliliter = request->getParam("moisturePerMilliliter");
          aismv->soilController.updateSoilParameters(moisturePerMilliliter->value().toFloat(), 'm');
          request->send(200, "text/plain", "Moisture Per Milliliter Updated!");
        }
        if (request->hasParam("farmDirection")) {
          AsyncWebParameter *farmDirection = request->getParam("farmDirection");
          aismv->soilController.updateFarmDirection((farmDirection->value().c_str())[0]);
          request->send(200, "text/plain", "Starting Directions Updated!");
        }
      });
  }

  public:
  WebServerBackendInterface(AISMVOperator &aismvOperator) {
    /// To extract from and set properties into AISMV
    aismv = &aismvOperator;
    // Set Static IP address
    IPAddress local_IP(192, 168, 1, 184);
    // Set Gateway IP address
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    // Configures static IP address
    if (!WiFi.softAPConfig(local_IP, gateway, subnet))
       Serial.println("STA Failed to configure");
    WiFi.softAP("AISMV_Comm", "eminence123");
    //Serial.println(WiFi.localIP());
    //Serial.println(WiFi.softAPIP());

    // Set up and begin Server
    server = new AsyncWebServer(8080);
    startServer();
  }

  /// Sets the routes and starts the server.
  void startServer() { 
    // Set server routing
    restServerRouting();
    // Set not found response
    server->onNotFound([] (AsyncWebServerRequest *request) {
      request->send(404, "text/plain", "Not found");
    });
    // Start server
    server->begin();
    //Serial.println("HTTP server started");
  }
};
