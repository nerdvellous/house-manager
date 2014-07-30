#include <SoftwareSerial.h>

/*
 * Port série de liaison avec la carte principale
 */
SoftwareSerial comm(9, 8); // RX, TX

/*
 * Définitions des pins
 */
#define REDPin 5    // RED pin of the LED to PWM pin 4
#define GREENPin 3  // GREEN pin of the LED to PWM pin 5
#define BLUEPin 4   // BLUE pin of the LED to PWM pin 6
#define TEMPPin A3   // BLUE pin of the LED to PWM pin 6

/*
 * Get the tempetrature from sensor
 */
char get_temp() {
  int sensorValue = analogRead(TEMPPin);
  float voltage = sensorValue * 5;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100 ; 
  
  if (temperatureC < 0) {
    return temperatureC*-1+100;
  } else {
    return temperatureC;
  }
}

void red() {
  analogWrite(REDPin, 255);
  analogWrite(GREENPin, 0);
  analogWrite(BLUEPin, 0);
}

void green() {
  analogWrite(REDPin, 0);
  analogWrite(GREENPin, 255);
  analogWrite(BLUEPin, 0);
}

void blue() {
  analogWrite(REDPin, 0);
  analogWrite(GREENPin, 0);
  analogWrite(BLUEPin, 255);
}

void yellow() {
  analogWrite(REDPin, 255);
  analogWrite(GREENPin, 50);
  analogWrite(BLUEPin, 0);
}

void white() {
  analogWrite(REDPin, 255);
  analogWrite(GREENPin, 255);
  analogWrite(BLUEPin, 255);
}

void setup()
{
  comm.begin(9600);
  pinMode(REDPin, OUTPUT);
  pinMode(GREENPin, OUTPUT);
  pinMode(BLUEPin, OUTPUT);
  analogReference(DEFAULT);
}

void loop() // run over and over
{
  if (comm.available()) {
    char command = comm.read();
    if (command == 'r') {
      red();
    } else if (command == 'g') {
      green();
    } else if (command == 'b') {
      blue();
    } else if (command == 'y') {
      yellow();
    } else if (command == 'w') {
      white();
    } else if (command == 't') {
      comm.println(get_temp(), DEC); 
    }
  }
}

// ATMEL ATTINY84 / ARDUINO
//
//                           +-\/-+
//                     VCC  1|    |14  GND
//             (D  0)  PB0  2|    |13  AREF (D 10)
//             (D  1)  PB1  3|    |12  PA1  (D  9) 
//                     PB3  4|    |11  PA2  (D  8) 
//  PWM  INT0  (D  2)  PB2  5|    |10  PA3  (D  7) 
//  PWM        (D  3)  PA7  6|    |9   PA4  (D  6) 
//  PWM        (D  4)  PA6  7|    |8   PA5  (D  5)        PWM
//                           +----+
