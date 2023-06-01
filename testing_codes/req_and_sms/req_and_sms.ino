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
#define LED_PIN 13
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

// Power configuration for SIM800L_IP5306_VERSION_20190610 (v1.3) board
bool setupPMU()
{
  bool en = true;
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en)
  {
    Wire.write(0x37);
  }
  else
  {
    Wire.write(0x35);
  }
  return Wire.endTransmission() == 0;
}

// Modem initial setup (cold start)
void setupModem()
{
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Reset pin high
  digitalWrite(MODEM_RST, HIGH);

  // Turn on the Modem power first
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Pull down PWRKEY for more than 1 second according to manual requirements
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(200);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1200);
  digitalWrite(MODEM_PWRKEY, HIGH);

  // Initialize the indicator as an output
  digitalWrite(LED_PIN, LOW);
}

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
  SerialAT.begin(19200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  //Add CA Certificate
  secure_presentation_layer.setCACert(root_ca);

  // SIM modem initial setup
  setupModem();

}

void loop()
{
  // initialize json buffer to store response from API
  // StaticJsonBuffer<300> jsonBuffer;

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
    String str_response = http_client.responseBody();

    int str_len = str_response.length() + 1;
    char response[str_len];
    str_response.toCharArray(response, str_len);


    Serial.print("Status code: ");
    Serial.println(status_code);
    Serial.print("Response: ");
    Serial.println(response);


    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    String nums = doc["numbers"];
    Serial.print("String nums: ");
    Serial.println(nums);
    
    const size_t CAPACITY = JSON_ARRAY_SIZE(3);
    StaticJsonDocument<CAPACITY> doc2;
    deserializeJson(doc2, nums);
    // Serial.print("deserialize json: ");
    // Serial.println(doc2);

    JsonArray array = doc2.as<JsonArray>();
    Serial.print("deserialize array: ");
    Serial.println(array);

    long number;
    for(JsonVariant v : array) {
      number = v.as<long>();
      Serial.print(v.as<long>());
      Serial.println(number);
      // bool res = sim_modem.sendSMS(number, message);
    }

    // Serial.println(maze[0]);
    // Serial.println(maze[1]);

    // String message = doc["message"];
    // Serial.println(message);


    // JsonObject& json_response = jsonBuffer.parseObject(response);
    // JsonArray& numbers = jsonBuffer.parseArray(json_response["numbers"]);

    // if(!json_response.success()) {
    //   Serial.println("parseObject() failed");
    // }
    // else{
    //   String message = json_response["message"];
    //   Serial.print("Json Response message: ");
    //   Serial.println(message);

    //   String numbers = json_response["numbers"];
    //   Serial.println(numbers);

    //   char numbers2[26] = {json_response["numbers"]};
    //   Serial.println(numbers2);
      
    //   Serial.println("Testing");

    //   // String numbers[3];
    //   // numbers = &json_response["numbers"];
    //   // // strcpy(numbers, json_response["numbers"]);
    //   // Serial.println(numbers);


    //   int size = sizeof(json_response["numbers"]);
    //   Serial.println(size);

    //   char number1[size] = {json_response["numbers"][0]};
    //   Serial.println(number1);

    //   int size2 = sizeof(number1);
    //   Serial.println(size2);

    //   char numberx[size] = {json_response["numbers"]};
    //   int size3 = sizeof(numberx);
    //   Serial.println(size3);
      // Serial.println(json_response["numbers"]);
      // for (int i=0; i<2; i++){
        // Serial.println(i);
        // Serial.println(json_response["numbers"][i]);
      // }

    //   String numbers[3] = json_response["numbers"];
    //   Serial.println("Json Response numbers: ");
    //   Serial.println(numbers[0]);
    //   Serial.println(numbers[1]);
    //   Serial.println(sizeof(numbers));
    //   Serial.println(sizeof(numbers[0]));
    //   Serial.println(sizeof(numbers[1]));
    //   Serial.println();

    //   String number;
    //   for (int i=0; i<sizeof(numbers)/sizeof(numbers[0]); i++) {
    //     number = numbers[i];
    //     Serial.println(number);
    //     Serial.println(i);        
    //     // bool res = sim_modem.sendSMS(number, message);
    //   }


    // }
    http_client.stop();
  }
  
  else{
    Serial.println("...not connected");
  }

  // Disconnect GPRS
  // sim_modem.gprsDisconnect();
  // SerialMon.println("GPRS disconnected");
  // digitalWrite(LED_PIN, LOW);

  //Turn off the moden (if use, you need run setupModem() again)
  //sim_modem.poweroff();
  //SerialMon.println("Modem poweroff");
  //delay(1000);
  //setupModem();

  delay(3000);
}
