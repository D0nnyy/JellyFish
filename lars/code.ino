#include <Bridge.h>
#include <HttpClient.h>
#include <Process.h>
#include <ArduinoJson.h>

#define ECHO_PIN 13
#define TRIG_PIN 12
#define BUTTON_PIN 11
#define SOUND_PIN A0
#define LED_PIN 7
#define LED_PIN2 6
#define LED_PIN3 5
#define BAUD_RATE 9600

int buttonState = 0;
int ledState = LOW;
unsigned long previousMillis = 0;
const long infraredInterval = 500;
int distance = 0;
int lastSoundState = HIGH;
int currentSoundState = HIGH;
const int soundThreshold = 50;

String serverProtocol = "https://";
String serverIP = "34932.hosts1.ma-cloud.nl";
String serverDirectory = "/jaar2/php/data.php";
String URL = "";

const int capacity = JSON_OBJECT_SIZE(4);
StaticJsonDocument<capacity> doc;
char jsonOut[256];

void setup()
{
  Bridge.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(SOUND_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  lastSoundState = digitalRead(SOUND_PIN);

  if (serverProtocol.length() > 0)
    URL += serverProtocol;
  URL += serverIP;
  if (serverDirectory.length() > 0)
    URL += "/" + serverDirectory;
}

void loop()
{
  button();
  infrared();
  sound();
  sendData();
}

void button()
{
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH)
  {
    digitalWrite(LED_PIN2, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN2, LOW);
  }
}

void infrared()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= infraredInterval)
  {
    previousMillis = currentMillis;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    const unsigned long duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration / 31 / 2;
    if (duration == 0)
    {
      Serial.println("Warning: no pulse from sensor");
    }
    else
    {
      Serial.print("Distance to nearest object: ");
      Serial.print(distance);
      Serial.println(" cm");
    }
    if (distance < 20)
    {
      digitalWrite(LED_PIN, HIGH);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void sound()
{
  lastSoundState = currentSoundState;
  currentSoundState = digitalRead(SOUND_PIN);

  if (lastSoundState == HIGH && currentSoundState == LOW)
  {
    Serial.println("The sound has been detected");
    ledState = !ledState;
    digitalWrite(LED_PIN3, ledState);
  }
}

void sendData()
{
  static unsigned long lastSendTime = 0;
  const unsigned long sendInterval = 2000;
  if (millis() - lastSendTime >= sendInterval)
  {
    generateJson();
    Serial.println("Sending JSON data:");
    Serial.println(jsonOut);

    Process p;
    p.begin("curl");
    p.addParameter("-k");
    p.addParameter("-H");
    p.addParameter("Content-Type: application/json");
    p.addParameter("--data");
    p.addParameter(jsonOut);
    p.addParameter(URL);

    p.run();

    Serial.print("[HTTP] Response: ");
    while (p.available() > 0)
    {
      char c = p.read();
      Serial.print(c);
    }
    Serial.println();

    lastSendTime = millis();
  }
}

void generateJson()
{
  doc.clear();

  doc["Distance"] = distance;
  doc["SoundDetected"] = digitalRead(LED_PIN3);
  doc["ButtonState"] = buttonState;

  digitalWrite(LED_PIN3, LOW);

  serializeJson(doc, jsonOut);
}