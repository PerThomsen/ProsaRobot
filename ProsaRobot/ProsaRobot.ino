/*
 * Robotbyggeprojekt af 'Elektronik & Mekanik' februar 2018
 * 
 * Styring af trehjulet autonom robotbil. 
 * Undgå forhindringer og undgå at køre ud over en kant
 *  
 * Sensorer:
 * 3 Ping-sensor (ultralyd). 1 placeret centralt fortil
 * og 2 placeret i hvert af de forreste hjørner 
 * 
 * Program struktur:
 *  GLOBAL SETTINGS
 *    Benforbindelser
 *    Konstanter
 *    Globale variabler
 *  SETUP
 *  INVERTER
 *  RMP (Måler antal flanker. 40 = 1 omgang)
 *  STOP MOTOR
 *  KØR FORLÆNS
 *  KØR BAGLÆNS
 *  TURN LR (Drej venstre eller højre)
 *  SPIN (Spin venstre eller højre)
 *  HASTIGHED
 *  PING STATE
 *  LOOP (Hovedprogram)
 *  
 */

                                                  //=============== GLOBAL SETTINGS
                                        //=============== Benforbindelser
// Sætter ben til hastighedssensorer 
#define encoderInV 8 // input venstre
#define encoderInH 9 // input højre 

// Pingben
#define trigPin 7
#define echoPin 6

// Venstre motor benforbindelser
#define MOTOR_L_PWM 11 // PIN D11 --> MOTOR B+ / PWM Speed (IA2) GUL
#define MOTOR_L_DIR 13 // PIN D13 --> MOTOR B  / Retning (IB2) ORANGE
// Højre motor benforbindelser
#define MOTOR_R_PWM 10 // PIN D10 --> MOTOR B+ / PWM Speed (IA1) GRÅ
#define MOTOR_R_DIR 12 // PIN D12 --> MOTOR B  / Retning (IB1) HVID

                                        //=============== Konstanter 
// Prefixed hastigheder
#define PWM_SLOW 100  // Fastsætter lav hastighed PWM duty cycle
#define PWM_MID 150 // Fastsætter medium hastighedd PWM duty cycle
#define PWM_FAST 200 // Fastsætter hurtig hastighed PWM duty cycle
#define DIR_DELAY 1000 // Kort delay for at gøre motor klar til ændringer

// Retningskonstanter
#define M_FORWARD LOW
#define M_REVERSE HIGH

                                        //=============== Globale variabler
int bias = 0; // Kompensation til højre motor (for at den kører ligeud)

// RMP variabler
int detectStateV=0; // Variabel til aflæsning af venstre encoder status
int detectStateH=0; // Variabel til aflæsning af højre encoder status
int lastStateV;     // Venstre - Sidste status 
int lastStateH;     // Højre - Sidste status
int newStateV;      // Venstre - Ny status
int newStateH;      // Højre - Ny status
int counterV;       // Tæller flanker fra venstre encoder
int counterH;       // Tæller flanker fra højre encoder

int pingState = 0;

                                                   //=============== SETUP
void setup() {
  // Init forbindelse til seriel monitor
  Serial.begin( 9600 );
  
  // Init venstre motor
  pinMode( MOTOR_L_DIR, OUTPUT );
  pinMode( MOTOR_L_PWM, OUTPUT );
  digitalWrite( MOTOR_L_DIR, LOW );
  digitalWrite( MOTOR_L_PWM, LOW );

  // Init højre motor
  pinMode( MOTOR_R_DIR, OUTPUT );
  pinMode( MOTOR_R_PWM, OUTPUT );
  digitalWrite( MOTOR_R_DIR, LOW );
  digitalWrite( MOTOR_R_PWM, LOW );

  // Init encoder
  pinMode(encoderInV, INPUT); //Sæt ben 8 som input
  pinMode(encoderInH, INPUT); //Sæt ben 9 som input

  // Init ping
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  
}

                                                  // INVERTER
int invertOurValue(int input) {
  return 255 - input;
}

                                                  // RMP
