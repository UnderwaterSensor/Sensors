/******************
This is all the sepearte parts together
- sd card
- clock
- temperature
- ph value
******************/

/*includes*/
#include <OneWire.h> // one wire communication
#include <DallasTemperature.h> // for the temeparture sensor
#include <SD.h> //library for teh SD Card
#include <Wire.h> // part of the realtime 
#include <avr/sleep.h>
#include <DS3231.h>



/*variables*/
float temprature = 0;
String time_measurement =" ";
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
int chipSelect = 4; //chip select pin for the MicroSD Card Adapter
File file; // file object that is used to read and write data
String NameOfFile = "file1457.txt";
String celcius = " ";
String fahrenheit = " ";
String TotalData = " ";

/*set up temperature*/
// Data wire of temeparture sensor is conntec to the Arduino digital pin 2
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
/* end temperature*/


/* set up realtime clock */  
#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}
/* end define clock */


void setup() {
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // for the clock
  Wire.begin(); 

  
  // Start up the library for temperature
  sensors.begin();
  
  // Start SD Card
    pinMode(chipSelect, OUTPUT); // chip select pin must be set to OUTPUT mode
  if (!SD.begin(chipSelect)) { // Initialize SD card
    Serial.println("Could not initialize SD card."); // if return value is false, something went wrong.
  }
  
  /*if (SD.exists(NameOfFile)) { // if "file.txt" exists, fill will be deleted
    Serial.println(NameOfFile + " File exists.");
    file.close();
    if (SD.remove(NameOfFile) == true) {
      Serial.println("Successfully removed file.");
    } else {
      Serial.println("Could not removed file.");
    }
  }*/
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(30,07,20,6,29,01,21);
  
  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  //setDS3231time(05,59,17,6,5,2,21);
}
// void clock
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime(){
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  
  time_measurement = String(dayOfMonth) + "/" + String(month) + "/" + String(year) + ", " + String(hour) + ":" + String(minute) +", ";
}


void loop() {
  // clock loop
  displayTime(); // display the real-time clock data on the Serial Monitor,
  //Serial.println(time_measurement);
  
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  celcius = String (sensors.getTempCByIndex(0));
  fahrenheit = String (sensors.getTempFByIndex(0));
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  //Serial.print(celcius); 
  //Serial.print(", ");
  //Serial.print(fahrenheit);
  //Serial.print(", ");
  
//write to file
  file = SD.open(NameOfFile, FILE_WRITE); // open "file.txt" to write data
  TotalData = time_measurement + celcius + ", " + fahrenheit;
  if (file) {
    //int number = random(10); // generate random number between 0 and 9
    file.println(TotalData); // write number to file
    file.close(); // close file
   // Serial.print("Wrote number: "); // debug output: show written number in serial monitor
  Serial.println(TotalData);
  } else {
    Serial.println("Could not open file (writing).");
  }
  
  /*file = SD.open(NameOfFile, FILE_READ); // open "file.txt" to read data
  if (file) {
    // Serial.println("- – Reading start – -");
    char character;
    while ((character = file.read()) != -1) { // this while loop reads data stored in "file.txt" and prints it to serial monitor
      Serial.print(character);
    }
    file.close();
    //Serial.println("- – Reading end – -");
  } else {
    Serial.println("Could not open file (reading).");
  }
*/
   
  delay(150000); // every 10 seconds
  //time_measurement ="";
}
