#ifndef thermalprinter_h
#define thermalprinter_h

#include "Arduino.h"

#include "esphome/core/component.h" 

#include <I2S.h>
// #include "WiFiClientSecure/src/WiFiClientSecure.h"
#include <HTTPClient.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>

//Variables to be used in the recording program, do not change for best
#define SAMPLE_RATE 16000U
#define SAMPLE_BITS 16
#define WAV_HEADER_SIZE 44
#define VOLUME_GAIN 2
#define RECORD_TIME 10      // seconds, The maximum value is 240

uint32_t record_size = (SAMPLE_RATE * SAMPLE_BITS / 8) * RECORD_TIME;
namespace esphome{
    namespace handScanner{
        class JPGVoice : public Component {
            public:
            JPGVoice();
            void listDir(const char *dirname, uint8_t levels);
            void uploadFile(const char *filename);
            void generate_wav_header(uint8_t *wav_header, uint32_t wav_size, uint32_t sample_rate);
            void recordAndUpload(void *arg);
            void startRecordAndUploadTask();
            private:
            bool isWIFIConnected = false;
            File fileHandle;
            const char defaultFilename[15] = "/recording.wav";
        };
    }
}
#endif