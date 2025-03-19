/*
Access the source code for this in https://github.com/Rover-cell/automatic-segregator-thingy/blob/main/
*/

#include <Servo.h>

Servo latch;

int position = 0;
const int S0 = 10;
const int S1 = 9;
const int S2 = 8;
const int S3 = 7;
const int sensorOut = 6;

#define IN1 A3
#define IN2 A2
#define IN3 A1
#define IN4 A0

const int minPaperR = 1300, maxPaperR = 29000;
const int minPaperG = 1600, maxPaperG = 29000;
const int minPaperB = 1700, maxPaperB = 29000;

const int minPlasticR = 600, maxPlasticR = 1700;
const int minPlasticG = 1100, maxPlasticG = 1900;
const int minPlasticB = 600, maxPlasticB = 1700;

bool paper = false;
bool plastic = false;
 
struct RGB {
  int red;
  int green;
  int blue;
};

void setup() {
  pinMode(sensorOut, INPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  latch.attach(3);
  latch.write(0);

  Serial.begin(9600);
}

void loop() {
  
  Serial.println("...");
  delay(1000);
  latch.write(0);
  Serial.println("Ready");
  differentiate();
}

void openLatch(){
  for(position = 0; position <= 180; position += 1){
    latch.write(position);
    delay(5);
  }
  delay(2500);
}

void closeLatch(){
  for(position = 180; position >= 0; position -= 1){
    latch.write(position);
    delay(5);
  }
}

RGB readColor() {
  RGB color;
  color.red = getColorValue(LOW, LOW);
  color.green = getColorValue(HIGH, HIGH);
  color.blue = getColorValue(LOW, HIGH);

  Serial.print("R: "); Serial.print(color.red);
  Serial.print(" G: "); Serial.print(color.green);
  Serial.print(" B: "); Serial.println(color.blue);
  
  return color;
}

int getColorValue(int s2State, int s3State) {
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  delay(100);  

  unsigned long total = 0;
  int readings = 5;
  
  for (int i = 0; i < readings; i++) {
    unsigned long frequency = pulseIn(sensorOut, LOW, 50000); 

    if (frequency == 0) {
      Serial.println("Warning: No pulse detected! Using fallback value.");
      total += 9000000;
    } else {
      total += (9000000 / frequency);
    }
    
    delay(1000);
  }
  
  return total / readings; 
}


void differentiate() {
  RGB sensorValue = readColor();

  paper = false;
  plastic = false;

  Serial.println("Differentiating materials...");
  delay(1000);

  if ((sensorValue.red >= minPaperR && sensorValue.red <= maxPaperR) &&
      (sensorValue.green >= minPaperG && sensorValue.green <= maxPaperG) &&
      (sensorValue.blue >= minPaperB && sensorValue.blue <= maxPaperB)) {
    paper = true;
    Serial.println("Paper detected");
    goToPaperBin();
  }
  
  else if ((sensorValue.red >= minPlasticR && sensorValue.red <= maxPlasticR) &&
           (sensorValue.green >= minPlasticG && sensorValue.green <= maxPlasticG) &&
           (sensorValue.blue >= minPlasticB && sensorValue.blue <= maxPlasticB)) {
    plastic = true;
    Serial.println("Plastic detected");
    openLatch();
  }
  else {
    Serial.println("No objects detected");
  }
}

void stopMotors(){
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward(){
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void moveBack(){
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void goToPaperBin(){
  moveForward();
  delay(800);
  stopMotors();
  openLatch();
  closeLatch();
  delay(1000);
  moveBack();
  delay(800);
  stopMotors();
}
