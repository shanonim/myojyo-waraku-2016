#include <Milkcocoa.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>

#define MILKCOCOA_APP_ID    "hotiv4mwrqx"
#define MILKCOCOA_DATASTORE "otaku_blade"
#define MILKCOCOA_SERVERPORT    1883
#define MAX_VAL 64 // 0 to 255 for brightness

const char* ssid = "diversity";
const char* password = "YdJ2QywuD2jaQFHLHj5PWBYjDRxRP8Lu";
const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;
boolean connectedStatus = false;

WiFiServer server(80);
WiFiClient client;
Milkcocoa milkcocoa = Milkcocoa(&client, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(60, 12, NEO_GRB + NEO_KHZ800);
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
// NEO_RGB     Pixels are wired for RGB bitstream
// NEO_GRB     Pixels are wired for GRB bitstream
// NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
// NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

int network_status_led = 14;
int sw_led = 15;
int sw_button = 16;
int sw_status;
int colorCode = 1;

extern "C" {
    #include "user_interface.h"
}

void setup() {
    pinMode(network_status_led, OUTPUT);
    pinMode(sw_led, OUTPUT);
    pinMode(sw_button, INPUT);
    
    Serial.begin(115200);
    
    // Connect to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    
    // Start the server
    server.begin();
    Serial.println("Server started");
    
    // Print the IP address
    Serial.println(WiFi.localIP());
    digitalWrite(14, HIGH);

    pixel.begin();
    pixel.show(); // Initialize all pixels to 'off'
}

void loop() {
    if (!connectedStatus) {
        milkcocoa.loop();
        DataElement elem = DataElement();
        elem.setValue("status", "connected");
        milkcocoa.push(MILKCOCOA_DATASTORE, &elem); 
        connectedStatus = true;
    }

    // switch status
    sw_status = digitalRead(sw_button);

    if (sw_status == 0) {
        digitalWrite(sw_led, LOW);
    } else {
        digitalWrite(sw_led, HIGH);
        setColor(colorCode);

        colorCode++;
        if (colorCode == 4) {
            colorCode = 1;
        }
    }
}

void setColor(uint16_t colorCode) {
    uint16_t i;
    switch (colorCode) {
        case 1:
            // red
            for (i=0; i<pixel.numPixels(); i++) {
                pixel.setPixelColor(i, pixel.Color(MAX_VAL, 0, 0));
                pixel.show();
            }
            break;
        case 2:
            // green
            for (i=0; i<pixel.numPixels(); i++) {
                pixel.setPixelColor(i, pixel.Color(0, MAX_VAL, 0));
                pixel.show();
            }
            break;
        case 3:
            // blue
            for (i=0; i<pixel.numPixels(); i++) {
                pixel.setPixelColor(i, pixel.Color(0, 0, MAX_VAL));
                pixel.show();
            }
            break;
    }
}

