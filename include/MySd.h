#include "SPI.h"
#include "FS.h"
#include "SD.h"
/*
 * pin 1 - not used          |  Micro SD card     |
 * pin 2 - CS (SS)           |                   /
 * pin 3 - DI (MOSI)         |                  |__
 * pin 4 - VDD (3.3V)        |                    |
 * pin 5 - SCK (SCLK)        | 8 7 6 5 4 3 2 1   /
 * pin 6 - VSS (GND)         | ▄ ▄ ▄ ▄ ▄ ▄ ▄ ▄  /
 * pin 7 - DO (MISO)         | ▀ ▀ █ ▀ █ ▀ ▀ ▀ |
 * pin 8 - not used          |_________________|
 *                             ║ ║ ║ ║ ║ ║ ║ ║
 *                     ╔═══════╝ ║ ║ ║ ║ ║ ║ ╚═════════╗
 *                     ║         ║ ║ ║ ║ ║ ╚══════╗    ║
 *                     ║   ╔═════╝ ║ ║ ║ ╚═════╗  ║    ║
 * Connections for     ║   ║   ╔═══╩═║═║═══╗   ║  ║    ║
 * full-sized          ║   ║   ║   ╔═╝ ║   ║   ║  ║    ║
 * SD card             ║   ║   ║   ║   ║   ║   ║  ║    ║
 * Pin name         |  -  DO  VSS SCK VDD VSS DI CS    -  |
 * SD pin number    |  8   7   6   5   4   3   2   1   9 /
 *                  |                                  █/
 *                  |__▍___▊___█___█___█___█___█___█___/
 *
 * Note:  The SPI pins can be manually configured by using `SPI.begin(sck, miso, mosi, cs).`
 *        Alternatively, you can change the CS pin and use the other default settings by using `SD.begin(cs)`.
 *
 * +--------------+---------+-------+----------+----------+----------+
 * | SPI Pin Name | ESP8266 | ESP32 | ESP32-S2 | ESP32-C3 | ESP32-S3 |
 * +==============+=========+=======+==========+==========+==========+
 * | CS (SS)      | GPIO15  | GPIO5 | GPIO5    | GPIO13   | GPIO13   |
 * +--------------+---------+-------+----------+----------+----------+
 * | DI (MOSI)    | GPIO13  | GPIO23| GPIO24   | GPIO14   | GPIO14   |
 * +--------------+---------+-------+----------+----------+----------+
 * | DO (MISO)    | GPIO12  | GPIO19| GPIO25   | GPIO15   | GPIO15   |
 * +--------------+---------+-------+----------+----------+----------+
 * | SCK (SCLK)   | GPIO14  | GPIO18| GPIO19   | GPIO16   | GPIO16   |
 * +--------------+---------+-------+----------+----------+----------+
 *
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 */

/*
If you don’t pass any argument to the begin() function, it will try to initialize SPI communication with the 
microSD card on the default chip select (CS) pin. If you want to use another CS pin, you can pass it as an 
argument to the begin() function. For example, if you wanted to use GPIO 17 as a CS pin, you should use the 
following lines of code:

if(!SD.begin(17))

To use other SPI pins, you can proceed as follows:

At the beginning of your code, declare the pins you want to use, for example:

#define SCK  17
#define MISO  19
#define MOSI  23
#define CS  5

Create a new SPI class on HSPI or VSPI. We’re using VSPI. Both will work fine.

SPIClass spi = SPIClass(VSPI);

In the setup(), initialize SPI communication protocol on the pins defined previously:

spi.begin(SCK, MISO, MOSI, CS);

Finally, initialize the microSD card with the begin() method. Pass as argument the CS pin, the SPI instance you want to use, and the bus frequency.

if (!SD.begin(CS,spi,80000000)) {
  Serial.println("Card Mount Failed");
  return;
}
*/



class MySd
{
    public:


    // Constructor
    MySd() 
    {
    }
    
    // Destructor
    ~MySd()  
    {
    }

    //********************************************************************************
    //* Name : mountSd
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void mountSd(const char* hv)
    {
        Serial.printf("Mounting SD Card using %sSPI\n", hv);
        
        if (hv[0] == 'h' || hv[0] == 'H')
        {
            int sck = 14;   int miso = 12;  int mosi = 13;  int cs = 15;
            SPIClass spi = SPIClass(HSPI);
            spi.begin(sck, miso, mosi, cs);
            if (!SD.begin(cs, spi, 80000000)) 
            {
                Serial.println("Card Mount Failed");
                return;
            }
        }
        else
        {
            //if(!SD.begin(cs)){ //Change to this function to manually change CS pin
            if(!SD.begin())
            {
                Serial.println("Card Mount Failed");
                return;
            }
        }

        // get the card type
        uint8_t cardType = SD.cardType();

        if(cardType == CARD_NONE)
        {
            Serial.println("No SD card attached");
            return;
        }

        Serial.print("SD Card Type: ");
        if(cardType == CARD_MMC) 
        {
            Serial.println("MMC");
        } else if(cardType == CARD_SD){
            Serial.println("SDSC");
        } else if(cardType == CARD_SDHC){
            Serial.println("SDHC");
        } else {
            Serial.println("UNKNOWN");
        }

    }
   
