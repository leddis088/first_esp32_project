#include <Arduino.h>
#include "pitches.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define BUZZZER_PIN  18

const char* ssid = "";
const char* password = "";

const char* apiToken = "";
const char* userToken = "";
const char* pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";
const char *PUSHOVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
                  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
                  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
                  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
                  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
                  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
                  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
                  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
                  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
                  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
                  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
                  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
                  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
                  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
                  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
                  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
                  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
                  "MrY=\n"
                  "-----END CERTIFICATE-----\n";

const String discord_webhook = "";
const String discord_tts = "false";
const char* DISCORD_CERT = R"(
-----BEGIN CERTIFICATE-----
MIIDzTCCArWgAwIBAgIQCjeHZF5ftIwiTv0b7RQMPDANBgkqhkiG9w0BAQsFADBa
MQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJl
clRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTIw
MDEyNzEyNDgwOFoXDTI0MTIzMTIzNTk1OVowSjELMAkGA1UEBhMCVVMxGTAXBgNV
BAoTEENsb3VkZmxhcmUsIEluYy4xIDAeBgNVBAMTF0Nsb3VkZmxhcmUgSW5jIEVD
QyBDQS0zMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEua1NZpkUC0bsH4HRKlAe
nQMVLzQSfS2WuIg4m4Vfj7+7Te9hRsTJc9QkT+DuHM5ss1FxL2ruTAUJd9NyYqSb
16OCAWgwggFkMB0GA1UdDgQWBBSlzjfq67B1DpRniLRF+tkkEIeWHzAfBgNVHSME
GDAWgBTlnVkwgkdYzKz6CFQ2hns6tQRN8DAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0l
BBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwNAYI
KwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j
b20wOgYDVR0fBDMwMTAvoC2gK4YpaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL09t
bmlyb290MjAyNS5jcmwwbQYDVR0gBGYwZDA3BglghkgBhv1sAQEwKjAoBggrBgEF
BQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzALBglghkgBhv1sAQIw
CAYGZ4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEB
AAUkHd0bsCrrmNaF4zlNXmtXnYJX/OvoMaJXkGUFvhZEOFp3ArnPEELG4ZKk40Un
+ABHLGioVplTVI+tnkDB0A+21w0LOEhsUCxJkAZbZB2LzEgwLt4I4ptJIsCSDBFe
lpKU1fwg3FZs5ZKTv3ocwDfjhUkV+ivhdDkYD7fa86JXWGBPzI6UAPxGezQxPk1H
goE6y/SJXQ7vTQ1unBuCJN0yJV0ReFEQPaA1IwQvZW+cwdFD19Ae8zFnWSfda9J1
CZMRJCQUzym+5iPDuI9yP+kHyCREU3qzuWFloUwOxkgAyXVjBYdwRVKD05WdRerw
6DEdfgkfCv4+3ao8XnTSrLE=
-----END CERTIFICATE-----
)";

const int buttonPin = 23;
const int swPinOn = 25;
const int testPin = 15;
const int redLED = 2;
const int yellowLED = 4;
const int greenLED = 0;
int swOnOff = 0;
int led = 0;
int alarmCounter = 0;
bool triggered = false;
int counter = 1;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3
};

