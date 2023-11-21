#ifndef thermalprinter_h
#define thermalprinter_h

#include "Arduino.h"

#include "esphome/core/component.h"

#include "ESP_I2S.h"
#include "WiFiClientSecure.h"
#include "HTTPClient.h"
#include "FS.h"
#include "SPI.h"
#include "SD.h"

namespace esphome{
    namespace handScanner{
        class Voice : public Component {
            public:
            Voice();
            void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
            void uploadFile();
            void generate_wav_header(uint8_t *wav_header, uint32_t wav_size, uint32_t sample_rate);
            void recordAndUpload(void *arg);
        }
    }
}
#endif