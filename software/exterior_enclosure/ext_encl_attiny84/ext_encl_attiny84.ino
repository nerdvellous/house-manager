/**
 * Project : house-manager
 * File : ext_encl_attiny84.ino
 * This is the program run by the attiny84 in the exterior enclosure.
 * It receives commands from the control panel and send the exterior temperature
 * if needed.
 *
 * Copyright (C) 2014 nerdvellous
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <SoftwareSerial.h>

/*
 * Serial communication with the main board
 */
SoftwareSerial comm(9, 8); // RX, TX

/*
 * pin mapping
 */
#define REDPin 5    // red pin (PWM)
#define GREENPin 3  // green pin (PWM)
#define BLUEPin 4   // blue pin (PWM)
#define BUZZPin 6  // temperature sensor
#define TEMPPin A3  // temperature sensor

/*
 * color mapping
 */
unsigned char color_mapping[6][3] = {
  {0,0,0}, // 0 : none
  {255,0,0}, // 1 : red
  {0,255,0}, // 2 : green
  {0,0,255}, // 3 : blue
  {255,50,0}, // 4 : yellow
  {255,255,150}, // 5 : white
};

/*
 * Get the tempetrature from the sensor (in celsius).
 * If the temperature is negative, the function returns
 * the additive inverse of the value plus 100.
 * For instance -7 will become 107.
 */
char get_temp() {
  int sensorValue = analogRead(TEMPPin);
  float voltage = sensorValue * 5;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100; 
  
  if (temperatureC < 0) {
    return temperatureC*-1+100;
  } else {
    return temperatureC;
  }
}

/*
 * Manage the led
 */
void led(unsigned char rgb[]) {
  analogWrite(REDPin, rgb[0]);
  analogWrite(GREENPin, rgb[1]);
  analogWrite(BLUEPin, rgb[2]);
}

/*
 * Buzz
 */
void buzz() {
  digitalWrite(BUZZPin, HIGH);
  delay(400);
  digitalWrite(BUZZPin, LOW); 
}

// SETUP
void setup() {
  comm.begin(9600);
  pinMode(REDPin, OUTPUT);
  pinMode(GREENPin, OUTPUT);
  pinMode(BLUEPin, OUTPUT);
  pinMode(BUZZPin, OUTPUT);
  analogReference(DEFAULT);
}

// LOOP
/* Command list :
 * r : led to red
 * g : led to green
 * b : led to blue
 * y : led to yellow
 * w : led to white
 * n : led faded
 */
void loop() {
  if (comm.available()) {
    char command = comm.read();
    if (command == 'r') {
      led(color_mapping[1]);
    } else if (command == 'g') {
      led(color_mapping[2]);
    } else if (command == 'b') {
      led(color_mapping[3]);
    } else if (command == 'y') {
      led(color_mapping[4]);
    } else if (command == 'w') {
      led(color_mapping[5]);
    } else if (command == 'n') {
      led(color_mapping[0]);
    } else if (command == 'z') {
      buzz();
    } else if (command == 't') {
      comm.write(get_temp()); 
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
