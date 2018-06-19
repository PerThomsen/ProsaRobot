
// Pingben
#define trigPin1 14
#define echoPin1 15
#define trigPin2 16
#define echoPin2 17
#define trigPin3 18
#define echoPin3 19
#define pingTimeout 5000


int pingState = 0;

void setup() {
  // Init forbindelse til seriel monitor
  Serial.begin( 9600 );
  
  // Init ping 1-3
  pinMode(trigPin1, OUTPUT); // front
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT); // right
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT); // left
  pinMode(echoPin3, INPUT);  
}

// function getPingState måler afstand og returnerer pingState
int getPingState(int trigPin, int echoPin) {
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, pingTimeout);
  distance = (duration/2) / 29.1;
  //Serial.print(distance);
  //Serial.println(" cm");
  
  // evaluate to state      
  if (distance > 0 && distance <= 10 ){ 
    pingState = 2; 
  } else if (distance > 10 && distance < 25){ 
    pingState = 1; 
  } else {
    pingState = 0;
  }
  //Serial.println(pingState);
  return pingState;  
} // end function getPingState  

void loop() {

  if ((getPingState(trigPin1, echoPin1)) == 2) {
    // Stop og undersøg sider - evt bak
    //stopMotor();
    Serial.println("STOP"); 
    /*
    // look left
    if ((getPingState(trigPin2, echoPin2)) == 1) {
      if ((getPingState(trigPin3, echoPin3)) == 1) { 
        // bak
        Serial.println("REVERSE (slow)");
        speed(PWM_SLOW, PWM_SLOW + bias, M_REVERSE);
      }
    } else {
      speed(PWM_MID, PWM_MID + bias, M_FORWARD);
    }
    */
  } else if ((getPingState(trigPin1, echoPin1)) == 1) {
    // sænk farten
    Serial.println("FORWARD (mid)");
    //speed(PWM_MID, PWM_MID + bias, M_FORWARD);
  } else {
    Serial.println("FF");
  }
 }
