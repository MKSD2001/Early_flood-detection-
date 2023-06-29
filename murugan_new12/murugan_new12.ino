
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>
#include <SoftwareSerial.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define TRIGGER_PIN 7
#define ECHO_PIN 6
#define STEPPER_SPEED 10 // adjust to control speed of stepper motor
#define SOUND_SPEED 0.034
long duration;
float distance;
const int stepsPerRevolution = 2038;
const int doorOpenDistance =8; // distance in cm to open door
const int waterReleaseDistance =10; // distance in cm to release water
const int doorCloseDelay = 2000; // delay in ms before closing door after opening
const int raindropPin = A0; // analog input pin for raindrop sensor
float rainLevel = 0;

Stepper stepper(stepsPerRevolution, 8, 10, 9, 11); // initialize stepper motor
SoftwareSerial espSerial(2, 3);
void setup() {
   Serial.begin(9600); // initialize serial communication for debugging
   espSerial.begin(9600);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(13,OUTPUT);
  stepper.setSpeed(STEPPER_SPEED); // set initial speed of stepper motor
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("FLOOD   DETECTION");
  display.display();
  
   
}

void loop() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * SOUND_SPEED/2;

  float sensorValue = analogRead(raindropPin); // read analog input value from raindrop sensor
  Serial.print("Rain level: ");
  Serial.print(sensorValue);
  Serial.println("%");
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,1);
  display.print("Rain level:");
  display.print(sensorValue);
  display.print("%");
  display.display();
  delay(1000);
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm");
 
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Distance:");
  display.print(distance);
  display.display();
   espSerial.print("Rain level: ");
  espSerial.println(rainLevel);
  espSerial.print("Distance:");
  espSerial.println(distance);
  delay(1000);

  if (distance <= doorOpenDistance) { // if distance is greater than or equal to door open distance
    
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Opening door...");
    display.display();
    
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite (13,LOW);
    stepper.step(2*stepsPerRevolution); // move stepper motor forward to open door
    delay(1000);
    delay(doorCloseDelay); // wait before closing door
    while (distance >= waterReleaseDistance) { // keep checking distance until water release distance is reached
      digitalWrite(TRIGGER_PIN, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
      duration = pulseIn(ECHO_PIN, HIGH);
        distance = duration * SOUND_SPEED/2;
       

      delay(1000);
    }
    
    display.clearDisplay();
    display.setTextSize(2); 
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Closing door...");
    display.display();
    
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite (13,LOW);
    stepper.step(2*(-stepsPerRevolution)); // move stepper motor backward to close door
    delay(1000);
  }
  delay(1000);
}
