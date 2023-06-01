/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>

#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1); // change Serial1 pins in HardwareSerial.cpp file
int fingerprintID = 0;
String IDname;

void setup(){
  //Fingerprint sensor module setup
  Serial.begin(9600);
  Serial2.begin(115200);
  // set the data rate for the sensor serial port
  finger.begin(57600);

  Serial.println(finger.verifyPassword());
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  }
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
}

void loop(){
  // displayMainScreen();
  fingerprintID = getFingerprintIDez();
  delay(1500);
  Serial.print("ID: ");
  Serial.println(fingerprintID);
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); 
  Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); 
  Serial.println(finger.confidence);
  return finger.fingerID; 
}

// void displayMainScreen(){
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(7,5);
//   display.println("Waiting fingerprint");
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(52,20);
//   display.println("...");  
//   display.display();
//   delay(2000);
// }

// void displayUserGreeting(String Name){
//   display.clearDisplay();
//   display.setTextColor(WHITE);
//   display.setTextSize(2);
//   display.setCursor(0,0);
//   display.print("Hello");
//   display.setCursor(0,15);
//   display.print(Name); 
//   display.display();
//   delay(5000);
//   fingerprintID = 0; 
// }
