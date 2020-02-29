#include <ESP8266WiFi.h>
#include <Stepper.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// prototypes
boolean connectWifi();

//on/off callbacks 
bool deckingCurtainOn();
bool deckingCurtainOff();

// Change this before you flash
const char* ssid = "WeeabooNet";
const char* password = "FuckingKangaroos";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *office = NULL;
//Switch *kitchen = NULL;

bool isdeckingCurtainOn = false;
//bool isKitchenLightstsOn = false;



// Motor -------------------------

const int stepsPerRevolution = 200;
boolean outstandingWork = false;

Stepper myStepper(stepsPerRevolution,5,4,0,2); 


unsigned long TimerTravel;    //"ALWAYS use unsigned long for timers"

//End of motor--------------------


 
void setup()
{
  Serial.begin(9600);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  myStepper.setSpeed(60);
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    office = new Switch("Bedroom Curtains", 80, deckingCurtainOn, deckingCurtainOff);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*office);
  //  upnpBroadcastResponder.addDevice(*kitchen);
  }



// Motor -------------------


/*
  pinMode(SLEEP,OUTPUT);
  pinMode(STEP,OUTPUT);
  pinMode(DIR,OUTPUT);
  */
//end motor ---------------


}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      office->serverLoop();
	 }


       if (((millis()-TimerTravel) <= 7100UL) && (outstandingWork == true))  // Set the 'UL' figure to the tine it takes to open or close your curtain
      {
      Serial.println(F("Moving Curtain")); 
      moveCurtain();
      }
      else
      {
        outstandingWork = false;
        //Serial.println(F("Finished doing my work!"));
        //Serial.println(F("SLEEP HIGH"));
        Serial.println("SLEEP");
      }


      
}

bool deckingCurtainOn() {
    Serial.println("Switch 1 turn on ...");


          Serial.print("Curtain turned on.... Closing the curtain...");
          closeCurtain();

          
    isdeckingCurtainOn = true;
    return isdeckingCurtainOn;
}

bool deckingCurtainOff() {
    Serial.println("Switch 1 turn off ...");

          Serial.print("Curtain turned off...  Opening the curtain...");
          openCurtain();

    isdeckingCurtainOn = false;
    return isdeckingCurtainOn;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}


void openCurtain() {
  Serial.print("Still Opening the curtain...");
 
        Serial.println("Opening");
        myStepper.step(stepsPerRevolution)
        TimerTravel= millis();
        Serial.println(F("SLEEP LOW"));
        delay(500);
        outstandingWork = true;

  
  //delay(1000);                  // waits for a second

}


void closeCurtain() {
    Serial.print("Still closing the curtain...");
  
        Serial.println(F("dir LOW"));
                Serial.println("Closing");
        myStepper.step(-stepsPerRevolution)
        TimerTravel= millis();
        Serial.println(F("SLEEP LOW"));
        delay(500);
        outstandingWork = true;
  
  //delay(1000);                  // waits for a second
}

// for the motor -------------
/*
void stepNow(int totalSteps) {
  Serial.print(totalSteps);
  Serial.println(F(" steps."));

  int i;
  for(i=0;i<totalSteps;++i) {
    digitalWrite(STEP,HIGH);
    delayMicroseconds(1100);
    digitalWrite(STEP,LOW);
    delayMicroseconds(1100);
  }
}
*/
//----------------------------


void moveCurtain() {
  delayMicroseconds(500);
  myStepper.step(stepsPerRevolution)
}
