
/**************************************************************
 *
 * ESP32 LilyGo-T-Call-SIM800 Example
 *
 * HTTPS (TLS/SLL) with CA Certificate via "TinyGsm.h": https://github.com/vshymanskyy/TinyGSM
 * Tested on SIM800L_IP5306_VERSION_20190610 (v1.3) (R14.18)
 *
 * About board: https://github.com/Xinyuan-LilyGO/LilyGo-T-Call-SIM800
 * About SIM800L_IP5306 v1.3: https://github.com/Xinyuan-LilyGO/LilyGo-T-Call-SIM800/blob/master/doc/SIM800L_IP5306.MD    
 * Base example: https://github.com/Xinyuan-LilyGO/LilyGo-T-Call-SIM800/tree/master/examples/Arduino_TinyGSM  
 * 
 **************************************************************/

#include <Wire.h>

#include <SSLClient.h>
#include <ssl_client.h>

#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

//To make http request esay: https://github.com/arduino-libraries/ArduinoHttpClient
#include <ArduinoHttpClient.h>

//Please enter your CA certificate in ca_cert.h
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
#define SerialAT Serial1

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

void setup()
{
  SerialMon.begin(9600);
  delay(100);

  // Start board power management
  if (!setupPMU())
  {
    Serial.println("Setting board power management error");
  }

  // Set SIM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  //Add CA Certificate
  secure_presentation_layer.setCACert(root_ca);

  // SIM modem initial setup
  setupModem();

}

void loop()
{
  // initialize json buffer to store response from API
  StaticJsonBuffer<300> jsonBuffer;

  SerialMon.print("Initializing modem...");
  if (!sim_modem.init())
  {
    SerialMon.print(" fail... restarting modem...");
    setupModem();
    // Restart takes quite some time
    // Use modem.init() if you don't need the complete restart
    if (!sim_modem.restart())
    {
      SerialMon.println(" fail... even after restart");
      return;
    }
  }
  SerialMon.println(" OK");

  secure_presentation_layer.setCACert(root_ca);

  // General information
  String name = sim_modem.getModemName();
  Serial.println("Modem Name: " + name);
  String modem_info = sim_modem.getModemInfo();
  Serial.println("Modem Info: " + modem_info);

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && sim_modem.getSimStatus() != 3)
  {
    sim_modem.simUnlock(simPIN);
  }

  // Wait for network availability
  SerialMon.print("Waiting for network...");
  if (!sim_modem.waitForNetwork(240000L))
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");
  
  // Connect to the GPRS network
  SerialMon.print("Connecting to network...");
  if (!sim_modem.isNetworkConnected())
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  // Connect to APN
  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);
  if (!sim_modem.gprsConnect(apn, gprs_user, gprs_pass))
  {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  digitalWrite(LED_PIN, HIGH);
  SerialMon.println(" OK");

  // More info..
  Serial.println("");
  String ccid = sim_modem.getSimCCID();
  Serial.println("CCID: " + ccid);
  String imei = sim_modem.getIMEI();
  Serial.println("IMEI: " + imei);
  String cop = sim_modem.getOperator();
  Serial.println("Operator: " + cop);
  IPAddress local = sim_modem.localIP();
  Serial.println("Local IP: " + String(local));
  int csq = sim_modem.getSignalQuality();
  Serial.println("Signal quality: " + String(csq));

  /// HTTP Test
  if (sim_modem.isGprsConnected())
  {
    Serial.println("");
    Serial.println("Making GET request");
    http_client.get("/mark_attendance/?id=6");

    int status_code = http_client.responseStatusCode();
    String response = http_client.responseBody();

    Serial.print("Status code: ");
    Serial.println(status_code);
    Serial.print("Response: ");
    Serial.println(response);

    JsonObject& json_response = jsonBuffer.parseObject(response);

    if(!json_response.success()) {
      Serial.println("parseObject() failed");
    }
    else{
      String message = json_response["message"];
      Serial.print("Json Response message: ");
      Serial.println(message);

      String numbers[2] = json_response["numbers"];
      Serial.print("Json Response numbers: ");
      Serial.println(numbers[0]);
      Serial.println(numbers[1]);

      
    }

    http_client.stop();
  }
  else
  {
    Serial.println("...not connected");
  }

  // Disconnect GPRS
  sim_modem.gprsDisconnect();
  SerialMon.println("GPRS disconnected");
  digitalWrite(LED_PIN, LOW);

  //Turn off the moden (if use, you need run setupModem() again)
  //sim_modem.poweroff();
  //SerialMon.println("Modem poweroff");
  //delay(1000);
  //setupModem();

  delay(15000);
}
