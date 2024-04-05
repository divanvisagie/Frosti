#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// MQTT Server
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);

#define NUMPIXELS        1
#define PIN_NEOPIXEL     39
#define NEOPIXEL_POWER   38

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
     // set color red if we cant connect to wifi
    pixels.fill(0xFF0000);
    pixels.show();
    delay(500);
    Serial.print(".");
  }

  pixels.fill(0xFFDD00); //yellow
  pixels.show();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");

      // set color to green if can connect to mqtt
      pixels.fill(0x00FF00);
      pixels.show();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // set color to yellow if cant connect to mqtt
      pixels.fill(0xFFDD00); //yellow
      pixels.show();
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  pixels.setBrightness(10); // not so bright

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // set color
  pixels.fill(0x990077);
  pixels.show();
  client.publish("frosti/neopixel", "ON");
  delay(500); // wait half a second

  // turn off
  pixels.fill(0x000000);
  pixels.show();
  client.publish("frosti/neopixel", "OFF");
  delay(500); // wait half a second
}
