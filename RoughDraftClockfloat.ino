#include <Servo.h>

const int btnForwardPin = 2;
const int lightForwardPin = 3;
const int btnReversePin = 5;
const int lightReversePin = 6;
const int limitStartPin = 8;
const int limitEndPin = 9;

const float optimalForward =20.6; //as a %
const float optimalReverse = -100; //percent
int forwardSpeed = optimalForward;
int reverseSpeed = optimalReverse;
int sliderPosit;
boolean btnForward = false;
boolean btnReverse = false;
boolean limitStart = false;
boolean limitEnd = false;
int maxOffset = 13;
struct TIMER {
  unsigned long stamp;
  unsigned long duration;
  bool state;
};
TIMER ledBlink {0, 100, 0};  // timer for LED to blink with 100 msec
TIMER blinkBlink {0, 500, 0}; // timer to control LED blinking with 500 msec
unsigned long currentTime;

Servo clockServo;

void setup() {
  // put your setup code here, to run once:
  clockServo.attach(11);  // attaches the servo on pin 9 to the servo object

  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(btnForwardPin, INPUT);
  pinMode(btnReversePin, INPUT);
  pinMode(limitStartPin, INPUT);
  pinMode(limitEndPin, INPUT);
  pinMode(lightReversePin, OUTPUT);
  pinMode(lightForwardPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSensors();
  //    printStatus();

  if (btnForward && btnReverse) {
    driveMotor(0);
    blink(lightForwardPin, lightReversePin);
  } else if (btnForward ) {
    lightOff(lightReversePin);
    if (limitEnd == false) {
      lightOn(lightForwardPin);
      driveMotor(forwardSpeed);
    }
    if (limitEnd == true) {
      blink(lightForwardPin, -1);
      driveMotor(0);
    }
  } else if (btnReverse) {
    lightOff(lightForwardPin);
    if (limitStart == false) {
      lightOn(lightReversePin);
      driveMotor(reverseSpeed);
    }
    if (limitStart == true) {
      blink(lightReversePin, -1);
      driveMotor(0);
    }
  } else {
    lightOff(lightForwardPin);
    lightOff(lightReversePin);
    driveMotor(0);
  }

}

void printStatus() {
  Serial.println(sliderPosit);
  Serial.println(forwardSpeed);
  Serial.println(reverseSpeed);
  Serial.println(btnForward);
  Serial.println(btnReverse);
  Serial.println(limitStart);
  Serial.println(limitEnd);
  Serial.println("");
  delay(1000);
}

void updateSensors() {
  btnForward = digitalRead(btnForwardPin);
  btnReverse = digitalRead(btnReversePin);
  limitStart = digitalRead(limitStartPin);
  limitEnd = digitalRead(limitEndPin);
  sliderPosit = analogRead(A0);
  mapSliderPosit();
}
void mapSliderPosit() {
  forwardSpeed = map(sliderPosit, 0, 1023, -maxOffset, +maxOffset);
  forwardSpeed = optimalForward + forwardSpeed;
  if  (forwardSpeed < 0) {
    forwardSpeed = 0;

  }
  else if (forwardSpeed > 100) {
    forwardSpeed = 100;
  }
}
void blink(int pin, int pin2) {
  currentTime = millis();
  // timer to control blinking LED
  if (currentTime - blinkBlink.stamp >= blinkBlink.duration)   {
    blinkBlink.stamp = currentTime;
    ledBlink.state = !ledBlink.state;
    digitalWrite(pin, ledBlink.state );
    if (pin2 > -1)
      digitalWrite(pin2, ledBlink.state);
  }
}

void lightOff(int pin) {
  digitalWrite(pin, LOW);
}

void lightOn(int pin) {
  digitalWrite(pin, HIGH);
}
void driveMotor(int speed) {
  speed = map(speed, -100, 100, 50, 138);
  clockServo.write(speed);
}
