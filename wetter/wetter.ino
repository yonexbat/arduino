/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.


  ssh -o KexAlgorithms=diffie-hellman-group14-sha1 -oHostKeyAlgorithms=+ssh-dss root@10.0.0.16
  CONSOLE: telnet localhost 6571
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Console.h>
#include <FileIO.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
     //Initialize Console and wait for port to open:
    Bridge.begin();
    Console.begin();
    FileSystem.begin();
    
    Console.println(F("BME280 test"));

    unsigned status;
    
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();  
    if (!status) {
        Console.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Console.print("SensorID was: 0x"); 
        Console.println(bme.sensorID(),16);
        Console.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Console.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Console.print("        ID of 0x60 represents a BME 280.\n");
        Console.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }
    
    Console.println("-- Default Test --");
    delayTime = 10000;

    Console.println();
}


void loop() { 
    printValues();
    delay(delayTime);
}


void printValues() {
    Console.print("Temperature = ");
    Console.print(bme.readTemperature());
    Console.println(" *C");

    Console.print("Pressure = ");

    Console.print(bme.readPressure());
    Console.println(" hPa");

    Console.print("Approx. Altitude = ");
    Console.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Console.println(" m");

    Console.print("Humidity = ");
    Console.print(bme.readHumidity());
    Console.println(" %");

    Console.println();

    File dataFile = FileSystem.open("/mnt/sda1/datalog.txt", FILE_APPEND);
    if(dataFile) {
      dataFile.print(getTimeStamp());
      dataFile.print(";");
      dataFile.print(bme.readTemperature());
      dataFile.print(";");
      dataFile.print(bme.readPressure());
      dataFile.print(";");
      dataFile.print(bme.readHumidity());
      dataFile.println(";");
      dataFile.close();
    }
}

String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%F-%T");  // parameters: D for the complete date mm/dd/yy
  //             T for the time hh:mm:ss
  time.run();  // run the command

  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n') {
      result += c;
    }
  }

  return result;
}
