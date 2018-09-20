/*
   xjob Oliver Nilsson

   system to create a Wizard of Oz experiment using multiple vibration systems to
   augment a game controller

   (c) 2017 Malmo University Sweden

*/

int buttonPins [] = {13, 12, 11, 10, 9, 8};
int ledPins[] = {7, 2};
int motorPins[] = {6, 5, 3};
int potPin = A5;

long timerSerial = 0; // use to check when to send over serial
long timerMotor = 0; // use as a timer to turn motors on/off for stimuli to users
long timer = 0; // use as a timer to decide when to change the motors behaviours
int potVal = 0; // store the value of the potentiometer
int buttonVal[] = {1, 1, 1, 1 , 1, 1};
int buttonsPressed[] = {0, 0, 0, 0, 0, 0};
int buttonsReleased[] = {0, 0, 0, 0, 0, 0};
int buttonValOld[] = {1, 1, 1, 1 , 1, 1};
int ledState = 0; // to store whether the leds are off (0) or on (1)
int motorState[] = {0, 0, 0}; // to store whether the motors are off (0) or on (1)

void setup() {
  // declare inputs
  for (int i = 0; i < 6; i++)
    pinMode(buttonPins[i], INPUT_PULLUP);

  // declare leds
  for (int i = 0; i < 2; i++)
    pinMode(ledPins[i], OUTPUT);

  // declare motors
  for (int i = 0; i < 3; i++)
    pinMode(motorPins[i], OUTPUT);

  // start the serial communication to debug the system
  Serial.begin(9600);

  // start counting time
  timer = millis();
  timerSerial = millis();
  timerMotor = millis();

}

void loop() {
  // read all of the inputs and send the data to the serial port
  for (int i = 0; i < 6; i++) {
    buttonVal[i] = digitalRead(buttonPins[i]);
    buttonsPressed[i] |= buttonPressed(i);
    buttonsReleased[i] = buttonReleased(i);
    buttonsPressed[i] &= !buttonReleased(i);
    if (buttonsPressed[i]) {
      Serial.print("button pressed: ");
      Serial.println(i);
    }
    if (buttonsReleased[i]) {
      Serial.print("button released: ");
      Serial.println(i);
    }
  }
  potVal = analogRead(potPin);

  if (millis() - timerSerial > 250) {
    for (int i = 0; i < 6; i++) {
      Serial.print(buttonVal[i]);
      Serial.print(" ");
    }
    Serial.print(potVal);
    Serial.println();
    timerSerial = millis();
  }

  // use the LEDs based on the value on the potentiometer
  if (millis() - timer > potVal) {
    for (int i = 0; i < 2; i++)
      digitalWrite(ledPins[i], ledState);
    ledState = !ledState;
    timer = millis();
  }

  // use the button information to operate the motors
  buttonMotorOn(0, 0);
  buttonMotorOn(2, 1);
  buttonMotorOn(4, 2);

  if (millis() - timerMotor > potVal || buttonPressed(1) || buttonPressed(3) || buttonPressed(5)) {
    if (buttonsPressed[1])
      motorState[0] = !motorState[0];
    if (buttonsPressed[3]) {
      Serial.print("motor 1: "); Serial.println(motorState[1]);
      motorState[1] = !motorState[1];
    }
    if (buttonsPressed[5]) {
      Serial.print("motor 2: "); Serial.println(motorState[2]);
      motorState[2] = !motorState[2];
    }
    timerMotor = millis();
  }


  // check if the buttons for oscillating signals were released
  if (buttonsReleased[1])
    motorState[0] = 0;
  if (buttonsReleased[3])
    motorState[1] = 0;
  if (buttonsReleased[5])
    motorState[2] = 0;

  // copy the button values in the old values in order to detect pressed and released events
  for (int i = 0; i < 6; i++)
    buttonValOld[i] = buttonVal[i];

  // write the status to all of the motors
  for (int i = 0; i < 3; i++)
    digitalWrite(motorPins[i], motorState[i]);
}

int buttonMotorOn(int pinIn, int pinOut) {
  int ret = 0;

  // use the button information to operate the motors
  if (buttonsPressed[pinIn] == 1) {
    ret = 1;
    motorState[pinOut] = HIGH;
  }
  if (buttonsReleased[pinIn]) {
    motorState[pinOut] = LOW;
  }

  return ret;
}

int buttonPressed(int pinPos) {
  int ret = 0;

  if (buttonVal[pinPos] == LOW && buttonValOld[pinPos] == HIGH)
    ret = 1;

  return ret;
}

int buttonReleased(int pinPos) {
  int ret = 0;

  if (buttonVal[pinPos] == HIGH && buttonValOld[pinPos] == LOW)
    ret = 1;

  return ret;
}

