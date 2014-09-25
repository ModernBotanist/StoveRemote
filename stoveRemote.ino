#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"


SMSGSM sms; 


//To change pins for Software Serial, use the two lines in GSM.cpp.
//GSM TX pin to 2, RX to 3
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//cell stuff
boolean started=false;
char toSend[160]; //holds the char array of toSendStr
char smsbuffer[160]; //hold recieved messages
char number[20]; //who texted
byte Position; //address for sim card
int password = 12345;//the passcode to text
String partSend = "Confirming start. Reply 'STOP' to turn off stove. Current temp is ";
String toSendStr;
char myNumb[20] = "";//your phone number, for debug


//Temperature stuff
float voltage; 
float tempC;
float tempF;

//stovestuff
byte stoveState = 0; //start with stove off
unsigned long timer;

//some pins to for stuff
int tempPin = A7;
byte pwr = 6;
byte SIMreset = 7;
int stovePin = A0;


#define DEBUG true //change to false to turn off debug


void setup() 
{
  pinMode(tempPin, INPUT);
  pinMode(pwr, OUTPUT);
  pinMode(SIMreset, OUTPUT);
  pinMode(stovePin, OUTPUT);
  
  //turn on GSM when power is on
  digitalWrite(pwr, HIGH);
  delay(2500);
  digitalWrite(pwr, LOW);
  
  
  //Serial connection.
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("GSM Shield testing.");
  }
  //For http uses is recommended to use 4800 or slower.
  if (gsm.begin(2400)){ //talk to the SIM900
    if (DEBUG) Serial.println("\nstatus=READY");
    started=true;  
  }
  else {
    if (DEBUG) Serial.println("\nstatus=IDLE");
    digitalWrite(SIMreset, HIGH);
    delay(2000);
    digitalWrite(SIMreset, LOW);
  }

}

void loop() 
{
    //Read the temp once per loop from TMP36
    readTemp();
    toSendStr = partSend + String(tempF) + "F"; //concat. our strings
    toSendStr.toCharArray(toSend,160); //buff length needs to be only
    
    if (DEBUG) {
      Serial.println(tempF); 
      Serial.println(toSend);
      Serial.println(toSendStr);
    }
    
    /*look for texts*/
        Position = sms.IsSMSPresent(SMS_UNREAD); //check location of unread sms
	
        if (Position !=0) { // read new SMS
	  sms.GetSMS(Position, number, smsbuffer,160);
          if (DEBUG) {
            Serial.println(number); //who's it from
            Serial.println(smsbuffer); //what'd they say
            Serial.print("int translated:");
            Serial.println(atoi(smsbuffer));
          }
          sms.DeleteSMS(Position); //get rid of the text
         
    //do with them what needs to be done      
        if(stoveState == 0) { //stove currently off, look for passcode
          if (atoi(smsbuffer) == password) { //check code
            sms.SendSMS(number, toSend);
            Stove(); //and turn on/off the stove
            timer = millis(); //mark time to reset stove state
        } else {
            sms.SendSMS(number, "wrong passphrase");
            }
        }
        
        if(String(smsbuffer) == "STOP") { //someone is telling the stove 'turn off'
           if(stoveState == 1) { //it is on
           Stove(); //turn it off (assuming the button presses are the same)
           sms.SendSMS(number, "Stove turned Off");//for debug
        } 
    }}
    
    
    delay(1000);
  
  //deal with some variable resets
  if (timer > millis()) //if the timer wraps around, restart.
    timer = millis();
    
  /*This piece decays the stoveState back to 0. The
    only other way it goes to zero is to text stop. Since
    this usually won't happen, you would not be able to
    restart the stove on the following trip without first disconnecting
    power, or first texting stop, then the code.
    43200000 msec is 12 hours*/
    if (stoveState == 1) { //do we think it's on?
      if ((timer + 43200000) < millis()) { // has it been 12 hours since code sent?
        if (DEBUG) sms.SendSMS(myNumb, "it's been 12 hours");//for debug
        stoveState = 0;
  }
  }
  
}

void Stove() {
  if (stoveState == 0) { //if off
      stoveState = 1; //stove now on
  } else {
    stoveState = 0; }
      
      digitalWrite(stovePin, HIGH); //assuming it takes the same pattern for off/on
      delay(2000);
      digitalWrite(stovePin, LOW);
}

void readTemp() {
    voltage = analogRead(tempPin) * 3.3; //main power is 3.3V
    voltage /= 1024.0; //voltage now stores mV
    tempC = (voltage - 0.5) * 100;
    tempF = (tempC * 9.0 / 5.0)+29; //that 29 should be 32, but the temp was reading high
}
