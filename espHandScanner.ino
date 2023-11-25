#include <esphome/components/JPGVoice/JPGVoice.h>
void setup(){
    Serial.begin(115200);
    voiceBox = JPGVoice();
    voiceBox.listDir("/", 0);
}


void loop(){

}