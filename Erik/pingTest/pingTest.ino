/*
 *   Trehjulet robot fra forårskursus 2018
 *   rettet 30 marts 2018
 *   sidst rettet 24 april 2018
 *   created marts 2018
 *   by Erik Christesen
 *   
 *   Bestykning:
 *   Arduino UNO controller
 *   To DC-motorer styret af et 
 *   Adafruit piggy back shield
 *   
 *   tre ping sensorer i front
 *   Den midterste regulerer hastighede i tre trin
 *   De to i siderne observerer forhindringer
 *   
 *   Struktur:
 *   
 *   includes
 *   create objects
 *   constants
 *   global variabels
 *   setup
 *   
 *   loop
 *   
 *   readPingC
 *   setSpeedC
 *   readPings
 *   drive
 *   reverse
 *   turnLeft
 *   turnRight
 *   stopM
 *   reduceSpeed
 *   increaseSpeed
 *   calcSpeedL
 *   
 */
#include <Servo.h> 
#include <AFMotor.h>

AF_DCMotor motorR(4);
AF_DCMotor motorL(3);
Servo servo01;    // create servo object to control a servo 

// These constants won't change.  They're used to give names
// to the pins used:             //============================= CONSTANTS ===  
const int trigLpin = A0;         // IR trigger pin Left
const int echoLpin = A1;         // IR echo pin Left
const int trigCpin = A2;         // IR trigger pin Center
const int echoCpin = A3;         // IR echo pin Center
const int trigRpin = A4;         // IR trigger pin Right
const int echoRpin = A5;         // IR echo pin Right

const int servoPin = 9;          // Servopin

const int minPing = 15;          // Min threshold for ping in cm
const int maxPing = 35;          // Max threshold for ping in cm

const int minSpeed = 75;
const int medSpeed = 100;
const int maxSpeed = 175;

const int wait = 50;

                              // =============================== Global variables ====
int duration = 0;             // time for ping
int cm = 0;                   // Ping dist in cm
int timeout = 5000;           // timeout for ping ca. 85 cm
int servoValue = 0;           // value output to the servo
int speedR = medSpeed;        // basis-speed sættes altid på højre motor
int biasM = 95;               // difference-faktor mellem R og L  
int speedL = (speedR * biasM)/100;// Venstre motor beregnes

void setup() {                // ==================================== SETUP ===
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  servo01.attach(servoPin);
  // turn on motor
  motorR.setSpeed(speedR);
  motorR.run(RELEASE);
  motorL.setSpeed(speedL);
  motorL.run(RELEASE);
  // set Ping I/O
  pinMode(trigLpin, OUTPUT);
  pinMode(echoLpin, INPUT);
  pinMode(trigCpin, OUTPUT);
  pinMode(echoCpin, INPUT);
  pinMode(trigRpin, OUTPUT);
  pinMode(echoRpin, INPUT);
} // end setup

void loop() {                         //====================================== LOOP ===
  Serial.print("pingTest C - R - L\t");

  setSpeedC();                        // regulerer speedR efter ping C
  calcSpeedL();
  drive();

  int pingState = readPings();
  while (pingState > 0) {
    reduceSpeed();
    delay(100);
    switch (pingState) {
      case 1:
        turnRight();
        break;
      case 2:
        turnLeft();
        break;
      case 3:
        stopM();
        reverse();
        turnLeft();
        break;
    } // end switch
    pingState = readPings();
  } // end while
  
  delay(wait*3);
} // end loop

void readPingC() {                    //================================ readPingC == 
    digitalWrite(trigCpin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigCpin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigCpin, LOW);
    duration = pulseIn(echoCpin, HIGH, timeout); // if > timeout then = 0
  }while (duration < 1);   // end do .. while
  cm = (duration / 58);
  Serial.print(cm);
  Serial.println(" cm\t");
} // end readPingC  

void setSpeedC() {                    //================================ setSpeedC ==
  readPingC();                        // læser pingC i cm
  if (cm < cm > maxPing) {            // 0 eller > max
    increaseSpeed();                  // increase speedR
  } else {
    if (cm < minPing) {               // pingC nærmere end minPing
      stopM();                        // STOP
      delay(wait*2);
      reverse();
      do {
        readPingC();
        delay(wait);
      } while(cm < (minPing + 10));    // bak indtil pingC er minPing + 10 cm
      stopM();                         // STOP
      delay(wait*2);
    } else { 
      reduceSpeed();
    }  // end if
  }
  calcSpeedL();
} // setSpeedC

int readPings() {                   //===================================== readPings ==
  int state = 0;
  long duration = 0;
  do {
    digitalWrite(trigRpin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigRpin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigRpin, LOW);
    duration = pulseIn(echoRpin, HIGH, timeout);
  } while (duration < 1);
  cm = (duration / 58);
  Serial.print(cm);
  Serial.print(" cm\t");
  if (cm < maxPing) {reduceSpeed();}  //reduce speedR
  if (cm < minPing) state = 2;
  
  digitalWrite(trigLpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigLpin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigLpin, LOW);
  duration = pulseIn(echoLpin, HIGH, timeout);
  cm = (duration / 58);
  Serial.print(cm);
  Serial.print(" cm\t");
  if (cm > 0 && cm < maxPing) {reduceSpeed();}  //reduce speedR
  if (cm > 0 && cm < minPing) state = state+1;
  return state;
} // end readPings

void drive() {                  //=================================== drive ===
  motorR.run(FORWARD);
  motorL.run(FORWARD);
  motorR.setSpeed(speedR);  
  motorL.setSpeed(speedL);  
  delay(200);
  Serial.print("Forward speed R \t");
  Serial.print(speedR);
  Serial.print("L \t");
  Serial.println(speedL);
} // end drive
 
void reverse() {                //=================================== reverse ===
  speedR = minSpeed;
  calcSpeedL();
  motorR.run(BACKWARD);
  motorL.run(BACKWARD);
  motorR.setSpeed(speedR);  
  motorL.setSpeed(speedL);  
  delay(200);
  Serial.print("Bakwardspeed R \t");
  Serial.print(speedR);
  Serial.print("L \t");
  Serial.println(speedL);
} // end reverse
 
void stopM() {                  // ============================== stopM =========
  motorR.setSpeed(0);  
  motorL.setSpeed(0);
  delay(500);
}

void turnLeft() {               // ============================== turnLeft ======
  motorR.setSpeed(maxSpeed);  
  motorL.setSpeed(minSpeed);  
  Serial.print("Left turn \t");
  delay(10);
}

void turnRight() {               // ============================== turnRight ======
  motorR.setSpeed(minSpeed);  
  motorL.setSpeed(maxSpeed);  
  Serial.print("Right turn speed R \t");
  Serial.print(minSpeed);
  Serial.print(" L \t");
  Serial.println(maxSpeed);
  delay(100);
}

void increaseSpeed() {          //=============================== increaseSpeed ===
  if (speedR <= minSpeed) { 
    speedR = medSpeed;
  } else {
    speedR = maxSpeed;
  } // endif
} // end increase

void reduceSpeed() {            // ================================= reduceSpeed ===
  if (speedR <= medSpeed) {        
    speedR = minSpeed;
  }else {
    speedR = medSpeed;          
  }
} //end reduce

void calcSpeedL() {               // =============================== calcSpeedL ===
  speedL = (speedR*biasM)/100;
}

// ========================================  LAST LINE ============================
