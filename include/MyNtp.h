#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "time.h"

#include <ESP32Time.h>

ESP32Time rtc(0);  


class MyNtp
{
    public:


    // Constructor
    MyNtp() 
    {
    }
    
    // Destructor
    ~MyNtp()  
    {
    }

    const char* ntpServer = "pool.ntp.org";
    // offset in seconds GMT-6
    int gmtOffset_sec = -6 * 3600;
    // offset in seconds DST
    int daylightOffset_sec = 3600;
    // Buffer for time
    char txtTime[25];




  //********************************************************************************
  //* Name : SetRtcFromNtp
  //* Desc : Set the ESP32 Real Time Clock using the network time protocol
  //* Parm : n/a
  //* Retn : void
  //********************************************************************************
  void SetRtcFromNtp() 
  {
    int x;
    struct tm timeinfo;

    Serial.print("Retrieving Time using NTP ");

    for (x = 0; x < 5; x++)
    {
      if (getLocalTime(&timeinfo))
      {
        rtc.setTimeStruct(timeinfo); 
        Serial.println(" Time set from NTP Server");
        strftime(txtTime, 26, "%Y-%m-%d %H:%M:%S", &timeinfo);
        Serial.println(txtTime);
        return;
      }
      else
      {
        Serial.print(".");
      }  
      
    }
    Serial.println("NTP Server not found");

  }

  //********************************************************************************
  //* Name : GetTimeFromRtc
  //* Desc : get the time from the RTC into txtTime
  //* Parm : n/a
  //* Retn : void
  //********************************************************************************
  void GetTimeFromRtc() 
  {
    int x;
    struct tm timeinfo;
    // Get the timr from the RTC into a tm struct
    getLocalTime(&timeinfo);
    strftime(txtTime, 26, "%Y-%m-%d %H:%M:%S", &timeinfo);
  }

  
};