    //********************************************************************************
    //* Name : listDir
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
    {
        Serial.printf("Listing directory: %s\n", dirname);

        File root = fs.open(dirname);
        if(!root)
        {
            Serial.println("Failed to open directory");
            return;
        }
        if(!root.isDirectory())
        {
            Serial.println("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while(file)
        {
            if(file.isDirectory())
            {
                Serial.print("  DIR : ");
                Serial.println(file.name());
                if(levels){
                listDir(fs, file.name(), levels -1);
                }
            } 
            else 
            {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("  SIZE: ");
                Serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }

    //********************************************************************************
    //* Name : createDir
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void createDir(fs::FS &fs, const char * path)
    {
        Serial.printf("Creating Dir: %s\n", path);
        if(fs.mkdir(path)){
        Serial.println("Dir created");
        } else {
        Serial.println("mkdir failed");
        }
    }

    //********************************************************************************
    //* Name : removeDir
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void removeDir(fs::FS &fs, const char * path)
    {
        Serial.printf("Removing Dir: %s\n", path);
        if(fs.rmdir(path))
        {
            Serial.println("Dir removed");
        } 
        else 
        {
            Serial.println("rmdir failed");
        }
    }

    //********************************************************************************
    //* Name : readFile
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void readFile(fs::FS &fs, const char * path)
    {
        Serial.printf("Reading file: %s\n", path);

        File file = fs.open(path);
        if(!file)
        {
            Serial.println("Failed to open file for reading");
            return;
        }

        Serial.print("Read from file: ");
        while(file.available())
        {
            Serial.write(file.read());
        }
        file.close();
    }

    //********************************************************************************
    //* Name : writeFile
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void writeFile(fs::FS &fs, const char * path, const char * message)
    {
        Serial.printf("Writing file: %s\n", path);

        File file = fs.open(path, FILE_WRITE);
        if(!file){
        Serial.println("Failed to open file for writing");
        return;
        }
        if(file.print(message)){
        Serial.println("File written");
        } else {
        Serial.println("Write failed");
        }
        file.close();
    }

    //********************************************************************************
    //* Name : appendFile
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void appendFile(fs::FS &fs, const char * path, const char * message)
    {
        Serial.printf("Appending to file: %s\n", path);

        File file = fs.open(path, FILE_APPEND);
        if(!file){
        Serial.println("Failed to open file for appending");
        return;
        }
        if(file.print(message)){
            Serial.println("Message appended");
        } else {
        Serial.println("Append failed");
        }
        file.close();
    }

    //********************************************************************************
    //* Name : renameFile
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void renameFile(fs::FS &fs, const char * path1, const char * path2)
    {
        Serial.printf("Renaming file %s to %s\n", path1, path2);
        if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
        } else {
        Serial.println("Rename failed");
        }
    }

    //********************************************************************************
    //* Name : deleteFile
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void deleteFile(fs::FS &fs, const char * path)
    {
        Serial.printf("Deleting file: %s\n", path);
        if(fs.remove(path)){
        Serial.println("File deleted");
        } else {
        Serial.println("Delete failed");
        }
    }

//********************************************************************************
    //* Name : fileExists
    //* Desc : Return true if the file exists
    //* Parm : const char* file path
    //* Retn : bool
    //********************************************************************************
    bool fileExists(fs::FS &fs, const char * path)
    {
        File file = fs.open(path);
        static uint8_t buf[512];
        size_t len = 0;
        uint32_t start = millis();
        uint32_t end = start;
        if(file)
        {
            file.close();
            Serial.printf("File %s exists.\n", path);
            return true;
        } 
        else 
        {
            Serial.printf("File %s Does Not Exist !!!\n", path);
            file.close();
            return false;
        }
    }

    //********************************************************************************
    //* Name : testFileIO
    //* Desc : 
    //* Parm : n/a
    //* Retn : void
    //********************************************************************************
    void testFileIO(fs::FS &fs, const char * path)
    {
        File file = fs.open(path);
        static uint8_t buf[512];
        size_t len = 0;
        uint32_t start = millis();
        uint32_t end = start;
        if(file)
        {
            len = file.size();
            size_t flen = len;
            start = millis();
            while(len)
            {
                size_t toRead = len;
                if(toRead > 512){
                toRead = 512;
                }
                file.read(buf, toRead);
                len -= toRead;
            }
            end = millis() - start;
            Serial.printf("%u bytes read for %u ms\n", flen, end);
            file.close();
        } 
        else 
        {
            Serial.println("Failed to open file for reading");
        }


        file = fs.open(path, FILE_WRITE);
        if(!file) 
        {
            Serial.println("Failed to open file for writing");
          return;
        }

        size_t i;
        start = millis();
        for(i=0; i<2048; i++) 
        {
            file.write(buf, 512);
        }
        end = millis() - start;
        Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
        file.close();
    }
  
};