void measureRMP() { 
  // Aflæs omdrejninger (RPM Measurement)
  detectStateV=digitalRead(encoderInV);
  detectStateH=digitalRead(encoderInH);
  //counter++;
  
  if (detectStateV == HIGH) { //Hvis V-encoder output er high
    newStateV = 1;
  } else {
    newStateV = 0;          
  }
  if (detectStateH == HIGH) { //Hvis H-encoder output er high
    newStateH = 1;
  } else {
    newStateH = 0;          
  }
  
  if (lastStateV == newStateV) {
    if (lastStateV == 0) lastStateV = 1; else lastStateV = 0;
    counterV++;
  }
  
  if (lastStateH == newStateH) {
    if (lastStateH == 0) lastStateH = 1; else lastStateH = 0;
    counterH++;
  }
  bias = 0;
  if (counterV != counterH) {
    bias = (counterV - counterH);
  }
  //bias = 20;
  /*
  Serial.print("Cnt1: ");  
  Serial.print(counterV);
  Serial.print(" \tCnt2: ");  
  Serial.print(counterH);
  Serial.print(" \tBias: ");
  Serial.println(bias);
  /*/

  //bias = (bias + 1) * 2;
  bias = (bias * 3) + 10;
  //*/
  if (counterV == 20) {
    counterV = 0;
    counterH = 0;
    //delay(1000);
  }
}

                                                  // STOP MOTOR
void stopMotor() {
  // Altid stoppe motoren kortvarigt, for at gøre den klar til ændringer
  digitalWrite( MOTOR_L_DIR, LOW );
  digitalWrite( MOTOR_L_PWM, LOW );
  digitalWrite( MOTOR_R_DIR, LOW );
  digitalWrite( MOTOR_R_PWM, LOW );
}

                                                  // KØR FORLÆNS
void runFW(){
  // Kør forlæns
  // stopMotor();
  // delay( DIR_DELAY );
  speed(PWM_FAST, PWM_FAST + bias, M_FORWARD);
}

                                                  // KØR BAGLÆNS
void runREW(){
  // Kør baglæns
  // stopMotor();
  // delay( DIR_DELAY );
  speed(PWM_FAST, PWM_FAST + bias, M_REVERSE);
}

                                                  // TURN LR
void turnLR() {
  // Turn left or right
  // Der skal modtages 2 parametre
  // L/R og faktor for hastighedsforskel
  // kald speed med de respektive paramertre
  switch (bias) {
    case 5:
      // 45 grd
      break;
    case 10:
      // 90 grd
      break;
    case 20:
      // 180 grd
      break;
    default:
      // Ingenting
      break;
  }  
}

                                                  // SPIN LR
void spin() {
  // Spin left or right
  // Der skal modtages 1 parameter (L/R)
  // Motorene sættes til at køre modsat hinanden
  // digitalWrite( MOTOR_L_DIR, M_FORWARD ); 
  // digitalWrite( MOTOR_R_DIR, M_REVERSE );    
}

                                                  // HASTIGHED
void speed(int speedL, int speedR, int mDir) {
  if (mDir == HIGH) {
    //Hvis baglæns
    speedL = invertOurValue( speedL );
    speedR = invertOurValue( speedR );
  } 
  digitalWrite( MOTOR_L_DIR, mDir ); 
  digitalWrite( MOTOR_R_DIR, mDir );   

  analogWrite( MOTOR_L_PWM, speedL );           
  analogWrite( MOTOR_R_PWM, speedR );           
}

                                          //======================== PING STATE
// function getPingState måler afstand og returnerer pingState
//int getPingState() {
void getPingState() {

  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println(" cm");
  delay(500); 
  //if (distance < 4) {  // This is where the LED On/Off happens
 
  /*
  //local variables:
  long duration;
  int cm = 0;

  // activate a ping
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // get distance in cm
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH); // tid til ekko registreres
  cm = duration / 29 / 2;

  // evaluate to state
  pingState = 0;
  if (cm <= 10 ){ pingState = 1; }
  if (cm > 10 && cm < 25){ pingState = 2; }
  
  return pingState;
*/  
} // end function getPingState  

                                                  //MAIN
void loop() {
  //runREW();
  //measureRMP();
  getPingState();
}
