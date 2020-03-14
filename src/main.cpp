#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#define SerialAT Serial1

// TTGO T-Call pins
#define MODEM_RST 5
#define MODEM_PWKEY 4
#define MODEM_POWER_ON 23
#define MODEM_TX 27
#define MODEM_RX 26
#define I2C_SDA 21
#define I2C_SCL 22

const char apn[] = "";      // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password
const char simPIN[] = "";   // Pin de SIM

const char server[] = "jsonplaceholder.typicode.com"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/todos/1";                   // resource path, for example: /post-data.php
const int port = 80;

const uint32_t rate = 0;
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);

//GPS VARS
HardwareSerial SerialGPS(1);
static const int RXD2 = 16, TXD2 = 17;

TinyGPSPlus gps;

void requestApiLocation()
{
  Serial.print("Connecting to APN");
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    Serial.println("Fail");
  }
  else
  {
    // Making an HTTP POST request
    Serial.println("GPRS connected");

    Serial.print(F("Starting web client"));
    http.connectionKeepAlive(); // Currently, this is needed for HTTPS
    int err = http.get(resource);
    if (err != 0)
    {
      Serial.println(F("Failed to connect"));
      delay(10000);
      return;
    }

    int status = http.responseStatusCode();
    Serial.print(F("Response status code: "));
    Serial.println(status);
    if (!status)
    {
      delay(10000);
      return;
    }

    Serial.println(F("Response Headers:"));
    while (http.headerAvailable())
    {
      String headerName = http.readHeaderName();
      String headerValue = http.readHeaderValue();
      Serial.println("    " + headerName + " : " + headerValue);
    }

    int length = http.contentLength();
    if (length >= 0)
    {
      Serial.print(F("Content length is: "));
      Serial.println(length);
    }
    if (http.isResponseChunked())
    {
      Serial.println(F("The response is chunked"));
    }

    String body = http.responseBody();
    Serial.println(F("Response:"));
    Serial.println(body);

    Serial.print(F("Body length is: "));
    Serial.println(body.length());

    // Shutdown
    http.stop();
    Serial.println(F("Web client shutdown"));

    modem.gprsDisconnect();
    Serial.println(F("GPRS disconnected"));
  }
}


void getGPSData()
{
  while (SerialGPS.available() > 0)
  {
    gps.encode(SerialGPS.read());
  }

  Serial.print("LAT=");
  Serial.println(gps.location.lat(), 6);
  Serial.print("LONG=");
  Serial.println(gps.location.lng(), 6);
  Serial.print("ALT=");
  Serial.println(gps.altitude.meters());
}

/* Metodos de Arduino */
void setup()
{
  Serial.begin(115200);
  Serial.println("Setup starting...");
  SerialGPS.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(5000);

  /* // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  // use modem.init() if you don't need the complete restart

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info");
  Serial.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3)
  {
    modem.simUnlock(simPIN);
  }

  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork())
  {
    Serial.println("Fail");
    delay(10000);
    return;
  }
  Serial.println("Network success");

  if (modem.isNetworkConnected())
  {
    Serial.println("Network connected");
  }*/
}

void loop()
{
  //requestApiLocation();
  getGPSData();
  Serial.println(SerialGPS.read());
  delay(5000);
}