#include "UbidotsESPMQTT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TOKEN "BBFF-p7mqyFmNxsI8hi4jv9t0WoQ4ulRjfo" // Your Ubidots TOKEN
#define WIFINAME "POCO" // Your SSID
#define WIFIPASS "asdfghjk" // Your Wi-Fi Password

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the I2C address and LCD dimensions (16x2)

#define inSensor D5
#define outSensor D6

#define relay D1

int inStatus;
int outStatus;

int countin = 0;
int countout = 0;

int in;
int out;
int now;

Ubidots client(TOKEN);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  client.setDebug(true); // Pass a true or false bool value to activate debug messages
  Serial.begin(115200); // Use the desired baud rate

  client.wifiConnection(WIFINAME, WIFIPASS);
  client.begin(callback);

  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.init();
  lcd.backlight();
  lcd.print("Visitor");
  lcd.setCursor(0, 1);
  lcd.print("Counter");
  delay(3000);
  lcd.clear();

  pinMode(inSensor, INPUT);
  pinMode(outSensor, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  Serial.println("Visitor Counter Demo");
}

void loop() {
  if (!client.connected()) {
    client.reconnect();
  }

  inStatus = digitalRead(inSensor);
  outStatus = digitalRead(outSensor);
  if (inStatus == 0) {
    in = countin++;
  }

  if (outStatus == 0) {
    out = countout++;
  }

  now = in - out;

  if (now <= 0) {
    digitalWrite(relay, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Visitor");
    lcd.setCursor(0, 1);
    lcd.print("Light Off");
    Serial.println("No Visitors! Light Off");
    delay(500);
  } else {
    digitalWrite(relay, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current Visitor:");
    lcd.setCursor(0, 1);
    lcd.print(now);

    Serial.print("Current Visitor: ");
    Serial.println(now);
    Serial.print("IN: ");
    Serial.println(in);
    Serial.print("OUT: ");
    Serial.println(out);
    delay(500);
  }

  int relaystatus = digitalRead(relay);
  client.add("in", in);
  client.add("out", out);
  client.add("now", now);
  client.add("Light Status", relaystatus);
  client.ubidotsPublish("ESP8266");
  client.loop();
  Serial.println();
  delay(100);
}
