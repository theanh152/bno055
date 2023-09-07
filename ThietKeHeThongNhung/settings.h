#include <DHT.h>         // DHT for ESP32 library
#include <WiFi.h>           // WiFi control for ESP32
#include <ThingsBoard.h>    // ThingsBoard SDK
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// ESP32 pin used to query DHT11/DHT22
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LCD config
RTC_DS1307 rtc; 
// Create the lcd object address 0x3F and 16 columns x 2 rows 
LiquidCrystal_I2C lcd (0x27, 16,2);  
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Buzzer config
int BUZZER_PIN = 13;
int BUZZER_CHANNEL = 0;

// Alarm config
float temp_warning = 40;
float humid_warning = 90;

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// WiFi access point
#define WIFI_AP_NAME        "An"
// WiFi password
#define WIFI_PASSWORD       "khongbiet"
// See https://thingsboard.io/docs/getting-started-guides/helloworld/ 
// to understand how to obtain an access token
#define TOKEN               "fZCx23mCC77UK3Br20cv"
// ThingsBoard server instance.
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD    115200

// Period of sending a temperature/humidity data.
int send_delay = 2000;

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;


// Array with LEDs that should be lit up one by one
uint8_t leds_cycling[] = { 25, 26, 32 };
// Array with LEDs that should be controlled from ThingsBoard, one by one
uint8_t leds_control[] = { 19, 22, 21 };
// Main application loop delay
int quant = 20;
// Initial period of LED cycling.
int led_delay = 1000;
// Time passed after LED was turned ON, milliseconds.
int led_passed = 0;
// Time passed after temperature/humidity data was sent, milliseconds.
int send_passed = 0;
// Set to true if application is subscribed for the RPC messages.
bool subscribed = false;
// LED number that is currenlty ON.
int current_led = 0;