int noteDurations[] = {
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

int pauseBetweenNotes = 0;

LiquidCrystal_I2C lcd (0x27, 20, 4);
void setup() {
  lcd.init();
  lcd.backlight();
  Wire.begin();
  Serial.begin(115200);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(swPinOn, INPUT_PULLUP);
  pinMode(testPin, INPUT_PULLUP);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Connecting to WiFi..");
    Serial.println("Connecting to WiFi...");
    sendDiscordMessage("Connecting to WiFi...");
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Connected to WiFi");
  Serial.println("Connected to WiFi");
  sendDiscordMessage("Connected to WiFi");
  delay(1000);
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("TEST BEGIN");
  Serial.println("TEST BEGIN");
  sendDiscordMessage("TEST BEGIN");
  delay(750);
  lcd.clear();
  lcd.setCursor(8,1);
  lcd.print("TEST");
  lcd.setCursor(8,2);
  lcd.print("BEEP");
  Serial.println("BEEP");
  sendDiscordMessage("BEEP");
  for (int thisNote = 0; thisNote < 1; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);
    Serial.println("PLAYING TONE");
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZZER_PIN);
  }
  delay(100);
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(8,1);
    lcd.print("TEST");
    lcd.setCursor(0,2);
    lcd.print("SENDING PUSH MESSEGE");
    StaticJsonDocument<512> notification; 
      notification["token"] = apiToken; //required
      notification["user"] = userToken; //required
      notification["message"] = "Hello from the emergency stop"; //required
      notification["title"] = "ESP32 TEST NOTIFICATION"; //optional
      notification["priority"] = "0"; //optional
      notification["sound"] = "cosmic"; //optional
    String jsonStringNotification;
    Serial.println("message saved to JSON");
    serializeJson(notification, jsonStringNotification);
    HTTPClient https;
    https.begin(pushoverApiEndpoint);
    https.addHeader("Content-Type", "application/json");
    int httpResponseCode = https.POST(jsonStringNotification);
    if (httpResponseCode > 0) {
        Serial.printf("HTTP response code: %d\n", httpResponseCode);
        String response = https.getString();
        Serial.println("Response:");
        Serial.println(response);
      } else {
        Serial.printf("HTTP response code: %d\n", httpResponseCode);
      }
    https.end();
  }
  delay(600);
  lcd.clear();
  lcd.setCursor(8,1);
  lcd.print("TEST");
  lcd.setCursor(1,2);
  lcd.print("SENDING DC MESSEGE");
  sendDiscordMessage("Hello from the emergency stop");
  delay(300);
  lcd.clear();
  lcd.setCursor(8,1);
  lcd.print("TEST");
  lcd.setCursor(3,2);
  lcd.print("RED LED BLINK");
  Serial.println("RED LED Blink");
  sendDiscordMessage("RED LED Blink");
  delay(200);
  digitalWrite(2, HIGH);
  Serial.println("RED LED status HIGH");
  delay(200);
  digitalWrite(2, LOW);
  Serial.println("RED LED status LOW");
  delay(500);
  lcd.clear();
  lcd.setCursor(8,1);
  lcd.print("TEST");
  lcd.setCursor(1,2);
  lcd.print("YELLOW LED BLINK");
  sendDiscordMessage("YELLOW LED BLINK");
  Serial.println("YELLOW LED BLINK");
  delay(200);
  digitalWrite(yellowLED, HIGH);
  Serial.println("YELLOW LED status HIGH");
  delay(200);
  digitalWrite(yellowLED, LOW);
  Serial.println("YELLOW LED status LOW");
  delay(500);
  lcd.clear();
  lcd.setCursor(8,1);
  lcd.print("TEST");
  lcd.setCursor(2,2);
  lcd.print("GREEN LED BLINK");
  Serial.println("GREEN LED BLINK");
  sendDiscordMessage("GREEN LED BLINK");
  delay(200);
  digitalWrite(greenLED, HIGH);
  Serial.println("GREEN LED status HIGH");
  delay(200);
  digitalWrite(greenLED, LOW);
  Serial.println("GREEN LED status HIGH");
  delay(500);
  lcd.clear();
  lcd.setCursor(6,1);
  lcd.print("TEST END");
  Serial.println("TEST END");
  sendDiscordMessage("TEST END");
  delay(750);
  if (digitalRead(swPinOn) == LOW) {
    startup();
  } else {
    swOnOff = 2;
  }
  delay(2000);
}

