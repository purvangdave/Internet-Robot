/*
ESP8266 WiFi Robot Car
Motor Driver : L298N
Board : NodeMCU ESP8266
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// ==========================
// WiFi and MQTT Settings
// ==========================
const char* ssid = "Dhruv's A35";
const char* password = "dhruv2813";
const char* mqtt_topic = "my_car";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
// ==========================
// Motor Pins
// ==========================
// Right Motor
#define RIGHT_MOTOR_IN1 D8
#define RIGHT_MOTOR_IN2 D7
// Left Motor
#define LEFT_MOTOR_IN3 D4
#define LEFT_MOTOR_IN4 D3
// Enable Pins for PWM Speed Control
#define ENA D5
#define ENB D6
// Motor speed
// ESP8266 PWM range = 0 to 1023
int carSpeed = 700;
// Current movement state
char currentCommand = 'S';
WiFiClient espClient;
PubSubClient client(espClient);
// ==========================
// WiFi Connection
// ==========================
void setup_wifi()
{
delay(10);
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.mode(WIFI_STA);

WiFi.begin(ssid, password);

while(WiFi.status() != WL_CONNECTED)
{
    delay(500);
    Serial.print(".");
}

Serial.println();
Serial.println("WiFi Connected");

Serial.print("IP Address: ");
Serial.println(WiFi.localIP());
}
// ==========================
// MQTT Callback
// ==========================
void callback(char* topic, byte* payload, unsigned int length)
{
String message;
for(unsigned int i = 0; i < length; i++)
{
    message += (char)payload[i];
}

Serial.print("Received: ");
Serial.println(message);

// Speed control command
if(message.startsWith("V"))
{
    carSpeed = message.substring(1).toInt();

    Serial.print("Speed Updated: ");
    Serial.println(carSpeed);

    executeCommand(currentCommand);
}

// Direction command
else if(message.length() == 1)
{
    currentCommand = message[0];

    executeCommand(currentCommand);
}
}
// ==========================
// Execute Robot Commands
// ==========================
void executeCommand(char cmd)
{
if(cmd == 'F')
{
moveForward();
}
else if(cmd == 'B')
{
    moveBackward();
}

else if(cmd == 'L')
{
    turnLeft();
}

else if(cmd == 'R')
{
    turnRight();
}

else if(cmd == 'S')
{
    stopCar();
}
}
// ==========================
// MQTT Reconnect
// ==========================
void reconnect()
{
while(!client.connected())
{
Serial.print("Attempting MQTT Connection...");
    String clientId = "NodeMCUClient-";
    clientId += String(random(0xffff), HEX);

    if(client.connect(clientId.c_str()))
    {
        Serial.println("Connected");

        client.subscribe(mqtt_topic);
    }

    else
    {
        Serial.print("Failed, rc=");
        Serial.print(client.state());

        Serial.println(" Retrying in 5 seconds");

        delay(5000);
    }
}
}
// ==========================
// Setup
// ==========================
void setup()
{
Serial.begin(115200);
// Motor direction pins
pinMode(RIGHT_MOTOR_IN1, OUTPUT);
pinMode(RIGHT_MOTOR_IN2, OUTPUT);

pinMode(LEFT_MOTOR_IN3, OUTPUT);
pinMode(LEFT_MOTOR_IN4, OUTPUT);

// PWM enable pins
pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);

// Stop motors at startup
stopCar();

// Connect WiFi
setup_wifi();

// MQTT setup
client.setServer(mqtt_server, mqtt_port);

client.setCallback(callback);
}
// ==========================
// Main Loop
// ==========================
void loop()
{
if(!client.connected())
{
reconnect();
}
client.loop();
}
// ==========================
// Movement Functions
// ==========================
void moveForward()
{
Serial.println("Moving Forward");
analogWrite(ENA, carSpeed);
analogWrite(ENB, carSpeed);

digitalWrite(RIGHT_MOTOR_IN1, HIGH);
digitalWrite(RIGHT_MOTOR_IN2, LOW);

digitalWrite(LEFT_MOTOR_IN3, HIGH);
digitalWrite(LEFT_MOTOR_IN4, LOW);
}
void moveBackward()
{
Serial.println("Moving Backward");
analogWrite(ENA, carSpeed);
analogWrite(ENB, carSpeed);

digitalWrite(RIGHT_MOTOR_IN1, LOW);
digitalWrite(RIGHT_MOTOR_IN2, HIGH);

digitalWrite(LEFT_MOTOR_IN3, LOW);
digitalWrite(LEFT_MOTOR_IN4, HIGH);
}
void turnLeft()
{
Serial.println("Turning Left");
analogWrite(ENA, carSpeed);
analogWrite(ENB, carSpeed);

digitalWrite(RIGHT_MOTOR_IN1, HIGH);
digitalWrite(RIGHT_MOTOR_IN2, LOW);

digitalWrite(LEFT_MOTOR_IN3, LOW);
digitalWrite(LEFT_MOTOR_IN4, HIGH);
}
void turnRight()
{
Serial.println("Turning Right");
analogWrite(ENA, carSpeed);
analogWrite(ENB, carSpeed);

digitalWrite(RIGHT_MOTOR_IN1, LOW);
digitalWrite(RIGHT_MOTOR_IN2, HIGH);

digitalWrite(LEFT_MOTOR_IN3, HIGH);
digitalWrite(LEFT_MOTOR_IN4, LOW);
}
void stopCar()
{
Serial.println("Stopped");
analogWrite(ENA, 0);
analogWrite(ENB, 0);

digitalWrite(RIGHT_MOTOR_IN1, LOW);
digitalWrite(RIGHT_MOTOR_IN2, LOW);

digitalWrite(LEFT_MOTOR_IN3, LOW);
digitalWrite(LEFT_MOTOR_IN4, LOW);
}
