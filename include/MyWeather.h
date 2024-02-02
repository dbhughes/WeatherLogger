#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>



class MyWeather
{
  public:
    // Constructor
    MyWeather() 
    {
    }
    
    // Destructor
    ~MyWeather()  
    {
    }


    const char* weatherURL="https://api.openweathermap.org/data/2.5/weather?lat=36.0874&lon=-93.7350&appid=de69ec242df9a3d501bf1d8191ed088e&units=imperial";
    String payload="";
    String message = "";

    char cWeather[100];
    char cHot[100];
    char cWindSpd[100];
    char cWindDir[100];
    char timeMsg[15];

    JsonDocument doc;

//********************************************************************************
//* Name : getWeatherData
//* Desc : Goes out to the weather station and reads the weather info in JSON
//* Parm : n/a
//* Retn : void
//********************************************************************************
void getWeatherData()
{
  //Check the current connection status
  if ((WiFi.status() == WL_CONNECTED)) 
  { 
    // Make an HTTP object
    HTTPClient http;

    // Setup the weather data URL
    http.begin(weatherURL); //Specify the URL

    // Tell us that we are getting the weather info
    Serial.print("Requesting Weather data  ");

    //Make the GET request to the weather URL
    int httpCode = http.GET();  
    
    //Check for the returning code to be greater than 0
    if (httpCode > 0) 
    { 
      // Get the web page responce into payload
      payload = http.getString();
      
      // make a char array that is the length of the payload string
      char inp[payload.length()];    

      // Copy the payload string into the char array
      payload.toCharArray(inp,payload.length());

      // Deserialize the input string into a json object
      deserializeJson(doc,inp);  
/*
      // The filter: it contains "true" for each value we want to keep
      StaticJsonDocument<400> filter;
      
      filter["main"]["temp"] = true;
      filter["weather"][0]["main"] = true;
      filter["wind"]["speed"] = true;
      filter["wind"]["deg"] = true;

      deserializeJson(doc, inp, DeserializationOption::Filter(filter));
*/

      // Print the JSON in an easy to read format
//      Serial.println("Fancy ");
//      serializeJsonPretty(doc, Serial);

      String weather = doc["weather"][0]["main"];
      String hot = doc["main"]["temp"];
      String windSpd = doc["wind"]["speed"];
      String windDir = doc["wind"]["deg"];

      weather.toCharArray(cWeather, weather.length()+1);
      hot.toCharArray(cHot, hot.length()+1);
      windSpd.toCharArray(cWindSpd, windSpd.length()+1);
      windDir.toCharArray(cWindDir, windDir.length()+1);

      message = timeMsg;  message += "\n";  
      //message += ip + "\n";  
      message += weather + "\n";  
      message += hot+ " Deg (F)\n";
      message += windSpd+ " Mph\n";
      message += windDir+ " Deg\n";
      
      strcat(cHot, " Deg (F)");
      strcat(cWindSpd, " Mph");
      strcat(cWindDir, " Deg");


      Serial.println("");
      Serial.print("The weather is: ");   Serial.println(weather);
      Serial.print("The Temp is: ");      Serial.println(hot);
      Serial.print("The wind is: ");      Serial.println(windSpd);
      Serial.print("The wind at: ");      Serial.println(windDir);


      return;        
    }
  }
}


  
};
