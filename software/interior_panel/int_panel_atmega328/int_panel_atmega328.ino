/*
 * Includes
 */
#include "Picaso_Serial_4DLib.h"
#include "Picaso_const4D.h"
#include <SimpleTimer.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "Wire.h"

#define DS1307_I2C_ADDRESS 0x68  // This is the I2C address

/*
 * PIN definition
 */
#define DOORPin 7
#define BTNPin 9


/*
 * Port de liaison série avec l'écran
 */
#define DisplaySerial Serial

/*
 * Objet Display
 */
Picaso_Serial_4DLib Display(&DisplaySerial);

/*
 * Port de liaison série avec le lecteur RFID
 */
SoftwareSerial RFIDSerial(12, 8); // RX, TX

/*
 * Port de liaison série avec le lecteur RFID
 */
SoftwareSerial EXTSerial(10, 11); // RX, TX

/*
 * handle for display model
 */
char mdl[20];

/*
 * Screen firmware versions
 */
char spe2[4];
char pmmc[4];

/*
 * Valeurs RTC
 */
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

/*
 * Timer
 */
SimpleTimer timer;
int current_timer_id = 0;

/*
 * Compteur pour l'envoi des donnees à l'écran
 */
long refresh_counter = 0;

/*
 * Button
 */
boolean btn = false;
boolean previous_btn = false;


/********
 * RFID *
 ********/
 
byte cardID[6];
byte checksum = 0;
boolean newCard = false;

/*
 * Read a card id from the serail port
 */
void getCardID() {
  byte count = 0;
  byte rx = 0;
  byte tempByte = 0;
  
  while(count < 12) {
    if(RFIDSerial.available() > 0) {
      rx = RFIDSerial.read();
      if ((rx == 0x02) || (rx == 0x0A) || (rx == 0x0D) || (rx == 0x03)) {
        break;
      }
      
      // Transformation des caractères ASCII en hexa
      if ((rx >= '0') && (rx <= '9')) {
        rx = rx - '0';
      }
      else if ((rx >= 'A') && (rx <= 'F')) {
        rx = 10 + rx - 'A';
      }
      
      if (count & 1 == 1) {
        cardID[count >> 1] = (rx | (tempByte << 4));
        
        // Calcul du checksum en continu
        if (count >> 1 < 5) {
          checksum ^= cardID[count >> 1];
        }
      } else {
        tempByte = rx;
      }
      count++;
    }
  }
}

/*
 * Print the current card id
 */
void printCardID() {
  /*int i = 0;
  for (i = 0 ; i < 6 ; i++) {
    LOG.print(cardID[i], HEX);
  }
  LOG.print("\n");*/
}

/*
 * Launch or restart the standing by timer
 */
void launch_timer() {
  if (current_timer_id != 0) {
    timer.restartTimer(current_timer_id);
  } else {
    current_timer_id = timer.setTimeout(2000, standby);
  }
}
 
/*
 * Display the error status on the led
 */
void error() {
  EXTSerial.print('r');
  launch_timer();
}

/*
 * Display the success status on the led
 */
void success(boolean auto_clear = true) {
  digitalWrite(DOORPin, HIGH);
  EXTSerial.print('g');
  EXTSerial.print('z');
  if (auto_clear) {
    launch_timer();
  }
}

/*
 * Display the standing by status on the led
 */
void standby() {
  digitalWrite(DOORPin, LOW);
  current_timer_id = 0;
  EXTSerial.print('b');
}

/*
 * Display the waiting status on the led
 */
void waiting() {
  EXTSerial.print('y');
}

/*
 * Check if the card id is stored
 */
boolean checkCardID() {
  byte cardsCount = EEPROM.read(0);
  if (cardsCount == 255) { cardsCount = 0; }
  boolean check = false;
  byte i;
  for (i = 0 ; i < cardsCount ; i++) {
    boolean checkCard = true;
    int j;
    for (j = 0 ; j < 6 ; j++) {
      if(EEPROM.read(8*i+1+j) != cardID[j]) {
        checkCard = false;
        break;
      }
    }
    if (checkCard) {
      check = true;
      break;
    }
  }
  return check;
}

/*
 * Save the card id
 */
void saveCard() {
  if (!checkCardID()) {
    byte cardsCount = EEPROM.read(0);
    if (cardsCount == 255) { cardsCount = 0; }
    byte j;
    for (j = 0 ; j < 6 ; j++) {
      EEPROM.write(8*cardsCount+1+j, cardID[j]);
    }
    EEPROM.write(0, cardsCount+1);
  }
}

/*
 * Samples the door button
 */
void btn_sampling() {
  if (digitalRead(BTNPin) == LOW) {
    btn = true;  
  } else {
    btn = false;
  }
}

/**********
 * SCREEN *
 **********/
 
 /* 
 * SD card mouting
 */
int trymount(void)
{
  #define retries 15
  int i ;
  int j ;
  i = Display.file_Mount() ;
  j = 0 ;
  if (!i)
  {
    Display.putstr("Pleass insert uSD card\n") ;
    while (   (!i)
           && (j < retries) )
    {
      Display.putstr(".") ;
      i = Display.file_Mount() ;
      j++ ;
    }
    Display.putstr("\n") ;
  }
  if (j == retries)
    return false ;
  else
    return true ;
}

/*
 * Formate un entier pour obtenir une chaine
 * de caractères de version affichable
 */
void get_infos(char* str, word value) {
  str[3] = 0;
  str[1] = '.';
  str[0] = ((value & 0xF00) >> 8)+48;
  str[2] = (value & 0xF)+48;
}

