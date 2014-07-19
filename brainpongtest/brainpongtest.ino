#include "
// data on rows, clock on columns

#define COLUMNS 1
#define ROWS 6
#define LED_PIN 13

// data on port D

uint8_t clock_pins[COLUMNS] = {8};

void clock(uint8_t column) {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(clock_pins[column], HIGH);
  delayMicroseconds(1);
  digitalWrite(clock_pins[column], LOW);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}

void setup() {
  int i;
  
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  
  for (i = 0; i < COLUMNS; i++) {
    pinMode(clock_pins[i], OUTPUT);
    digitalWrite(clock_pins[i], LOW);
  }
  
  DDRD = 0B11111111;
  PORTD = 0B00000000;

}

void loop() {
  int i;
  for (i = 0; i < ROWS; i++) {
    PORTD = 1<<i;
    clock(0);
    delay(150);
  }
  
}

