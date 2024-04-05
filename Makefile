# Makefile for uploading code to ESP32

# Set the path to your Arduino CLI or IDE
ARDUINO_CLI = arduino-cli
# Set the path to your ESP32 board specification
BOARD = esp32:esp32:adafruit_qtpy_esp32s2
# Define your serial port
PORT = /dev/ttyACM0
# Define the sketch directory
SKETCH = ./Frosti.ino

# Optionally set WiFi credentials as environment variables
# WIFI_SSID and WIFI_PASS need to be set in your environment or Makefile

# Upload command
upload:
	@echo "Uploading to ESP32..."
	$(ARDUINO_CLI) compile --fqbn $(BOARD) $(SKETCH)
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(BOARD) $(SKETCH)
	@echo "Upload complete."

# Monitor serial output
listen:
	@echo "Listening on $(PORT)..."
	@screen $(PORT) 115200

.PHONY: upload listen
