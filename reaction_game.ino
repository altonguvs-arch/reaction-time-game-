#include <Arduino.h>
#include <TM1637Display.h> // The new display library

// Pin Configurations
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 8; 

// Display Module Pins
const int CLK_PIN = 10;
const int DIO_PIN = 11;

// Initialize the display object
TM1637Display display(CLK_PIN, DIO_PIN);

// Your 5 LEDs in physical order from left to right
const int ledPins[] = {3, 4, 5, 6, 7}; 
const int totalLeds = 5;
const int TARGET_LED_INDEX = 2; // The Green LED in the middle

// Game Settings
int currentLed = 0;
int direction = 1; 
int lastButtonState = HIGH;
int score = 0; // Tracks your current win streak!

// Speed Control variables
unsigned long lastMoveTime = 0;
int startSpeed = 300;     
int currentSpeed = 300;   
int speedStep = 40;       
int maxSpeed = 40;        

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  for (int i = 0; i < totalLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); 
  }
  
  // Initialize Display settings
  display.setBrightness(0x0f); // Set to maximum brightness
  display.showNumberDec(score); // Show the starting score of 0
  
  // Boot test: Flash all 5 LEDs
  flashAllLeds(3, 100);
}

void loop() {
  unsigned long currentTime = millis();

  // --- 1. MOVE THE RUNNING LIGHT ---
  if (currentTime - lastMoveTime >= currentSpeed) {
    lastMoveTime = currentTime;
    digitalWrite(ledPins[currentLed], LOW);
    currentLed += direction;
    if (currentLed >= totalLeds - 1 || currentLed <= 0) {
      direction = -direction;
    }
    digitalWrite(ledPins[currentLed], HIGH);
  }

  // --- 2. CHECK THE BUTTON PRESS ---
  int buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == LOW && lastButtonState == HIGH) {
    if (currentLed == TARGET_LED_INDEX) {
      
      // --- 🎉 WIN SEQUENCE ---
      score++; // Add 1 to your streak!
      display.showNumberDec(score); // Update the screen instantly
      
      currentSpeed -= speedStep; 
      if (currentSpeed < maxSpeed) {
        currentSpeed = maxSpeed; 
      }
      
      digitalWrite(ledPins[currentLed], LOW); 
      winSequence();
      
    } else {
      
      // --- ❌ LOSE SEQUENCE ---
      score = 0; // Reset streak back to zero
      display.showNumberDec(score); // Update the screen instantly
      
      currentSpeed = startSpeed; 
      loseSequence();
    }
    
    lastMoveTime = millis();
  }
  
  lastButtonState = buttonState;
}

void winSequence() {
  digitalWrite(BUZZER_PIN, HIGH); 
  for (int i = 0; i < totalLeds; i++) digitalWrite(ledPins[i], HIGH);
  delay(250); 
  
  digitalWrite(BUZZER_PIN, LOW); 
  for (int i = 0; i < totalLeds; i++) digitalWrite(ledPins[i], LOW);
  delay(500); 
}

void loseSequence() {
  delay(400);
  digitalWrite(ledPins[currentLed], LOW); 
  
  digitalWrite(BUZZER_PIN, HIGH); delay(80);
  digitalWrite(BUZZER_PIN, LOW);  delay(80);
  digitalWrite(BUZZER_PIN, HIGH); delay(80);
  digitalWrite(BUZZER_PIN, LOW);
  delay(400);
}

void flashAllLeds(int times, int speedMs) {
  for (int t = 0; t < times; t++) {
    for (int i = 0; i < totalLeds; i++) digitalWrite(ledPins[i], HIGH);
    delay(speedMs);
    for (int i = 0; i < totalLeds; i++) digitalWrite(ledPins[i], LOW);
    delay(speedMs);
  }
}
