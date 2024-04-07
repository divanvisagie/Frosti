#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_BME680.h"

#define NUMPIXELS        1
#define PIN_NEOPIXEL     39 //GPIO 39
#define NEOPIXEL_POWER   38 //GPIO 38

#define BME_SCL          18 //GPIO 18
#define BME_SDA          17 //GPIO 17


// WiFi credentials
const char* ssid = "";
const char* password = "";

// MQTT Server
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);

TwoWire bmeWire = TwoWire(0);
Adafruit_BME680 bme(&bmeWire);

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

#define SEALEVELPRESSURE_HPA (1013.25)

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
  
  // BME Sensor setup
  bmeWire.begin(BME_SDA, BME_SCL);
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  } 
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

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

  // Section for readings
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  // Temperature in C
  client.publish("frosti/temperature", String(bme.temperature).c_str());

  // Pressure in hPa
  float pressure = bme.pressure / 100.0;
  client.publish("frosti/pressure", String(pressure).c_str());

  // Humidity in %
  float humidity = bme.humidity;
  client.publish("frosti/humidity", String(humidity).c_str());

  // Gas in Kohms
  float gas = bme.gas_resistance / 1000.0;
  client.publish("frosti/gas", String(gas).c_str());

  // Altitude in meters
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  client.publish("frosti/altitude", String(altitude).c_str());

  delay(1000); 
  // set pixel to black after first run
  pixels.fill(0x000000);
  pixels.show();
}
