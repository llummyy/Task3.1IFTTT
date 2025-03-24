#include <WiFiNINA.h>
#include <PubSubClient.h>  // MQTT library
#include <BH1750.h>
#include <Wire.h>


// WiFi Credentials
#define WIFI_SSID "WiFi-CF8N"
#define WIFI_PASSWORD "63305234"


// MQTT Broker Credentials
#define MQTT_SERVER "49dcc968ea4a46e28a0488449edcbb8d.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "Sensor/ Motion"

#define MQTT_USER "hivemq.webclient.1742718591343"  // Replace with your HiveMQ username
#define MQTT_PASSWORD "jX.85Ton:B@VH,1gM2da"  // Replace with your HiveMQ password


BH1750 light;


// Use Secure socket layer Client instead of regular WiFiClient
WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);


// Function to Connect to the mqtt broker(hive)
void connectMQTT() {
   while (!mqttClient.connected()) {
       Serial.println("Connecting to MQTT...");
       if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
           Serial.println("Connected to MQTT Broker!");
       } else {
           Serial.print("Failed, rc=");
           Serial.print(mqttClient.state());
           Serial.println(" Retrying in 5 seconds...");
           delay(5000);
       }
   }
}


void setup() {
   Serial.begin(115200);
   delay(1000);
   Wire.begin();
   if (light.begin()) {
        Serial.println("BH1750 Sensor setup");
    } else {
        Serial.println("couldnt set up the sensor");
        while (1); // if fail ends setup
    }


   // Connect to WiFi
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
   }
   Serial.println("Connected to WiFi");


 mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  
   // Connect to MQTT
   connectMQTT();
}


void loop() {
  if (!mqttClient.connected()) {
       connectMQTT();
 }
   mqttClient.loop(); // Keep MQTT connection alive
   float lux = light.readLightLevel();
    Serial.print("Light Intensity: ");
    Serial.print(lux);
    Serial.println(" lx");

    // Publish light intensity directly
    mqttClient.publish(MQTT_TOPIC, String(lux).c_str());
    
    delay(5000); // Send data every 5 seconds
}