void loop() {
  if (digitalRead(swPinOn) == LOW) {
    if (swOnOff == 0) {
      startup();
    }
    if (digitalRead(buttonPin) == HIGH) {
      delay(50);
      alarm();
    } else {
      if (counter == 0) {
        while (led < 4){
          delay(200);
          digitalWrite(redLED, HIGH);
          Serial.println("RED LED status HIGH");
          digitalWrite(yellowLED, HIGH);
          Serial.println("YELLOW LED status HIGH");
          digitalWrite(greenLED, HIGH);
          Serial.println("GREEN LED status HIGH");
          delay(200);
          digitalWrite(redLED, LOW);
          Serial.println("RED LED status LOW");
          digitalWrite(yellowLED, LOW);
          Serial.println("YELLOW LED status LOW");
          digitalWrite(greenLED, LOW);
          Serial.println("GREEN LED status LOW");
          led ++;
        }
        led = 0;
        digitalWrite(redLED, LOW);
        Serial.println("RED LED status LOW");
        digitalWrite(greenLED, HIGH);
        Serial.println("GREEN LED status HIGH");
        lcd.clear();
        lcd.print("ALARM OS v.0.5");
        lcd.setCursor(0,1);
        lcd.print("ALARM HAS BEEN RESET");
        Serial.println("ALARM HAS BEEN RESET");
        sendDiscordMessage("ALARM HAS BEEN RESET");
        if (WiFi.status() == WL_CONNECTED) {
          StaticJsonDocument<512> notification; 
          notification["token"] = apiToken; //required
          notification["user"] = userToken; //required
          notification["message"] = "The Alarm is reseted!"; //required
          notification["title"] = "RESET"; //optional
          String jsonStringNotification;
          Serial.println("message saved to JSON");
          serializeJson(notification, jsonStringNotification);
          HTTPClient https;
          https.begin(pushoverApiEndpoint);
          https.addHeader("Content-Type", "application/json");
          int httpResponseCode = https.POST(jsonStringNotification);
          if (httpResponseCode > 0) {
            Serial.printf("HTTP response code: %d\n", httpResponseCode);
            String response = https.getString();
            Serial.println("Response:");
            Serial.println(response);
          } else {
            Serial.printf("HTTP response code: %d\n", httpResponseCode);
          }
          https.end();
        }
        counter ++;
        alarmCounter = 0;
        delay(3000);
      }
      if (alarmCounter != 0) {
        alarmCounter = 0;
      }
      if (digitalRead(yellowLED) == HIGH) {
        digitalWrite(yellowLED, LOW);
        Serial.println("YELLOW LED status LOW");
      }
      lcd.clear();
      lcd.print("ALARM OS v.0.5");
      lcd.setCursor(1,1);
      lcd.print("SYSTEM IS IDLEING");
      Serial.println("System is idleing");
      sendDiscordMessage("System is idleing");
      if (digitalRead(testPin) == HIGH) {
        delay(150); 
        if (digitalRead(testPin) == HIGH) {
          lcd.clear();
          lcd.print("ALARM OS v.0.5");
          lcd.setCursor(4,1);
          lcd.print("TEST ALRM");
          Serial.println("TEST ALARM");
          sendDiscordMessage("TEST ALARM");
          if (WiFi.status() == WL_CONNECTED) {
            StaticJsonDocument<512> notification; 
            notification["token"] = apiToken; //required
            notification["user"] = userToken; //required
            notification["message"] = "TEST ALARM MESSAGE"; //required
            notification["title"] = "TEST ALARM"; //optional
            String jsonStringNotification;
            Serial.println("message saved to JSON");
            serializeJson(notification, jsonStringNotification);
            HTTPClient https;
            https.begin(pushoverApiEndpoint);
            https.addHeader("Content-Type", "application/json");
            int httpResponseCode = https.POST(jsonStringNotification);
            if (httpResponseCode > 0) {
              Serial.printf("HTTP response code: %d\n", httpResponseCode);
              String response = https.getString();
              Serial.println("Response:");
              Serial.println(response);
            }else {
              Serial.printf("HTTP response code: %d\n", httpResponseCode);
            }
            https.end();
          }
          for (int thisNote = 0; thisNote < 14; thisNote++) {
            int noteDuration = 1000 / noteDurations[thisNote];
            tone(BUZZZER_PIN, melody[thisNote], noteDuration);
            Serial.println("PLAYING TONE");
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            noTone(BUZZZER_PIN);
          }
        }
      }
      delay(500);
    }
  } else {
    if (swOnOff == 1) {
      lcd.clear();
      lcd.print("SHOUTTING DOWN OS...");
      Serial.println("SHOUTTING DOWN OS...");
      sendDiscordMessage("SHOUTTING DOWN OS...");
      delay(2100);
      lcd.clear();
      lcd.print("SHOUTTED DOWN OS");
      Serial.println("SHOUTTED DOWN OS");
      sendDiscordMessage("SHOUTTED DOWN OS");
      delay(750);
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.print("ALARM IS NOT READY");
      Serial.println("ALARM IS NOT READY");
      sendDiscordMessage("ALARM IS NOT READY");
      digitalWrite(redLED, LOW);
      Serial.println("RED LED status LOW");
      digitalWrite(yellowLED, LOW);
      Serial.println("YELLOW LED status LOW");
      digitalWrite(greenLED, LOW);
      Serial.println("GREEN LED status LOW");
      swOnOff = 0;
      alarmCounter = 0;
      if (WiFi.status() == WL_CONNECTED) {
          StaticJsonDocument<512> notification; 
          notification["token"] = apiToken; //required
          notification["user"] = userToken; //required
          notification["message"] = "The Alarm has been turned off!"; //required
          notification["title"] = "SHOUTDOWN"; //optional
          String jsonStringNotification;
          Serial.println("message saved to JSON");
          serializeJson(notification, jsonStringNotification);
          HTTPClient https;
          https.begin(pushoverApiEndpoint);
          https.addHeader("Content-Type", "application/json");
          int httpResponseCode = https.POST(jsonStringNotification);
          if (httpResponseCode > 0) {
            Serial.printf("HTTP response code: %d\n", httpResponseCode);
            String response = https.getString();
            Serial.println("Response:");
            Serial.println(response);
          } else {
            Serial.printf("HTTP response code: %d\n", httpResponseCode);
          }
          https.end();
        }
    } else if (swOnOff == 2) {
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.print("ALARM IS NOT READY");
      Serial.println("ALARM IS NOT READY");
      sendDiscordMessage("ALARM IS NOT READY");
      swOnOff = 0;
    }
  }
}

