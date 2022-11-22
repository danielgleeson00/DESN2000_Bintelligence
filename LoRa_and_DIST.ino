/**
 * @file low_power_lorawan_example.cpp
 * @author Kalina Knight
 * @brief A simple example of the LoRaWAN functions.
 * @version 0.1
 * @date 2021-08-27
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // 
#include "LoRaWAN_functs.h" /**< Go here to provide the OTAA keys & change the LoRaWAN settings. */
#include "OTAA_keys.h"      /**< Go here to set the OTAA keys (See README). */
#include <Wire.h>
#include <Adafruit_GFX.h>   //Click here to get the library: http://librarymanager/All#Adafruit_GFX
#include <Adafruit_SSD1306.h>   //Click here to get the library: http://librarymanager/All#Adafruit_SSD1306

#ifdef RAK4630
  #define BOARD "RAK4631 "
  #define  RAK4631_BOARD true  
#else    
  #define  RAK4631_BOARD false             
#endif
 
#define TRIG WB_IO6
#define ECHO WB_IO4
#define PD   WB_IO5   //power done control （=1 power done，=0 power on） 

#define TIME_OUT  24125 //max measure distance is 4m,the velocity of sound is 331.6m/s in 0℃,TIME_OUT=4*2/331.6*1000000=24215us

float ratio = 346.6/1000/2;   //velocity of sound =331.6+0.6*25℃(m/s),(Indoor temperature about 25℃)

//measure high level time
long int duration_time();

// App payload interval value in [ms] = 30 seconds.
const int lorawan_app_interval = 1000;

// Buffer that payload data is placed in.
uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE] = "";

// Struct that passes the payload data for a LoRaWAN frame.
lmh_app_data_t lorawan_payload = {
    payload_buffer, strlen(""), 203, 0, 0,
};

/**
 * @brief Setup code runs once on reset/startup.
 */
void setup() {
    
    // Initialize built in green LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Initialize Serial for debug output
    Serial.begin(115200);

    // Wait for USB Serial to be ready or terminal to be connected
    time_t timeout = millis(); // Timeout in case the system runs on its own
    // Waiting for Serial
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            // Blink the LED to show that we are alive
            delay(100);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        else
        {
            // Timeout while waiting for USB Serial
            break;
        }
    }
        Serial.println("\n=================================");
        Serial.println("Welcome to Simple LoRaWAN Example");
        Serial.println("\n=================================");

    // Init LoRaWAN
    if (!initLoRaWAN(OTAA_KEY_APP_EUI, OTAA_KEY_DEV_EUI, OTAA_KEY_APP_KEY)) {
        delay(1000);
        return;
    }

    // Attempt to join the network
    startLoRaWANJoinProcedure();
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    // every lorawan_app_interval milliseconds check if the device is connected
    delay(lorawan_app_interval);

    long int duration, mm;
    uint8_t send[PAYLOAD_BUFFER_SIZE];
    digitalWrite(LED_BLUE,HIGH);
    duration = duration_time();
    if(duration > 0)
    {
      mm = duration*ratio; //Test distance = (high level time×velocity of sound (340M/S) / 2,
      digitalWrite(LED_BLUE,LOW);     
      Serial.print(mm);
      Serial.print("mm");
      Serial.println();
    }
     else
     {
      // payload_buffer = payload_buffer"Out of Range";
      // lmh_app_data_t lorawan_payload = {
      //   payload_buffer, strlen("Out of Range"), 1, 0, 0,
      // };
      Serial.println("Out of Range"); 
       
    } 

    if (isLoRaWANConnected()) {
        Serial.println("Send payload");
        // send sendLoRaWANFrame will do nothing if not connected anyway, but it's best practice to check
        sendLoRaWANFrame(&lorawan_payload);
    } else {
        // else log that it's not connected
        Serial.println("LoRaWAN not connected. Try again later.");
    }
    
}


long int duration_time()
{
   long int respondTime;
   pinMode(TRIG, OUTPUT);
   digitalWrite(TRIG, HIGH);
   delayMicroseconds(12);   //pull high time need over 10us 
   digitalWrite(TRIG, LOW);  
   pinMode(ECHO, INPUT);
   respondTime = pulseIn(ECHO, HIGH); // microseconds 
   delay(33);
   if(RAK4631_BOARD)
   {
     respondTime = respondTime*0.7726; // Time calibration factor is 0.7726,to get real time microseconds for 4631board
   }
   Serial.printf("respond time is %d\r\n",respondTime);

   if((respondTime>0)&&(respondTime < TIME_OUT))  //ECHO pin max timeout is 33000us according it's datasheet 
   {
    return respondTime;
   }
   else
   {
     return -1;  
   }   
}