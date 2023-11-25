#ifndef thermalprinter_h
#define thermalprinter_h

#include "Arduino.h"

#include "esphome/core/component.h"

#include "I2S/src/I2S.h"
// #include "WiFiClientSecure/src/WiFiClientSecure.h"
#include "HTTPClient/src/HTTPClient.h"
#include "FS/src/FS.h"
#include "SPI/src/SPI.h"
#include "SD/src/SD.h"

namespace esphome{
    namespace handScanner{
        class JPGVoice : public Component {
            public:
            JPGVoice();
            void listDir(const char *dirname, uint8_t levels);
            void uploadFile();
            void generate_wav_header(uint8_t *wav_header, uint32_t wav_size, uint32_t sample_rate);
            void recordAndUpload(void *arg);
            void startRecordAndUploadTask();
        }
    }
}
#endif