/**
 * Code for startup
 */
void startup() {
  lcd.clear();
  lcd.print("LOADING OS...");
  Serial.println("LOADING OS...");
  sendDiscordMessage("LOADING OS...");
  delay(1500);
  lcd.setCursor(0,1);
  lcd.print("ALARM OS v.0.5");
  Serial.println("ALARM OS v.0.5");
  sendDiscordMessage("ALARM OS v.0.5");
  delay(700);
  lcd.setCursor(0,2);
  lcd.print("LOADING SOUNDFILES..");
  Serial.println("LOADING SOUNDFILES..");
  sendDiscordMessage("LOADING SOUNDFILES..");
  delay(1500);
  lcd.clear();
  lcd.print("LOADING OS...");
  lcd.setCursor(0,1);
  lcd.print("ALARM OS v.0.5");
  lcd.setCursor(0,2);
  delay(1500);
  lcd.print("SOUNDFILES LOADED");
  Serial.println("SOUNDFILES LOADED");
  sendDiscordMessage("SOUNDFILES LOADED");
  delay(750);
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("ALARM IS READY");
  Serial.println("ALARM IS READY");
  sendDiscordMessage("ALARM IS READY");
  digitalWrite(greenLED, HIGH);
  Serial.println("GREEN LED status HIGH");
  swOnOff = 1;
  if (WiFi.status() == WL_CONNECTED) {
    StaticJsonDocument<512> notification; 
    notification["token"] = apiToken; //required
    notification["user"] = userToken; //required
    notification["message"] = "The Alarm has started and is ready"; //required
    notification["title"] = "STARTUP"; //optional
    String jsonStringNotification;
    Serial.println("message saved to JSON");
    serializeJson(notification, jsonStringNotification);
    HTTPClient https;
    https.begin(pushoverApiEndpoint);
    https.addHeader("Content-Type", "application/json");
    int httpResponseCode = https.POST(jsonStringNotification);
    if (httpResponseCode > 0) {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
      String response = https.getString();
      Serial.println("Response:");
      Serial.println(response);
    } else {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
    }
    https.end();
  }
  delay(700);
}

/**
 * Code for the alarm processing
 */
