//#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <SPI.h>
#include <SD.h>

int chipSelect = 10;  // CS pin for SD card
int lowerLimitCO2 = 700;
int upperLimitCO2 = 900;
int lowerLimitTemp = 18;
int upperLimitTemp = 23;
int lowerLimitHumid = 80;
int upperLimitHumid = 90;
int lowerLimitLight = 10;  // light will switch on at 10 o'clock.
int upperLimitLight = 18;  // light will switch off at 18 o'clock
boolean cardPresent = false;
boolean relayStatus1;
boolean relayStatus2;
boolean relayStatus3;
boolean relayStatus4;
boolean socketStatus1;
boolean socketStatus2;
boolean socketStatus3;
String dataString = "";

// Relays
// const int pinRelay1 = 18; // not in use
const int pinRelay2 = 19; // circulating air fan pin
const int pinRelay3 = 20; // fresh air fan pin 
// const int pinRelay4 = 21; // not in use

//MH-Z19
byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
char response[9];
#define pwmPin 6
int prevVal = LOW;
long th, tl, h, l, ppm = 0.0;

// DHT22
//#define DHTPIN 5
//#define DHTTYPE DHT22

DHT dht(5, DHT22);  // DHTpin, DHTtype
RCSwitch mySwitch = RCSwitch();
LiquidCrystal_I2C lcd(0x20, 16, 2); // LCD-Display 16x2 oder 20x4


void setup() {
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB
//  }    
  mySwitch.enableTransmit(7);  // FM-Transmitter auf Pin 7; optional: Pulslänge setzen: mySwitch.setPulseLength(320);
  pinMode(pinRelay2, OUTPUT); // set fresh air fan pin as output pin
  pinMode(pinRelay3, OUTPUT); // set circulating air fan pin as output pin

  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("Pilzparadies 1.0");
  lcd.backlight();
  delay(1000);
  // lcd.clear();

    // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) { // LCD-Ausgabe einfügen
    lcd.setCursor(0,1);
    lcd.print("SD card error");
    cardPresent = false;
    Serial.println("SD card error (read failed or card not present)");
    delay(3000);
    lcd.clear();
    // don't do anything more:
    return;
  } else {
    lcd.setCursor(0,1);
    lcd.print("SD card okay");
    cardPresent = true;
    Serial.println("Card initialized.");
    delay(500);
  }
  lcd.clear();

  // Read thresholds from settings file
  File myFile;
  myFile = SD.open("settings.txt", FILE_READ);  
  if (myFile) {
    Serial.println("settings.txt:");
    while (myFile.available()>0) {   // obere und untere Grenzwerte durch Komma, Gruppen durch Semikolon getrennt
      lowerLimitCO2 = myFile.parseInt();
      upperLimitCO2 = myFile.parseInt();
      lowerLimitTemp = myFile.parseInt();
      upperLimitTemp = myFile.parseInt();
      lowerLimitHumid = myFile.parseInt();
      upperLimitHumid = myFile.parseInt();      
      lowerLimitLight = myFile.parseInt();
      upperLimitLight = myFile.parseInt();            
      if (Serial.read() == '\n'){
        break;
      }
      Serial.println("Eingelesene Werte (CO2, Temp, Humid, Light)");
      Serial.println(lowerLimitCO2);
      Serial.println(upperLimitCO2);
      Serial.println(lowerLimitTemp);
      Serial.println(upperLimitTemp);
      Serial.println(lowerLimitHumid);
      Serial.println(upperLimitHumid);
      Serial.println(lowerLimitLight);
      Serial.println(upperLimitLight);
      Serial.println();
      
      
//      byte piss = "0";
//      int shit;
//      Serial.println(piss);
//      while (int(piss) >= 32 && int(piss) <=127){
//        Serial.println("2");
//        while (piss != "," && piss != ";"){    
//          Serial.println("3");
//          shit *= 10;
//          shit += int (piss);
//          piss = myFile.read();
//          Serial.println(piss);
//        } 
//          Serial.print("Shit!!!!! :)");
//          Serial.println(shit);      
//      }
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening settings.txt");
  }

    
  
  // Wo gehört das hin?
  pinMode(pwmPin, INPUT);
  dht.begin();
}

