#include "Arduino.h"
#include "JPGVoice.h"


namespace esphome {
namespace handScanner {
JPGVoice::JPGVoice()
{
  ESP_LOGD("INFO:","Booting Up.");
  I2S.setAllPins(-1, 42, 41, -1, -1);
  //The transmission mode is PDM_MONO_MODE, which means that PDM (pulse density modulation) mono mode is used for transmission
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, SAMPLE_BITS)) {
    ESP_LOGD("INFO:","Failed to initialize I2S!");
  }

  SPI.begin(7,8,9,-1);
  // SPI.setDataMode(SPI_MODE0);
  if (!SD.begin(21)) {
    ESP_LOGD("INFO:","Failed to mount SD Card!");
  }
}

// void JPGVoice::listDir(const char *dirname, uint8_t levels) {
//   ESP_LOGD("INFO:","Listing directory: %s\n", dirname);
//   fs::FS* fs = SD;
//   File root = fs.open(dirname);
//   if (!root) {
//     ESP_LOGD("INFO:","Failed to open directory");
//     return;
//   }
//   if (!root.isDirectory()) {
//     ESP_LOGD("INFO:","Not a directory");
//     return;
//   }

//   File file = root.openNextFile();
//   while (file) {
//     if (file.isDirectory()) {
//       ESP_LOGD("INFO:",file.name());
//       if (levels) {
//         listDir(file.path(), levels - 1);
//       }
//     } else {
//       ESP_LOGD("INFO:",file.size());
//     }
//     file = root.openNextFile();
//   }
// }


void JPGVoice::generate_wav_header(uint8_t *wav_header, uint32_t wav_size, uint32_t sample_rate) {
  // See this for reference: http://soundfile.sapp.org/doc/WaveFormat/
  uint32_t file_size = wav_size + WAV_HEADER_SIZE - 8;
  uint32_t byte_rate = SAMPLE_RATE * SAMPLE_BITS / 8;
  const uint8_t set_wav_header[] = {
    'R', 'I', 'F', 'F',                                                   // ChunkID
    file_size, file_size >> 8, file_size >> 16, file_size >> 24,          // ChunkSize
    'W', 'A', 'V', 'E',                                                   // Format
    'f', 'm', 't', ' ',                                                   // Subchunk1ID
    0x10, 0x00, 0x00, 0x00,                                               // Subchunk1Size (16 for PCM)
    0x01, 0x00,                                                           // AudioFormat (1 for PCM)
    0x01, 0x00,                                                           // NumChannels (1 channel)
    sample_rate, sample_rate >> 8, sample_rate >> 16, sample_rate >> 24,  // SampleRate
    byte_rate, byte_rate >> 8, byte_rate >> 16, byte_rate >> 24,          // ByteRate
    0x02, 0x00,                                                           // BlockAlign
    0x10, 0x00,                                                           // BitsPerSample (16 bits)
    'd', 'a', 't', 'a',                                                   // Subchunk2ID
    wav_size, wav_size >> 8, wav_size >> 16, wav_size >> 24,              // Subchunk2Size
  };
  memcpy(wav_header, set_wav_header, sizeof(set_wav_header));
}

void JPGVoice::recordFile() {
  uint32_t sample_size = 0;

  //This variable will be used to point to the actual recording buffer
  uint8_t *rec_buffer = NULL;
  ESP_LOGD("INFO:","Ready to start recording ...\n");

  fileHandle = SD.open(defaultFilename, FILE_WRITE);

  // Write the header to the WAV file
  uint8_t wav_header[WAV_HEADER_SIZE];

  //Write the WAV file header information to the wav_header array
  generate_wav_header(wav_header, size_of_recording, SAMPLE_RATE);

  //Call the file.write() function to write the data in the wav_header array to the newly created WAV file
  fileHandle.write(wav_header, WAV_HEADER_SIZE);

  // This code uses the ESP32's PSRAM (external cache memory) to dynamically allocate a section of memory to store the recording data.
  rec_buffer = (uint8_t *)malloc(size_of_recording);
  if (rec_buffer == NULL) {
    ESP_LOGD("INFO:","malloc failed!\n");
  }
  ESP_LOGD("INFO:","Buffer: %d bytes\n", ESP.getPsramSize() - ESP.getFreePsram());

  // Start recording
  // I2S port number (in this case I2S_NUM_0),
  // a pointer to the buffer to which the data is to be written (i.e. rec_buffer),
  // the size of the data to be read (i.e. record_size),
  // a pointer to a variable that points to the actual size of the data being read (i.e. &sample_size),
  // and the maximum time to wait for the data to be read (in this case portMAX_DELAY, indicating an infinite wait time).
  esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, rec_buffer, size_of_recording, &sample_size, portMAX_DELAY);
  if (sample_size == 0) {
    ESP_LOGD("INFO:","Record Failed!\n");
  } else {
    ESP_LOGD("INFO:","Record %d bytes\n", sample_size);
  }

  // Increase volume
  for (uint32_t i = 0; i < sample_size; i += SAMPLE_BITS / 8) {
    (*(uint16_t *)(rec_buffer + i)) <<= VOLUME_GAIN;
  }

  // Write data to the WAV file
  ESP_LOGD("INFO:","Writing to the file ...\n");
  if (fileHandle.write(rec_buffer, size_of_recording) != size_of_recording)
    ESP_LOGD("INFO:","Write file Failed!\n");

  free(rec_buffer);
  rec_buffer = NULL;
  fileHandle.close();
  ESP_LOGD("INFO:","The recording is over.\n");

  // vTaskDelete(NULL);
}



void JPGVoice::uploadFile() {
  
  while (!isWIFIConnected) {
    delay(1000);
  }
  fileHandle = SD.open(defaultFilename, FILE_READ);
  if (!fileHandle) {
    ESP_LOGD("INFO:","fileHandle IS NOT AVAILABLE!");
    return;
  }

  ESP_LOGD("INFO:","===> Upload file to Node.js Server");

  HTTPClient client;
  client.begin("http://sliver.local:8888/uploadAudio");
  client.addHeader("Content-Type", "audio/wav");
  int httpResponseCode = client.sendRequest("POST", &fileHandle, fileHandle.size());
  ESP_LOGD("INFO:",httpResponseCode);

  if (httpResponseCode == 200) {
    String response = client.getString();
    ESP_LOGD("INFO:","==================== Transcription ====================");
    ESP_LOGD("INFO:",response);
    ESP_LOGD("INFO:","====================      End      ====================");
  } else {
    ESP_LOGD("INFO:","Error");
  }
  fileHandle.close();
  client.end();
}


// void JPGVoice::startRecordAndUploadTask(){
//   xTaskCreate(recordAndUpload, "recordAndUpload", 1024 * 8, NULL, 1, NULL);
// }

}
}