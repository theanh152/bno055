#include "settings.h"

// Processes function for RPC call "setValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processDelayChange(const RPC_Data &data)
{
  Serial.println("Received the set delay RPC method");

  // Process data

  led_delay = data;

  Serial.print("Set new delay: ");
  Serial.println(led_delay);

  return RPC_Response(NULL, led_delay);
}

// Processes function for RPC call "getValue"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processGetDelay(const RPC_Data &data)
{
  Serial.println("Received the get value method");

  return RPC_Response(NULL, led_delay);
}

// Processes function for RPC call "setGpioStatus"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processSetGpioState(const RPC_Data &data)
{
  Serial.println("Received the set GPIO RPC method");

  int pin = data["pin"];
  bool enabled = data["enabled"];

  if (pin < COUNT_OF(leds_control)) {
    Serial.print("Setting LED ");
    Serial.print(pin);
    Serial.print(" to state ");
    Serial.println(enabled);

    digitalWrite(leds_control[pin], enabled);
  }

  return RPC_Response(data["pin"], (bool)data["enabled"]);
}

// RPC handlers
RPC_Callback callbacks[] = {
  { "setValue",         processDelayChange },
  { "getValue",         processGetDelay },
  { "setGpioStatus",    processSetGpioState },
};

// Setup an application
void setup() {
  // Initialize serial for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  InitWiFi();
  
  // LCD initalize  
  lcd.init();
  // Turn on the backlight on LCD. 
  lcd. backlight ();
  // RTC init
  rtc.begin();
  //auto update from computer time
  rtc.adjust(DateTime(__DATE__, __TIME__));

  // Pinconfig

  for (size_t i = 0; i < COUNT_OF(leds_cycling); ++i) {
    pinMode(leds_cycling[i], OUTPUT);
  }

  for (size_t i = 0; i < COUNT_OF(leds_control); ++i) {
    pinMode(leds_control[i], OUTPUT);
  }

  // Initialize temperature sensor
  dht.begin();

  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);  //Buzzer
}

// Main application loop
void loop() {
  delay(quant);

  led_passed += quant;
  send_passed += quant;

  // Check if next LED should be lit up
  if (led_passed > led_delay) {
    // Turn off current LED
    digitalWrite(leds_cycling[current_led], LOW);
    led_passed = 0;
    current_led = current_led >= 2 ? 0 : (current_led + 1);
    // Turn on next LED in a row
    digitalWrite(leds_cycling[current_led], HIGH);
  }

  // Reconnect to WiFi, if needed
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
    return;
  }

  // Reconnect to ThingsBoard, if needed
  if (!tb.connected()) {
    subscribed = false;

    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  // Subscribe for RPC, if needed
  if (!subscribed) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    // if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
    //   Serial.println("Failed to subscribe for RPC");
    //   return;
    // }

    Serial.println("Subscribe done");
    subscribed = true;
  }

  // Check if it is a time to send DHT22 temperature and humidity
  if (send_passed > send_delay) {
    Serial.println("Sending data...");

    // Uploads new telemetry to ThingsBoard using MQTT. 
    // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api 
    // for more details

    float humidity_value = dht.readHumidity();
    float temperature_value = dht.readTemperature();

    if (isnan(humidity_value) || isnan(temperature_value)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(1000);
    return;
    }
    else{
      tb.sendTelemetryFloat("temperature", temperature_value);
      tb.sendTelemetryFloat("humidity", humidity_value);               
    }

    if ((temperature_value > temp_warning) || (humidity_value > humid_warning))
    {
      buzzerAlarm();
    }    
    
    // ledcWrite(0, 0);    

    LcdDisplay(temperature_value, humidity_value);    
        
    send_passed = 0;
  }

  // Process messages
  tb.loop();
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}