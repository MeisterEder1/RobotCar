#include <Thread.h>
Thread distThread = Thread();
Thread spdThread = Thread();

//motor A connected between A01 and A02
//motor B connected between B01 and B02
int STBY = 10; //standby

//Motor A
int PWMA = 6; //Speed control 
int AIN1 = 9; //Direction
int AIN2 = 8; //Direction

//Motor B
int PWMB = 5; //Speed control
int BIN1 = 11; //Direction
int BIN2 = 12; //Direction

byte motor_trim = 3;

// Ultraschall
int trigPin = 7;    // Trigger
int echoPin = 2;    // Echo
long duration, cm;

const int limit = 30;
const byte spd = 130;
const int rot_length = 40;
  
// speed sensor
volatile byte spd_cnt;

bool debug = false;

void setup() {
  if(debug) {
    Serial.begin (9600);
  }
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  attachInterrupt(1, SpdCnt, RISING);

  distThread.onRun(checkDistance);
  distThread.setInterval(500);

  spdThread.onRun(checkSpeed);
  spdThread.setInterval(500);
}

void SpdCnt()
{
  spd_cnt++;
}

void check()
{
// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = duration * 0.017;

  if(debug)
  {
    Serial.print("dist: ");
    Serial.print(cm);
    Serial.println();
  }
}

void checkDistance()
{ 
  check();  
 
   if(cm < 20)
   {
     stop();
     delay(100); 

     // rückwärts
     move(1, 100, 0); 
     move(2, 100, 0);
     delay(500); 

     // drehen
     move(1, 180, 1); 
     move(2, 180, 0); 
     delay(rot_length);
     check();
     
     while(cm < limit)
     {
       move(1, 180, 1); 
       move(2, 180, 0); 
       delay(rot_length);
       check();
     }
  }
}

void checkSpeed() {
  spd_cnt = 0;
  delay(500);
  if(spd_cnt == 0)
  {  
     if(debug)
     {
       Serial.print("cnt: ");
       Serial.print(spd_cnt);
       Serial.println();
     }
     // rückwärts
     move(1, 100, 0); 
     move(2, 100, 0);
     delay(2000); 

     // drehen
     move(1, 180, 0); 
     move(2, 180, 1); 
     delay(2000);
     check();

     while(cm < limit)
     {
       move(1, 180, 1); 
       move(2, 180, 0); 
       delay(rot_length);
       check();
     }   
   }
}

void moveRobot() {
  // vorwärts
  move(1, spd + motor_trim, 1); //motor 1, full speed, left
  move(2, spd - motor_trim, 1); //motor 2, full speed, left
}

void loop(){
  if(distThread.shouldRun())
    distThread.run();
  
  if(spdThread.shouldRun())
    spdThread.run();
    
  moveRobot();  
  //checkDistance();
  //checkSpeed();
}

void move(int motor, int speed, int direction){
//Move specific motor at speed and direction
//motor: 0 for B 1 for A
//speed: 0 is off, and 255 is full speed
//direction: 0 clockwise, 1 counter-clockwise

  digitalWrite(STBY, HIGH); //disable standby

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  if(motor == 1){
    digitalWrite(AIN1, inPin1);
    digitalWrite(AIN2, inPin2);
    analogWrite(PWMA, speed);
  }else{
    digitalWrite(BIN1, inPin1);
    digitalWrite(BIN2, inPin2);
    analogWrite(PWMB, speed);
  }
}

void stop(){
//enable standby  
  digitalWrite(STBY, LOW); 
}
