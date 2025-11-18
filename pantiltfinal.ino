#include <Servo.h>
/*
FOR WIRING:
Pan:
CH1 to Pin 2
Pin 6 to Pan servo (servo1)
CH4 to pin 3
Pin 7 to Tilt servo (servo4)

TO BE DELETED JUST INFO FOR SELF:
CH1/2 right joy stick, right left n up n down
CH3/4 left joy stick, up down n right left
*/

Servo servo1;
Servo servo2;
Servo servo4;

int ch1Pin = 2;  // right stick left/right (pan)
int ch4Pin = 3;  // left stick left/right (tilt)
int ch2Pin = 4; // right stick up/down (trigger)

unsigned long ch1Value;
unsigned long ch4Value;
unsigned long ch2Value;

/* 
This bellow can be modified
this is the angle from central that you want it to pan (pan is moving right n left)
similar for tilt, tilt is up n down 
*/
// set constants
int halfanglerangepan = 30;
int anglerangetilt = 30;
int triggerThreshold = 1700; //when joy stick pushed passed itll shoot contiously till its pulled to less

//trigger vars to delay without stopping everything:
bool triggerActive = false;
unsigned long triggerStart = 0;
int stopTime = 1000; //to modify to find reliable delay

// get max n min angles based of constants
int maxpan = 90 + halfanglerangepan;
int minpan = 90 - halfanglerangepan;
int maxtilt = 30 + anglerangetilt;
int mintilt = 30;

//dont worry abt, just for processing input to output
int lastAngle1 = 0;
int lastAngle4 = 0;
int maxsig = 1850;
int minsig = 1150;
int thresholdpan = 3;
int thresholdtilt = 3;

// temp

unsigned long previousMillis = 0;
const long interval = 1000; 
int angle = 10;

void setup() {

  //say where info is coming n going p much
  pinMode(ch1Pin, INPUT);
  pinMode(ch4Pin, INPUT);
  pinMode(ch2Pin, INPUT);
  servo1.attach(6); //pan
  servo4.attach(7); // tilt
  servo2.attach(5); //trigger

  // for debugging
  Serial.begin(9600);
}

void loop() {

  unsigned long currentMillis = millis(); // temp

  ch1Value = pulseIn(ch1Pin, HIGH, 40000); // read right stick
  delay(10);
  ch4Value = pulseIn(ch4Pin, HIGH, 40000); // read left stick
  delay(10);
  ch2Value = pulseIn(ch2Pin, HIGH, 40000); // read trigger


  //guards for maps in case of a glitch or smthn DELETE IF SOMETHING GOES WRONG
  if (ch1Value == 0) ch1Value = 1500;
  if (ch4Value == 0) ch4Value = 1500;
  if (ch2Value == 0) ch2Value = 1500;

  // calculate the angle based on how far from the center the joy stick is
  int angle1 = map(constrain(ch1Value, minsig, maxsig), minsig, maxsig, minpan, maxpan);
  int angle4 = map(constrain(ch4Value, minsig, maxsig), minsig, maxsig, mintilt, maxtilt);

  //write pan and tilt angles
  if (abs(angle1-lastAngle1) > thresholdpan) {
    servo1.write(angle1);
    lastAngle1 = angle1;
  }

  if (abs(angle4-lastAngle4) > thresholdtilt) {
    servo4.write(angle4);
    lastAngle4 = angle4;
  }

  // contious shooting
  /*
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // reset timer

    // move the servo
    angle = (angle == 10) ? 170 : 10; // toggle between 0 and 90 degrees
    servo2.write(angle);
  }
  */

  //shooting the gun from remote
  
  if (!triggerActive && ch2Value > triggerThreshold) {
    // Start the trigger movement
    triggerActive = true;
    triggerStart = millis(); //need a delay to allow the servo to reach angle (to fully pull trigger)
    servo2.write(180);   // fire
  }

  if (triggerActive) {
    if (millis() - triggerStart >= stopTime) {
        // 1 second has passed
        servo2.write(30);    // reset
        triggerActive = false;
    }
  }
  
  //terminal to figure out bug (there was a lot)
  Serial.print("\t");
  Serial.print("CH1: ");
  Serial.print(ch1Value);
  Serial.print("-> angle1: ");
  Serial.print(angle1);
  Serial.print("\tCH4: ");
  Serial.println(ch4Value);
  Serial.print("-> angle4: ");
  Serial.print(angle4);
  Serial.print("\tCH2: ");
  Serial.print(ch2Value);
  Serial.print("-> Trigger: ");
  Serial.print(triggerActive);
  delay(100);
}
