#import <Servo.h>

Servo servo;
int angle = 60;

void setup() {
  Serial.begin(9600);
  pinMode(D7, INPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, INPUT);
  servo.attach(D5);
  servo.write(0);
  delay(20000);  
}

void loop() {
  int pirState = digitalRead(D7);

  if(pirState == HIGH) {
    digitalWrite(D4, HIGH);
    Serial.println("Motion Detected!");
    openLid(1000);
    delay(1000);
  } else {
    digitalWrite(D4, LOW);
    Serial.println("No Motion Detected!");
  }
}

void openLid(int del) {
  servo.write(angle);
  delay(del);
  servo.write(0);
}
