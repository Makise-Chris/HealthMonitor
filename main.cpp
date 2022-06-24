#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
 
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

float heartRate;
float bodyTemperature;
float bloodPressure;
String deviceId;
 
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to wifi");
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  //client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["deviceId"] = deviceId;
  doc["heartRate"] = heartRate;
  doc["bodyTemperature"] = bodyTemperature;
  doc["bloodPressure"] = bloodPressure;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
void setup()
{
  Serial.begin(115200);
  connectAWS();
}
 
void loop()
{
  deviceId=WiFi.macAddress();
  heartRate=80.96;
  bodyTemperature=37.02;
  bloodPressure=123.66;

  Serial.print(F("Device Id:  "));
  Serial.print(deviceId);
  Serial.print(F("  Heart rate: "));
  Serial.print(heartRate);
  Serial.print(F("bps  Body temperature: "));
  Serial.print(bodyTemperature);
  Serial.print(F("°C   BloodPressure: "));
  Serial.print(bloodPressure);
  Serial.println("mmHg");
 
  publishMessage();
  client.loop();
  delay(1000);
}