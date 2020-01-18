
#include <ESP8266WiFi.h>

const char* ssid     = "fsociety";
const char* password = "astralisgg";

const char* host = "api.thingspeak.com";

void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.swap();
  delay(1000);

  // We start by connecting to a WiFi network
  Serial1.begin(115200);
  Serial1.println();
  Serial1.println();
  Serial1.print("Connecting to ");
  Serial1.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }

  Serial1.println("");
  Serial1.println("WiFi connected");  
  Serial1.println("IP address: ");
  Serial1.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(5000);

  Serial1.println("reading co2 level");
  const uint8_t request[] = {0xFE, 0x44, 0x00, 0x08, 0x02, 0x9F, 0x25};
  Serial.write(request, sizeof(request));
  uint8_t response[8];
  int t = millis();
  for (int i = 0; i < 7; ++i) {
    if (Serial.available()) {
      response[i] = Serial.read();
    }
    if (millis() - t > 1000) {
      Serial1.println("no response");
      return;
    }
  }

  int co2value = static_cast<int>(response[3]) * 256 + static_cast<int>(response[4]);
  
  Serial1.print("connecting to ");
  Serial1.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial1.println("connection failed");
    return;
  }

  
  // We now create a URI for the request
  String url = "/co2";
  url += "?value=";
  url += co2value;
  
  Serial1.print("Requesting URL: ");
  Serial1.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial1.print(line);
  }
  
  Serial1.println();
  Serial1.println("closing connection");
}
