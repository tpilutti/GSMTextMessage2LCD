/*
 GSM Text 2 LCD is a sketch that will allow a text message (SMS) to be sent to the Arduino 
 with a GSM shield and activated SIM card.  The sketch accepts messages from only one cell
 number.  The text message is saved, and then sent to an LCD.  Scrolling is done as needed.
 
 Circuit:
 * Arduino
 * GSM shield attached to an Arduino
 * SIM card that can receive SMS messages
 * 16x2 LCD and appropriate wiring
 
 Based on ReceiveSMS.ino:
 created 25 Feb 2012
 by Javier Zorzano / TD
 ReceiveSMS.ino example is in the public domain.
 http://arduino.cc/en/Tutorial/GSMExamplesReceiveSMS
 
 GSMTextMessage2LCD implementation written by T. Pilutti, December 8, 2013
 
 to use:
 0. Put your cell number in variable myCellNumber in code below
 1. Power up and wait for the "Waiting for SMS" message on the LCD
 2. Send SMS to the shield.
 3. Message will show up on LCD
 
*/

// include the GSM library
#include <GSM.h>
// include the LCD library
#include <LiquidCrystal.h>

// PIN Number for the SIM
#define PINNUMBER ""  // "" means no PIN is used

/* initialize the library with the numbers of interface pins:
LCD RS pin to digital pin 12
LCD Enable pin to digital pin 11
LCD data pin D4 to digital pin 5
LCD data pin D5 to digital pin 4
LCD data pin D6 to digital pin 6  // changed for gsm shield
LCD data pin D7 to digital pin 8  // changed for gsm shield
*/
LiquidCrystal lcd(12, 11, 5, 4, 6, 8);  // free pins when GSM shield used

// initialize the library instances
GSM gsmAccess;
GSMScanner scannerNetworks;
GSM_SMS sms;

char senderNumber[20];  // char array to hold the number a SMS is retreived from
char txtMsg[200];  // the text message char array
int numChars = 0; // number of characters in txtMsg
int validMessage = 0;  // set to 1 when there is a message to display
// put your cell number in myCellNumber
char myCellNumber[] = "+XXXXXXXXXXXX";  // used for message screening

void setup() 
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  
  // connection state
  boolean notConnected = true;
  
  // provide information to LCD as GSM gets started
  lcd.print("GSM starting up ...");
 
  // Start GSM connection
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
      lcd.setCursor(0,1); // format is col,row
      lcd.print("Initializing...");
      // delay(1000);
    }
  }
  
  // currently connected carrier
  Serial.print("Current carrier: ");
  Serial.println(scannerNetworks.getCurrentCarrier());
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Current carrier:");
  lcd.setCursor(0,1);
  lcd.print(scannerNetworks.getCurrentCarrier());
  delay(1000);
  lcd.clear();
  
  // returns strength and ber
  // signal strength in 0-31 scale. 31 means power > 51dBm
  // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
  Serial.print("Signal Strength: ");
  Serial.print(scannerNetworks.getSignalStrength());
  lcd.setCursor(0,0);
  lcd.print("Signal Strength:");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print(scannerNetworks.getSignalStrength());
  delay(2000);
  lcd.clear();
  
  // print one time message to the LCD
  lcd.setCursor(0,0); // format is col,row
  lcd.print("Where's TP?");  // first row
  
  lcd.setCursor(0,1); // format is col,row
  lcd.print("Waiting for SMS");
  Serial.println("Waiting for SMS");  // Diagnostic


}

void loop() 
{
  char c;
  int i; // char array index of txtMsg
  int validMessage; // set to 1 when there is a message to display
  
  // If there are any SMSs available()  
  if (sms.available())
  {  
    
    // show on display that a text message has been received
    lcd.setCursor(0,1); // format is col,row
    lcd.print("SMS received");
    delay(2000);
    
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println("Message received from:");  // Diagnostic
    Serial.println(senderNumber);  // Diagnostic

    // Any messages not coming from my cell number should be discarded. 
    if( strncmp( senderNumber, myCellNumber, 12 ) != 0 )  // strncmp returns 0 if match of 12 chars
    {
      Serial.println("Discarded SMS");  // Diagnostic 
      sms.flush();
    }
    else {
      // Read message bytes and save them
      i = 0;
      while( c = sms.read() ) {
        Serial.print(c);  // Diagnostic
        // save the message character by character and keep track how many chars in message
        txtMsg[i] = c;  
        i++;
        validMessage = 1;  // there is a valid message
      }
      numChars = i;  // the number of characters in txtMsg
    }
    
    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
    
   }  // end of if sms.available section

   if ( validMessage ) {

     // now print the message to the LCD.
     // if there is only one row, print it all at once, else
     // do scrolling.
     if ( numChars <= 16 ) {
       for (i = 0; i < numChars; i++) {
         lcd.setCursor(i, 1);  // format is col,row
         lcd.print(txtMsg[i]);
       }
     }
     else {
       // logic for how far to scroll
       for ( int q = 0; q <= numChars - 16; q++) {
         // print out just one char at a time, and repeat for each column
         for (i = 0; i < 16; i++) {
           lcd.setCursor(i, 1);  // i prints to a column
           lcd.print(txtMsg[i+q]);
           if (q == 0 && i == 15) {
             delay(1800); // time to read the starting 16 chars
           }
         }
          delay(650);  // time for the scroll delay
       }
     }
 
     delay(1000);  // time to hold the last line fragment
     
     // blank out the display after the scrolling has completed to prepare repeat
     for (i = 0; i < 16; i++) {
       lcd.setCursor(i, 1);  // format is col,row
       lcd.print(' ');  // blank out the second row
     }
   
     delay(2000);  // time the display is blank
   
   }  // end of if validMessage section
    
}