void loop() {

  tmElements_t tm;
  if (!RTC.read(tm)) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RTC error:");
    lcd.setCursor(0,1);    
      if (RTC.chipPresent()) {
      lcd.print("DS1307 stopped");
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println();
    } else {
      lcd.print("Read error");
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(500);
    lcd.clear();
  }

  // CO2 via PWM (mal testweise nach Zeit, dafür ohne delay)
  if (tm.Second % 2 == 0){
    do {
      th = pulseIn(pwmPin, HIGH, 1004000) / 1000.0;
      tl = 1004 - th;
      ppm = 5000 * (th - 2) / (th + tl - 4);
  //            Serial.println(tl);
  //            Serial.println(th);
  //            Serial.println(ppm);
    } while (ppm < 0.0);
  // delay(1000);
  }

  // READ DATA DHT22
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();  
  if (tm.Second % 2 == 1){   // (mal testweise nach Zeit, versetzt zum CO2-Sensor) 
    if (isnan(h) || isnan(t)) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("DHT error");
      Serial.println("DHT error");
      delay(500);
      lcd.clear();
      return;
    }
  // Compute heat index in Fahrenheit (the default)
  // float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  }

  if (RTC.read(tm)) {  // eleganter lösen
    // das muss woanders hin
    dataString = print2digits(tm.Day) + "." + print2digits(tm.Month) + "." + String (tmYearToCalendar(tm.Year)) + " " + print2digits(tm.Hour) + ":" + print2digits(tm.Minute) + ":" + print2digits(tm.Second) + "\t" + String(ppm) + "\t" + String(t) + "\t" + String(h) + "\t" + String(relayStatus1) + "\t" + String(relayStatus2) + "\t" + String(relayStatus3) + "\t" + String(relayStatus4) +  "\t" + String(socketStatus1) + "\t" + String(socketStatus2) + "\t" + String(socketStatus3) + "\r\n";
  } 
  Serial.print(dataString);
  // delay(500); // Test
 
  // Uhrzeit
  lcd.setCursor(0, 0);
  // lcd.print("Uhrzeit:       ");
  if (tm.Hour < 10){
    lcd.print(" ");
  }    
  lcd.print(tm.Hour);
  lcd.print(":");
  if (tm.Minute < 10){
    lcd.print("0");
  }     
  lcd.print(tm.Minute);
  
  lcd.setCursor(8, 0);
  // lcd.print("CO2:    ");
  for (int a = 0 ; a < 4-String(ppm).length(); a += 1){
    lcd.print(" ");
  }
  lcd.print(ppm);
  lcd.print(" ppm");

  
  lcd.setCursor(0, 1);
  // lcd.print("rel. Luftf.:     ");
  if (h < 10){
    lcd.print(" ");
  }
  lcd.print(h, 0);
  lcd.print("%");
  
  lcd.setCursor(10, 1);
  // lcd.print("Temperatur:   ");
  if (t < 10){
    lcd.print(" ");
  }  
  lcd.print(t, 1);
  lcd.write(223);
  lcd.print("C");
  
  // Serial.println(tm.Hour); 
  if((tm.Hour >= lowerLimitLight) && (tm.Hour < upperLimitLight)){
    socketStatus3 = true;
    mySwitch.switchOn("01111", "00100"); // Funksteckdose 03 an  
  } else {
    mySwitch.switchOff("01111", "00100"); // Funksteckdose 03 aus
    socketStatus3 = false;
    }    
    
  // Switch on/off heater if lower/upper temperature limit is reached
  if(t <= lowerLimitTemp){
    socketStatus2 = true;
    mySwitch.switchOn("01111", "01000"); // Funksteckdose 02 an  
  } else if(t >= upperLimitTemp) {
    socketStatus3 = false;
    mySwitch.switchOff("01111", "01000"); // Funksteckdose 02 aus
    }

  // Switch on/off humidifier if lower/upper humidity limit is reached
  if(h <= lowerLimitHumid){
    socketStatus1 = true;
    mySwitch.switchOn("01111", "10000"); // Funksteckdose 01 an  
  } else if(h >= upperLimitHumid) {
    socketStatus3 = false;
    mySwitch.switchOff("01111", "10000"); // Funksteckdose 01 aus
    } 

  // Switch on Fan 1 (fresh air) if CO2 reaches upper limit and switch it off when it reaches lower limit
  if(ppm >= upperLimitCO2){
    digitalWrite(pinRelay3, HIGH); 
    relayStatus1 = true;
  } else if(ppm <= lowerLimitCO2) {
    digitalWrite(pinRelay3, LOW); 
    relayStatus1 = false;
    }   
    
  // Switch on Fan 2 every minutes for 1o seconds
  if(tm.Second < 10){
    digitalWrite(pinRelay2, HIGH);
    relayStatus2 = true;
  } else {
    digitalWrite(pinRelay2, LOW);
    relayStatus2 = false;
  }        

  // Log data on SD card 
  if(tm.Minute % 2 == 0 && tm.Second == 0){
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println("Write to SD: ");
      Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SD card write fail");
      Serial.println("SD card write fail");
      delay(5000);
    }   
  } else {
  }         
}

String print2digits(int number) {
  String digits;
  if (number >= 0 && number < 10) {
    digits = "0";
  }
  digits += number;
  return(digits);
}
