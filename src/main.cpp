
#include "FS.h"
#include "SD.h"
#include "time.h"

#include "MyFi.h"
#include "MySd.h"
#include "MyNtp.h"
#include "MyWeather.h"


MySd mysd;
MyWeather myw;
MyNtp myntp;



char txtBuf[250];

// Assign names to Pushbutton pins
const int PushButton0 = 36;
const int PushButton1 = 0;
const int Led0 = 2;


const char* logFileName="/WLog.csv";

//********************************************************************************
//* Name : blinkLed
//* Desc : Blink the onboard Led
//* Parm : n/a
//* Retn : void
//********************************************************************************
void blinkLed(int amt)
{
    int x;
    for (x=0; x < amt; x++)
    {
        digitalWrite(Led0, HIGH);
        delay(500);
        digitalWrite(Led0, LOW);
        delay(500);
    }
}

//********************************************************************************
//* Name : setup
//* Desc : 
//* Parm : n/a
//* Retn : void
//********************************************************************************
void setup()
{
    Serial.begin(115200);
    while(!Serial) { delay (10); }
    Serial.print("\n\n");
    Serial.println("Dereks Weather logging system is now running.");       


    // Set the pin mode for the pushbutton
    pinMode(PushButton0, INPUT_PULLUP);
    pinMode(PushButton1, INPUT);
    pinMode(Led0, OUTPUT);

    // Turn led0 off
    blinkLed(4);

    // Instantiate object from MyFi Class to connect to WiFi
    MyFi Wap;

    // Use object method to connect to WAP
    Serial.println("Scanning WAPS to connct to one.");       
    Wap.ScanWapsAndConnect();

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());


    // Configure RTC settings
    configTime(myntp.gmtOffset_sec, myntp.daylightOffset_sec, myntp.ntpServer);
    
    // Set the ESP32 RTC from specified NTP server
    myntp.SetRtcFromNtp();

    // Get the weather data
    myw.getWeatherData();

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());

    // Mount the SD card using VSPI
    mysd.mountSd("V");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);    
    Serial.printf("Free space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    

    if (!mysd.fileExists(SD, logFileName))
    {
        mysd.writeFile(SD, logFileName, "");
    }

    mysd.readFile(SD, logFileName);

}

//********************************************************************************
//* Name : loop
//* Desc : 
//* Parm : n/a
//* Retn : void
//********************************************************************************
void loop()
{
    char weatherBuf[250];
    // Pushbutton delay time
    unsigned long startMillis;

    // Weather Check Delay time
    unsigned long WeatherDelayMillis;
    unsigned long WeatherMinutesInterval = 5 * 60 * 1000;  // 5 minutes
    unsigned long LedOnMillis;
    unsigned long LedOffMillis;

    // Setup millis weather timeout for 5 minutes
    WeatherDelayMillis = millis();
    LedOnMillis = millis() +  1000;    LedOffMillis = LedOnMillis + 150;


    // Loop forever
    while(1)
    {
        // Check for pushbutton 0 to read file
        if (digitalRead(PushButton1) == LOW )
        {
            Serial.println("\n-----------------------------------------------------------");
            mysd.readFile(SD, logFileName);
            Serial.println("End of file.\n");
        }


        // Check for pushbutton 0
        if (digitalRead(PushButton0) == LOW )
        { 
            // Turn led0 on
            digitalWrite(Led0, HIGH);    
            //
            Serial.println("You have pressed the file reset button.");       
            // Wait a debounce bit
            delay(100);
            // Loop until button is released
            while(digitalRead(PushButton0) == LOW );
            // Wait a debounce bit
            delay(100);
            // Loop for 2 seconds waiting for button to be pressed again
            Serial.printf("To reset the %s log file, You have 3 seconds to push the button again\n", logFileName);
            startMillis = millis();
            while( millis() < startMillis + 3000)
            {
                // If button pressed again then clear file and blink 3 times
                if (digitalRead(PushButton0) == LOW )
                { 
                    // Erase file if it exists
                    if (mysd.fileExists(SD, logFileName))
                    {
                        // Make a new clean file on the SD
                        mysd.deleteFile(SD, logFileName);
                        mysd.writeFile(SD, logFileName, "");
                        Serial.printf("Deleted Old and created new %s file\n", logFileName);
                    }

                    // Blink Led 3 times
                    blinkLed(3);

                    // Break out of 2 second loop
                    break;
                }

                // Turn led0 off
                digitalWrite(Led0, LOW);                
            }
        }

        // Check the weather every 5 minutes
        if (millis() > WeatherDelayMillis)
        {
            // Reset delay amount
            WeatherDelayMillis = millis() + WeatherMinutesInterval;

            // Get the network time
            myntp.SetRtcFromNtp();

            // Get the RTC Time into myntp.txtTime
            myntp.GetTimeFromRtc();

            // Get the weather data
            myw.getWeatherData();
            
            // make the weather string
            sprintf(weatherBuf, "%20S,%20s,%15s,%10s,%10s\r\n", myntp.txtTime, myw.cWeather, myw.cHot, myw.cWindSpd, myw.cWindDir);

            // Append the weather to the file
            mysd.appendFile(SD, logFileName, weatherBuf);
        }

        if (millis() > LedOnMillis)
        {
            digitalWrite(Led0, HIGH);    
        }

        if (millis() > LedOffMillis)
        {
            LedOnMillis = millis() +  1000;    LedOffMillis = LedOnMillis + 150;
            digitalWrite(Led0, LOW);    
        }



  }
}
