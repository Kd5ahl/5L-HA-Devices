#include <wifi_config.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Mapping
// Wemos / Arduino
// D0 = 16
// D1 = 5
// D2 = 4
// D3 = 0
// D4 = 2
// D5 = 14
// D6 = 12
// D7 = 13
// D8 = 15

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 14

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

// arrays to hold device addresses. Replace with actual values
DeviceAddress Master    = { 0x28, 0xFF, 0x4C, 0x25, 0x50, 0x17, 0x04, 0xE9 );
DeviceAddress Parents   = { 0x28, 0xFF, 0x3A, 0x26, 0x50, 0x17, 0x04, 0x90 );
DeviceAddress Freya     = { 0x28, 0xFF, 0x7E, 0xB0, 0x33, 0x17, 0x04, 0xCF );
DeviceAddress Bath      = { 0x28, 0xFF, 0x80, 0x8B, 0x50, 0x17, 0x04, 0x56 );
DeviceAddress Kitchen   = { 0x28, 0xFF, 0x05, 0xAC, 0x33, 0x17, 0x04, 0xF8 );
DeviceAddress Stairs    = { 0x28, 0xFF, 0xEF, 0x1A, 0x33, 0x17, 0x03, 0x84 );
DeviceAddress Living    = { 0x28, 0xFF, 0xB9, 0x5A, 0x40, 0x17, 0x03, 0x66 );
DeviceAddress Dining_A  = { 0x28, 0xFF, 0x5C, 0x8A, 0x33, 0x17, 0x04, 0xE7 );
DeviceAddress Dining_B  = { 0x28, 0xFF, 0xAF, 0x2B, 0xA1, 0x16, 0x04, 0x8A );
DeviceAddress Inlet     = { 0x28, 0xFF, 0x37, 0x2B, 0x50, 0x17, 0x04, 0xE1 );
DeviceAddress Outlet    = { 0x28, 0xFF, 0x2F, 0x2C, 0x50, 0x17, 0x04, 0x25 );
DeviceAddress Coil      = { 0x28, 0xFF, 0x63, 0x2A, 0xA1, 0x16, 0x03, 0x85 );



// values defined in wifi_config.h
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* mqtt_server = MQTT_SERVER;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg        = 0;
float MasterTemp    = 0;
float ParentsTemp   = 0;
float FreyaTemp     = 0;
float BathTemp      = 0;
float KitchenTemp   = 0;
float StairsTemp    = 0;
float LivingTemp    = 0;
float Dining_ATemp  = 0;
float Dining_BTemp  = 0;
float InletTemp     = 0;
float OutletTemp    = 0;
float CoilTemp      = 0;
char* payload       = "";
char* data          = "";

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  WiFi.hostname("ENV-AC-01");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("WineCoolerTempSensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  sensors.begin();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 300000) {
    lastMsg = now;
    sensors.setResolution(12);
    sensors.requestTemperatures(); // Send the command to get temperatures
  
  MasterTemp = sensors.getTempF(Master);
  ParentsTemp = sensors.getTempF(Parents);
  FreyaTemp = sensors.getTempF(Freya);
  BathTemp = sensors.getTempF(Bath);
  KitchenTemp = sensors.getTempF(Kitchen);
  StairsTemp = sensors.getTempF(Stairs);
  LivingTemp = sensors.getTempF(Living);
  Dining_ATemp = sensors.getTempF(Dining_A);
  Dining_BTemp = sensors.getTempF(Dining_B);
  InletTemp = sensors.getTempF(Inlet);
  OutletTemp = sensors.getTempF(Outlet);
  CoilTemp = sensors.getTempF(Coil);
  
  
  // We want our JSON String to be as follows: {"MasterTemp": MasterTemp_value,"ParentsTemp": ParentsTemp_value, ... "CoilTemp": CoilTemp_value}
  String payload="{\"MasterTemp\":"+String(MasterTemp)+ ",\"ParentsTemp\":"+String(ParentsTemp)+ ",\"FreyaTemp\":"+String(FreyaTemp)+ ",\"BathTemp\":"+String(BathTemp)+ ",\"KitchenTemp\":"+String(KitchenTemp)+ ",\"StairsTemp\":"+String(StairsTemp)+ ",\"LivingTemp\":"+String(LivingTemp)+ ",\"Dining_ATemp\":"+String(Dining_ATemp)+ ",\"Dining_BTemp\":"+String(Dining_BTemp)+ ",\"InletTemp\":"+String(InletTemp)+ ",\"OutletTemp\":"+String(OutletTemp)+ ",\"CoilTemp\":"+String(CoilTemp)+"} ";
  
  payload.toCharArray(data, (payload.length() + 1));
  client.publish("home/AC-01",data);
  
    
  }
}