void alarm() {
  alarmCounter++;
  if (alarmCounter == 1) {
    lcd.clear();
    lcd.print("ALARM OS v.0.5");
    lcd.setCursor(6,1);
    lcd.print("WARNING");
    Serial.println("WARNING");
    sendDiscordMessage("WARNING");
    if (WiFi.status() == WL_CONNECTED) {
      StaticJsonDocument<512> notification; 
      notification["token"] = apiToken; //required
      notification["user"] = userToken; //required
      notification["message"] = "Someone has activated the emergency shoutdown"; //required
      notification["title"] = "FIRST WARNING"; //optional
      notification["priority"] = "1"; //optional
      notification["sound"] = "siren"; //optional
      String jsonStringNotification;
      Serial.println("message saved to JSON");
      serializeJson(notification, jsonStringNotification);
      HTTPClient https;
      https.begin(pushoverApiEndpoint);
      https.addHeader("Content-Type", "application/json");
      int httpResponseCode = https.POST(jsonStringNotification);
      if (httpResponseCode > 0) {
        Serial.printf("HTTP response code: %d\n", httpResponseCode);
        String response = https.getString();
        Serial.println("Response:");
        Serial.println(response);
      } else {
        Serial.printf("HTTP response code: %d\n", httpResponseCode);
      }
      https.end();
    }
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    delay(10000);
  } else if (alarmCounter == 2) {
    lcd.clear();
    lcd.print("ALARM OS v.0.5");
    lcd.setCursor(3,1);
    lcd.print("SECOND WARNING");
    Serial.println("SECOND WARNING!");
    sendDiscordMessage("SECOND WARNING");
    if (WiFi.status() == WL_CONNECTED) {
      StaticJsonDocument<512> notification; 
      notification["token"] = apiToken; //required
      notification["user"] = userToken; //required
      notification["message"] = "You have 20 seconds to disable the emergency shoutdown!"; //required
      notification["title"] = "SECOND WARNING"; //optional
      notification["priority"] = "1"; //optional
      notification["sound"] = "siren"; //optional
      String jsonStringNotification;
      Serial.println("message saved to JSON");
      serializeJson(notification, jsonStringNotification);
      HTTPClient https;
      https.begin(pushoverApiEndpoint);
      https.addHeader("Content-Type", "application/json");
      int httpResponseCode = https.POST(jsonStringNotification);
      if (httpResponseCode > 0) {
        Serial.printf("HTTP response code: %d\n", httpResponseCode);
        String response = https.getString();
        Serial.println("Response:");
        Serial.println(response);
      } else {
        Serial.printf("HTTP response code: %d\n", httpResponseCode);
      }
      https.end();
    }
    while (led < 4) {
      delay(200);
      digitalWrite(redLED, HIGH);
      delay(200);
      digitalWrite(redLED, LOW);
      led ++;
    }
    led = 0;
    delay(20000);
  } else {
    Serial.println("ALARM!");
    if (alarmCounter == 3) {
      counter = 0;
      if (WiFi.status() == WL_CONNECTED) {
        StaticJsonDocument<512> notification; 
        notification["token"] = apiToken; //required
        notification["user"] = userToken; //required
        notification["message"] = "The Alarm has started!"; //required
        notification["title"] = "ALARM"; //optional
        notification["priority"] = "1"; //optional
        notification["sound"] = "siren"; //optional
        String jsonStringNotification;
        Serial.println("message saved to JSON");
        serializeJson(notification, jsonStringNotification);
        HTTPClient https;
        https.begin(pushoverApiEndpoint);
        https.addHeader("Content-Type", "application/json");
        int httpResponseCode = https.POST(jsonStringNotification);
        if (httpResponseCode > 0) {
          Serial.printf("HTTP response code: %d\n", httpResponseCode);
          String response = https.getString();
          Serial.println("Response:");
          Serial.println(response);
        } else {
          Serial.printf("HTTP response code: %d\n", httpResponseCode);
        }
      https.end();
      }
      digitalWrite(yellowLED, LOW);
      digitalWrite(redLED, HIGH);
      lcd.clear();
      lcd.print("ALARM OS v.0.5");
      lcd.setCursor(7,1);
      lcd.print("ALARM");
      sendDiscordMessage("ALARM");
    }
    for (int thisNote = 0; thisNote < 14; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(BUZZZER_PIN, melody[thisNote], noteDuration);
      Serial.println("PLAYING TONE");
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(BUZZZER_PIN);
    }
  }
}

/**
 * Discord integration
 */
 void sendDiscord(String content, String embedJson) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if (client) {
    client -> setCACert(DISCORD_CERT);
    {
      HTTPClient https;
      if (https.begin(*client, discord_webhook)) {
        https.addHeader("Content-Type", "application/json");
        String jsonPayload = "{\"content\":\"" + content + "\",\"tts\":" + discord_tts + ",\"embeds\": [" + embedJson + "]}";
        int httpCode = https.POST(jsonPayload);
        if (httpCode > 0) {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.print("HTTP Response: ");
            Serial.println(payload);
          }
        } else {
          Serial.print("HTTP Post failed: ");
          Serial.println(https.errorToString(httpCode).c_str());
        }
        https.end();
      }
    }
    delete client;
  }
}
void sendDiscordMessage(String content) {
  sendDiscord(content, "");
}
void sendDiscordEmbeds(String embeds) {
  sendDiscord("", embeds);
}
