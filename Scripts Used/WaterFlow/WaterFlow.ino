/* esp_mqtt_flow - ESP8266 mqtt node with 2 flow sensors as inputs
   Based on DHTServer(ESP8266Webserver, DHTexample, and BlinkWithoutDelay), mqtt_publish_in_callback  (thank you)
   Version 1.0  9/23/2015  Evan Allen
   Part of the code from https://gist.github.com/klanjabrik/
   Pinout as per NodeMCU v3
   Ouput via Mqtt server 
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#define USE_SERIAL Serial
#include <ArduinoJson.h>

// Variable init
const int buttonPin = D2; // variable for D2 pin
const int ledPin = D7;
char push_data[200]; //string used to send info to the server ThingSpeak
int addr = 0; //endereço eeprom
byte sensorInterrupt = 0; // 0 = digital pin 2

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

const char* ssid     = "SETYOURSSID";
const char* password = "SETYOURPASSWORD";
const char* mqtt_server = "SETYOURIP"; // IP Address of MQTT Server
const int mqttPort = 1883;
const char* mqttUser = "***"; // MQTT Auth Username
const char* mqttPassword = "***"; // MQTT Password
const char* clientID = "WaterTap"; // MQTT ClientID
const char* outTopic = "home/pi/datalog"; //Topic for output
const char* inTopic = "home/pi/datalog"; //Topic for input

// Update these with values suitable for your network.
IPAddress server(192, 168, 0, 52); // MQTT Server ip address duplicate don;t know why it is here

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
// Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.println(message);
  if(message == "flow, status"){
    getflow();
    Serial.print("Sending Flowrate:");
    Serial.println(flowRate);
    dtostrf(flowRate , 2, 2, msg);
    client.publish(outTopic, msg);
  }
}

void getflow() {
   detachInterrupt(sensorInterrupt);
   flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
   oldTime = millis();
   flowMilliLitres = (flowRate / 60) * 1000;
   totalMilliLitres += flowMilliLitres;
   unsigned int frac;
   frac = (flowRate - int(flowRate)) * 10;
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void ICACHE_RAM_ATTR pulseCounter();
void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
   while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
     if (client.connect(clientID, mqttUser, mqttPassword )) {
       Serial.println("connected");
     } else {
       Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
     }
  }
  client.setCallback(callback);
 
  // Initialization of the variable “buttonPin” as INPUT (D2 pin)
  pinMode(buttonPin, INPUT);

  // Blinking 1: Connecting to Wifi
  pinMode(ledPin, OUTPUT); 
  
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = 0;
  digitalWrite(buttonPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(buttonPin), pulseCounter, RISING);  
}

void loop()
{
 if (!client.connected()) {
    reconnect();
  }
  
    // Disable the interrupt while calculating flow rate and sending the value to the host
    detachInterrupt(sensorInterrupt);

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
        

        if (flowRate > 0) {
            // Print the flow rate for this second in litres / minute
            Serial.print("Flow rate: ");
            Serial.print(int(flowRate)); // Print the integer part of the variable
            Serial.print("."); // Print the decimal point
            frac = (flowRate - int(flowRate)) * 10;
            Serial.print(frac, DEC); // Print the fractional part of the variable
            Serial.print("L/min");
            // Print the number of litres flowed in this second
            Serial.print("  Current Liquid Flowing: "); // Output separator
            Serial.print(flowMilliLitres);
            Serial.print("mL/Sec");
            // Print the cumulative total of litres flowed since starting
            Serial.print("  Output Liquid Quantity: "); // Output separator
            Serial.print(totalMilliLitres);
            Serial.println("mL");
        
            digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(100);   
             StaticJsonBuffer<300> JSONbuffer;
              JsonObject& JSONencoder = JSONbuffer.createObject();
             
              JSONencoder["device"] = "WaterFlow";
              JSONencoder["status"] = "on";
              JSONencoder["flowrate"] = int(flowRate);
              JSONencoder["currentflow"] = flowMilliLitres;
              JSONencoder["totalml"] = totalMilliLitres;
              char JSONmessageBuffer[100];
              JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
              //Serial.println("Sending message to MQTT topic..");
              Serial.println(JSONmessageBuffer);
             
              if (client.publish(outTopic, JSONmessageBuffer) == true) {
                //Serial.println("Success sending message");
              } else {
                Serial.println("Error sending message");
              }

            digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
            delay(100);              
        } else {
          long now = millis();
            if (now - lastMsg > 20000) {
              ++value;
              lastMsg = now;
              StaticJsonBuffer<300> JSONbuffer;
              JsonObject& JSONencoder = JSONbuffer.createObject();
             
              JSONencoder["device"] = "WaterFlow";
              JSONencoder["status"] = "off";
              JSONencoder["flowrate"] = 0;
              JSONencoder["currentflow"] = 0;
              JSONencoder["totalml"] = totalMilliLitres;
             
              char JSONmessageBuffer[100];
              JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
              //Serial.println("Sending message to MQTT topic..");
              Serial.println(JSONmessageBuffer);
             
              if (client.publish(outTopic, JSONmessageBuffer) == true) {
                //Serial.println("Success sending message");
              } else {
                Serial.println("Error sending message");
              }
            }
        }
        
        // Reset the pulse counter so we can start incrementing again
        pulseCount = 0;

        // Enable the interrupt again now that we've finished sending output
        attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
client.loop();
delay(500);
         
}

/*
Insterrupt Service Routine
 */
void pulseCounter() {
    // Increment the pulse counter
    pulseCount++;
}

void startWIFI(void) {
  
  
}
