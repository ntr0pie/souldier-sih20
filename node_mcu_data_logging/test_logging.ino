

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/************************* WiFi Access Point *********************************/

#define R_SSID       "not for you"
#define PASS       "devesh12345"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ritwik25"
#define AIO_KEY         "aio_SPNe63vEVsogmtnAhvYiWG8eAJNf"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'temp' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish Latitude = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/lat");

// Setup a feed called 'humidity' for publishing.
Adafruit_MQTT_Publish Longitude = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/longi");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
//void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.setup(4); // data pin 4

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(R_SSID);

  WiFi.begin(R_SSID, PASS);
  int counter=10;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter--;
    if(counter==0){
      // reset me
        ESP.reset();
    }
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  }



void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  int lat = Serial.read();
  int longit = Serial.read();

  // Sending data to the feeds
  Serial.print(F("\nSending lat "));
  Serial.print("...");

  /*
   * Sending temperature data to the feed. The publish function will
   * return a false if not succesful. The if statement gives you
   * the chance to trap an error and respond to it
   */
  if (! Latitude.publish(lat)) {
    Serial.println(F("Latitude Failed"));
  } else {
    Serial.println("Latitude: "+String(lat)+" ");
  }
  Serial.print(F("\nSending longitutde Value "));
 /*
   * Sending Humidity data to the feed. The publish function will
   * return a false if not succesful. The if statement gives you
   * the chance to trap an error and respond to it
   */
  if (! Longitude.publish(longit)) {
    Serial.println(F("longitutde Failed"));
  } else {
    Serial.println("Longitude: "+String(longit)+" ");
  }


  delay(30000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // reset me
        ESP.reset();
       }
  }
  Serial.println("MQTT Connected!");
}
