#include <Wire.h>

#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

#include <SSLClient.h>
#include <ssl_client.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <ArduinoHttpClient.h>
#include "ca_cert.h"


// ESP32 LilyGo-T-Call-SIM800 SIM800L_IP5306_VERSION_20190610 (v1.3) pins definition
#define MODEM_UART_BAUD 9600
#define MODEM_RST 5
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26
#define I2C_SDA 21
#define I2C_SCL 22
#define LED_PIN 25
#define IP5306_ADDR 0x75
#define IP5306_REG_SYS_CTL0 0x00

// Set serial for debug console (to the Serial Monitor)
#define SerialMon Serial
// Set serial for AT commands (to the SIM800 module)
#define SerialAT Serial2

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800   // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// Include after TinyGSM definitions
#include <TinyGsmClient.h>

// Your GPRS credentials (leave empty, if missing)
const char apn[] = "zonginternet";       // Your APN
const char gprs_user[] = ""; // User
const char gprs_pass[] = ""; // Password
const char simPIN[] = "";    // SIM card PIN code, if any

const char hostname[] = "attendance-8vkd.onrender.com";
int port = 443;

// Layers stack
TinyGsm sim_modem(SerialAT);
TinyGsmClient gsm_transpor_layer(sim_modem);
SSLClient secure_presentation_layer(&gsm_transpor_layer);
HttpClient http_client = HttpClient(secure_presentation_layer, hostname, port);


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1); // change Serial1 pins in HardwareSerial.cpp file
int fingerprintID = 0;


StaticJsonBuffer<300> jsonBuffer;

const int buzzer = 32;

void setup(){
  pinMode(buzzer, OUTPUT);
  //Fingerprint sensor module setup
  Serial.begin(9600);
  Serial2.begin(115200);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(100);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  }
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  setupSim();
  int status = initializeSim();
  buzzer_tone_double(buzzer);
}

void loop(){
  fingerprintID = detect_finger();
  delay(1500);
  
  // Serial.print("ID: ");
  // Serial.println(fingerprintID);
  
  if (fingerprintID > 0){
    buzzer_tone_single(buzzer);
    Serial.print("ID: ");
    Serial.println(fingerprintID);
    mark_attendance(fingerprintID);
  }
}
