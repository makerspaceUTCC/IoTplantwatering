// WaterPlant OpenHouse
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
int Pump = 16;
int analogPin = 0;
int Val = 0;
int NB_BUTTON = 0;
int NB_STOP = 0 ;
int NB_AUTO = 0;
const char *ssid = "Wifi ssid";
const char *password = "password";

// MQTT
const char *mqtt_broker = "mqtt broker server address";
const char *topic1 = "topic001";
const char *mqtt_username = "username";
const char *mqtt_password = "password";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);
String client_id = "Arduino-UTCC";

#define SQL1 "insert into nb_data values (null,"
#define SQL2 ","
#define SQL3 ",null)"
#define SID 30004

void callback(char* topic1, byte* payload, unsigned int length);
void callback(char* topic1, byte* payload, unsigned int length) {
  
if ((char)payload[12] == 'o' && (char)payload[13] == 'n') {
NB_BUTTON = 1; 
NB_STOP = 0;
NB_AUTO = 0;
}
else if ((char)payload[12] == 'o' && (char)payload[13] == 'f') {
NB_BUTTON = 0; 
NB_STOP = 1; 
NB_AUTO = 0;
}
else if ((char)payload[12] == 'a' && (char)payload[13] == 'u') {
NB_BUTTON = 0; 
NB_STOP = 0 ;
NB_AUTO = 1;
}
}
void setup() {
  Serial.begin(9600);
  pinMode(Pump, OUTPUT);
  digitalWrite(Pump,LOW);
  pinMode(analogPin, INPUT);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
 Serial.println(WiFi.localIP());

  Serial.println("Connected to the WiFi network");

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  
  while (!client.connected()) {
      
     // client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         
         // client.subscribe(topic); //old 21/12/2564
          client.subscribe(topic1); 
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }   
  }
     
}
void loop() {
client.loop();
if(NB_BUTTON == 1){   
     digitalWrite(Pump , HIGH);
     delay(100);
     Serial.println("Water planting Grafana");
}
 else if(NB_STOP == 1){
     digitalWrite(Pump , LOW);
     delay(100);
     Serial.println("Stop planting Grafana");
     
}
  
  Val = analogRead(analogPin);
  if(NB_STOP == 0 && NB_BUTTON == 0){
  if(Val > 600){
     digitalWrite(Pump , HIGH);
     Serial.println("water planting");
     delay(10000);
     digitalWrite( Pump , LOW);
     Serial.println("stop planting");
  }
 }
  Serial.print("Val_Sensor = ");
  Serial.print(Val);
  Serial.println(); 

  char sql[256];                                                       
    snprintf(sql, sizeof sql, "%s%d%s%d%s%d%s%d%s%d%s", SQL1, SID, SQL2, Val, SQL2, 0, 
    SQL2,0 , SQL2,0 , SQL3);
     Serial.print(sql);
  if (client.publish(sql, "Suphanburi") == true) {
      //Serial.print("Successful sending: ");
      Serial.println(sql);
    } 
    else {
      //Serial.println("Failed sending.");
    }
  delay(3000);
}
