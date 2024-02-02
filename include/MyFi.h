#include "Arduino.h"
#include "secrets.h"
#include <WiFi.h>

struct 
{
  char ssid[20];    // Member (string variable)
  char pw[20];      // Member (string variable)
} mySecrets[5];     // Structure variable

class MyFi
{
  public:
    // Constructor
    MyFi() 
    {
        InitSecrets();
    }
    // Destructor
    ~MyFi()  
    {
    }

    void InitSecrets()
    {
      strcpy(mySecrets[0].ssid, ssid0);
      strcpy(mySecrets[0].pw, pass0);
    
      strcpy(mySecrets[1].ssid, ssid1);
      strcpy(mySecrets[1].pw, pass1);
    
      strcpy(mySecrets[2].ssid, "-");
      strcpy(mySecrets[2].pw, "-");
    
      strcpy(mySecrets[3].ssid, "-");
      strcpy(mySecrets[3].pw, "-");
    
      strcpy(mySecrets[4].ssid, "-");
      strcpy(mySecrets[4].pw, "-");
    
    }

    void ScanWapsAndConnect()
    {
        char ssid[50];
        int n,j;
    
        // Setup the secrets list
        InitSecrets();
        
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        // Make a list of the available WAPs
        n = WiFi.scanNetworks();
        Serial.println("Scanning networks...");
        
        // If there are no WAPs available then return
        if (n == 0) 
        {
            Serial.println("no networks found");
            return;
        } 
        else 
        {
            Serial.print(n);
            Serial.println(" networks found");
            // If WAPs found see if one of them is authorized and log in
            for (int i = 0; i < n; ++i) 
            {
                // Get the SSID from current index in the scanned list
                sprintf(ssid, "%s", WiFi.SSID(i).c_str());
                
                // Loop through all of the WAP we are authorized to connect to
                for (j = 0; j < 10; j++)
                {
                    // Check for the end of the list of authorized WAPs
                    if (strcmp("-", mySecrets[j].ssid) == 0)
                    {
                        break;
                    }
                    // If the SSID is in the list then log in using it
                    if (strcmp(ssid, mySecrets[j].ssid) == 0)
                    {
                        Serial.print("Connecting ");   Serial.print(ssid);  Serial.print(" ");    
                        WiFi.begin(mySecrets[j].ssid, mySecrets[j].pw);
                        // Loop until connection is made
                        while ( WiFi.status() != WL_CONNECTED) 
                        {                
                            Serial.print(".");
                            delay(500);
                        }                    
                        // Show that we are connected
                        Serial.print("\nConnected to ");    Serial.println(WiFi.SSID());
                    }
                }
                
                delay(100);
            }
            Serial.println();
        }
 
        // Delete the scan result to free memory for code below.
        WiFi.scanDelete();    
    }


  
};
