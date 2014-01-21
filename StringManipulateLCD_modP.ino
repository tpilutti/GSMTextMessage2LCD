/*
string sender to test LCD scrolling.  idea is to treat the console (keyboard)
as the SMS message extracted from the GSM.  Was a lot easier to do development 
this way without having to wait on the modem to restart.

scrolling to work on second row, and only if message is longer than 16 chars.

Written by T. Pilutti
Nov 29, 2013

readSerial() function taken without modification from SendSMS.ino
created 25 Feb 2012 by Tom Igoe
*/

// pull in the needed library
#include <LiquidCrystal.h>

/* initialize the library with the numbers of interface pins:
LCD RS pin to digital pin 12
LCD Enable pin to digital pin 11
LCD data pin D4 to digital pin 5
LCD data pin D5 to digital pin 4
LCD data pin D6 to digital pin 6  // changed for gsm when using shield
LCD data pin D7 to digital pin 8  // changed for gsm when using shield
*/

LiquidCrystal lcd(12, 11, 5, 4, 6, 8);
 
char txtMsg[200];
int numChars = 0; // number of characters in txtMsg


void setup()
{
  // initialize serial communications
  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  // print one time message to the LCD
  lcd.setCursor(0,0); 
  lcd.print("TP Office Update");

  Serial.println("String Sender");
  
  // mimics sms text
  Serial.print("enter string: ");
  
  readSerial(txtMsg);
  Serial.println("string received:");
  Serial.println(txtMsg);
  
  while(txtMsg[numChars] != '\0') {
    numChars++;
  }
  Serial.println(numChars);
  
}

void loop()
{ 
  int i = 0;
   
  // set cursor to row 2 first column (row 1 = 0)
  // lcd.setCursor(0, 1);  // format is col,row
  //
  
  if ( numChars <= 16 ) {
    // message does not need scrolling
    for (i = 0; i < numChars; i++) {
      lcd.setCursor(i, 1);  // format is col,row
      lcd.print(txtMsg[i]);
    }
  }
  else {
    // logic for how far to scroll.  scroll length of message minus the 
    // part that displays at first.  q controls number of scroll events.
    for ( int q = 0; q <= numChars - 16; q++) {
      // print out just one char at a time, and repeat for each column
      for (i = 0; i < 16; i++) {
        // send the LCD a set of 16 chars based on position in txtMsg
        // determined by indexing based on q start position.
        lcd.setCursor(i, 1);  // i prints to a column
        lcd.print(txtMsg[i+q]);
        if (q == 0 && i == 15) {
          // detects the first 16 chars that are displayed before
          // scrolling begins
          delay(1800); // pause time to read the starting 16 chars
        }
      }
      delay(500);  // time for the scroll delay
    }
  }
 
  delay(1500);  // time to hold the last line fragment
  
  // blank out the display after the scrolling has completed to prepare repeat
  for (i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);  // format is col,row
    lcd.print(' ');  // blank out the second row
  }
  delay(2000);  // time the display is blank
}

/*
  Read input serial from keyboard
 */
int readSerial(char result[])
{
  int i = 0;
  while(1)
  {
    while (Serial.available() > 0)
    {
      char inChar = Serial.read();
      if (inChar == '\n')
      {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if(inChar!='\r')
      {
        result[i] = inChar;
        i++;
      }
    }
  }
}
