#include "ESP8266WiFi.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WIFI Setup *********************************/
#define ssid        "MAUI"
#define password    "@dm!nm3a1x8@"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883  // use 8883 for SSL
#define AIO_USERNAME    "brunofranrodrigues"
#define AIO_KEY         "dc4c5949443d4f8285b6ce46d2fdedc0"



/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// MQTT for Publish
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

Adafruit_MQTT_Publish ph = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ph");

Adafruit_MQTT_Publish ec = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ec");

// MQTT for Subscribe
Adafruit_MQTT_Subscribe tank = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/tank");

Adafruit_MQTT_Subscribe irrigation = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/irrigation");

float phValue = 0.0;
float ecValue = 0.0;
float tempValue = 0.0;

void MQTT_connect();

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

boolean newData = false;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("<ESP8266 is ready>");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  delay(500);
  Serial.println("");
  delay(5000);
  Serial.println("WiFi connection Successful");
  delay(5000);
  Serial.println("The IP Address of ESP8266 Module is: ");
  delay(5000);
  Serial.println(WiFi.localIP());// Print the IP address
  delay(500);

  tank.setCallback(tankcallback);
  irrigation.setCallback(irrigationcallback);

  // Setup MQTT subscription for time feed.
  mqtt.subscribe(&tank);
  mqtt.subscribe(&irrigation);

}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.println("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void loop()
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();


  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(10000);

  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
  }
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ","); // this continues where the previous call left off
  phValue = atof(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  ecValue = atof(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  tempValue = atof(strtokIndx);     // convert this part to a float

}

void showParsedData() {
  //Serial.println("Valores recebidos:");
  //Serial.println(phValue);
  //Serial.println(ecValue);
  //Serial.println(tempValue);
  delay(60000);
  ph.publish(phValue);
  delay(60000);
  ec.publish(ecValue);
  delay(60000);
  temp.publish(tempValue);
}

void tankcallback(char *data, uint16_t len) {
  Serial.print("#");
  Serial.print("tank:");
  Serial.print(data);
  Serial.print("#");
}

void irrigationcallback(char *data, uint16_t len) {
  Serial.print("#");
  Serial.print("irrigation:");
  Serial.print(data);
  Serial.print("#");
}
