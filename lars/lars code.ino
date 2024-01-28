#define ECHO_PIN 13
#define TRIG_PIN 12
#define BUTTON_PIN 11
#define SOUND_PIN A0
#define LED_PIN 7
#define LED_PIN2 6
#define LED_PIN3 5
#define BAUD_RATE 9600

int buttonState = 0;
int val = 0;
int lastSoundState;
int currentSoundState;
int ledState = LOW;
unsigned long previousMillis = 0;
const long infraredInterval = 500; // Interval for the infrared function in milliseconds
int distance = 0; // Declare distance as a global variable

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(SOUND_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  Serial.begin(BAUD_RATE);
  currentSoundState = digitalRead(SOUND_PIN);
}

void loop() {
  button();
  infrared();
  sound();
}

void button() {
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) {
    digitalWrite(LED_PIN2, HIGH);
  } else {
    digitalWrite(LED_PIN2, LOW);
  }
}

void infrared() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= infraredInterval) {
    previousMillis = currentMillis;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    const unsigned long duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration / 31 / 2;
    if (duration == 0) {
      Serial.println("Warning: no pulse from sensor");
    } else {
      Serial.print("distance to nearest object: ");
      Serial.print(distance);
      Serial.println("cm");
    }
    if (distance < 20) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void sound() {
  lastSoundState = currentSoundState;
  currentSoundState = digitalRead(SOUND_PIN);

  if (lastSoundState == HIGH && currentSoundState == LOW) {
    Serial.println("The sound has been detected");
    ledState = !ledState;
    digitalWrite(LED_PIN3, ledState);
  }
}
