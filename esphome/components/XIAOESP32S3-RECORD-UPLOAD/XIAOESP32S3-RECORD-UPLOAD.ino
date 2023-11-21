#include <I2S.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "../voice.h"

//Variables to be used in the recording program, do not change for best
#define SAMPLE_RATE 16000U
#define SAMPLE_BITS 16
#define WAV_HEADER_SIZE 44
#define VOLUME_GAIN 2
#define RECORD_TIME 10  // seconds, The maximum value is 240

// Number of bytes required for the recording buffer
uint32_t record_size = (SAMPLE_RATE * SAMPLE_BITS / 8) * RECORD_TIME;

File file;
const char filename[] = "/recording.wav";

// char *ssid = "Apt. #3";
// char *password = "";
bool isWIFIConnected;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ;

  I2S.setAllPins(-1, 42, 41, -1, -1);

  //The transmission mode is PDM_MONO_MODE, which means that PDM (pulse density modulation) mono mode is used for transmission
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, SAMPLE_BITS)) {
    Serial.println("Failed to initialize I2S!");
    while (1)
      ;
  }

  if (!SD.begin(21)) {
    Serial.println("Failed to mount SD Card!");
    while (1)
      ;
  }

  // xTaskCreate(recordAndUpload, "recordAndUpload", 1024 * 8, NULL, 1, NULL);
  // delay(500);
  // xTaskCreate(wifiConnect, "wifi_Connect", 4096, NULL, 0, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}


// void wifiConnect(void *pvParameters) {
//   isWIFIConnected = false;
//   Serial.print("Try to connect to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     vTaskDelay(500);
//     Serial.print(".");
//   }
//   Serial.println("Wi-Fi Connected!");
//   isWIFIConnected = true;
//   while (true) {
//     vTaskDelay(1000);
//   }
}


