#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Minimal boot test start");
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
}