/*
 * Loads a program from the sd card into the screen RAM
 */
void load_file(char* file) {
  word handle;
  Display.gfx_Cls();
  handle = Display.file_LoadFunction(file) ;
  Display.file_CallFunction(handle, 0, 0);
}

/*
 * Send a value to the screen
 */
void send_value(char command, char value) {
  DisplaySerial.write(command);
  DisplaySerial.write(value); 
}

/*
 * Send the temperature to the screen
 */
void send_temp() {
  char temp = get_temp();
  send_value('t', temp);
}

/*
 * Send the date to the screen
 */
void send_time() {
  send_value('h', hour); 
  send_value('m', minute); 
}

/*
 * Send the date to the screen
 */
void send_date() {
  send_value('Y', year); 
  send_value('M', month); 
  send_value('D', dayOfMonth);
  send_value('W', dayOfWeek);
}

/*
 * Get the tempetrature from sensor
 */
byte get_temp() {
  EXTSerial.print('t');
  delay(100);
  byte rx = 0;
  while (EXTSerial.available()) {
      rx = (byte)EXTSerial.read();
  }
  return rx;
}

/* 
 * Convert binary coded decimal to normal decimal numbers
 */
byte bcdToDec(byte val)
{
  return ( ((val >> 4)*10) + (val%16) );
}

/*
 * Gets the date and time from the ds1307 and prints result
 */
void getDateDs1307()
{
  // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0x0);
  Wire.endTransmission();
 
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
 
  // A few of these need masks because certain bits are control bits
  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  dayOfWeek  = (bcdToDec(Wire.read())-1);
  if (dayOfWeek == 0) { dayOfWeek = 7; }
  dayOfMonth = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());
}

/*
 * Refresh screen data
 */
void refresh_data() {
  if (++refresh_counter == 60) {
    refresh_counter = 0;
    getDateDs1307();
    send_time(); 
    send_temp();
    if (hour == 0 && minute == 0) {
      send_date(); 
    }
  }
}

/*********
 * SETUP *
 *********/
void setup()
{
  // Initialize pins
  pinMode(DOORPin, OUTPUT);
  pinMode(BTNPin, INPUT_PULLUP);
  analogReference(DEFAULT);
  
  // Initialize RFID
  EXTSerial.begin(9600);
  
  // Initialize RFID
  RFIDSerial.begin(9600);
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize screen
  delay(1000);
  DisplaySerial.begin(9600);
  Display.TimeLimit4D = 2000;
  Display.gfx_Cls();
  Display.txt_Set(TEXT_COLOUR,WHITE);
  
  // Show infos
  Display.putstr("Alarm System v1.0\n");
  Display.putstr("(c) 2014\n\n");
  Display.putstr("Setting up ...\n\n");
  get_infos(spe2,Display.sys_GetVersion());
  get_infos(pmmc,Display.sys_GetPmmC());
  Display.txt_Set(TEXT_COLOUR,BLUE);
  Display.putstr("Model: ");
  Display.putstr(mdl);
  Display.putstr("\nSPE2 Version: ");
  Display.putstr(spe2);
  Display.putstr("\nPmmC Version: ");
  Display.putstr(pmmc);
  Display.putstr("\n\n");
  delay(1500); 
  
  if (!trymount()) {
    Display.txt_Set(TEXT_COLOUR,RED);
    Display.putstr("Mounting failed !\n");
    return; 
  }
  
  // Activate touch
  Display.txt_Set(TEXT_COLOUR,WHITE);
  Display.putstr("Enabling touch ...");
  Display.touch_Set(TOUCH_ENABLE);
  Display.touch_DetectRegion(0,0, 239, 319);
  Display.putstr(" done");
  Display.putstr("\n\nok.");
  delay(1500);
  
  // Load main program
  load_file("HOME.4XE");
  getDateDs1307();
  send_temp();
  send_time();
  send_date();
  
  timer.setInterval(50, btn_sampling);
  timer.setInterval(1000, refresh_data);
  
  standby();
}

/********
 * LOOP *
 ********/
void loop()
{
  timer.run();
  
  // BTN
  if (previous_btn == false && btn == true) {
    success(false);
  } else if (previous_btn == true && btn == false) {
    standby();
  }
  previous_btn = btn;
  
  byte rx = 0;
  if(RFIDSerial.available() > 0) {
    rx = RFIDSerial.read();
    if (rx == 2) {
      getCardID();
      //printCardID();
      
      if (checksum == cardID[5]) {
        checksum = 0;
        if (!newCard) {
          if (checkCardID()) {
            success();
          } else {
            error();
          }
        } else {
          saveCard();
          newCard = false;
          send_value('k', 0);
          standby();
        }
      } else {
        error();
      }
    }
  } else if(DisplaySerial.available() > 0) {
    rx = DisplaySerial.read();
    if (rx == 'n') {
      waiting();
      newCard = true;
    } else if (rx == 'c') {
      EEPROM.write(0, 0);
      send_value('k', 0);
    } else if (rx == 'd') {
      byte cardsCount = EEPROM.read(0);
      if (cardsCount == 255) { cardsCount = 0; }
      //DisplaySerial.print("Cards registered : ");
      //DisplaySerial.println(cardsCount, HEX);
      byte i;
      for (i = 0 ; i < cardsCount ; i++) {
        boolean checkCard = true;
        int j;
        for (j = 0 ; j < 6 ; j++) {
          //DisplaySerial.print(EEPROM.read(8*i+1+j), HEX);
        }
        //DisplaySerial.print("\n");
      }
    }
  }
}
