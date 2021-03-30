/**
 * 
 * org likk: https://translate.google.com.tw/?hl=zh-TW&sl=ja&tl=zh-TW&text=%E5%89%8D%E5%9B%9E%E8%A1%8C%E3%81%A3%E3%81%9F%E8%87%AA%E5%8A%9B%E3%81%A7%E3%81%AE%20WAV%20%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E5%86%8D%E7%94%9F%E3%81%A7%E3%81%AF%208bit%2C%208000Hz%20%E9%99%90%E5%AE%9A%E3%81%A0%E3%81%A3%E3%81%9F%E3%81%93%E3%81%A8%E3%82%82%E3%81%82%E3%82%8A%E3%80%81%20%E6%AF%94%E8%BC%83%E3%81%99%E3%82%8B%E3%81%A8%E9%9F%B3%E8%B3%AA%E3%81%AF%E3%81%8B%E3%81%AA%E3%82%8A%E6%94%B9%E5%96%84%E3%81%97%E3%81%BE%E3%81%97%E3%81%9F%E3%81%8C%E3%80%81%20%E5%86%8D%E7%94%9F%E3%81%AE%E5%89%8D%E5%BE%8C%E3%81%AB%E3%83%97%E3%83%81%E3%83%97%E3%83%81%E3%81%A8%E3%83%8E%E3%82%A4%E3%82%BA%E3%81%8C%E5%85%A5%E3%82%8B%E3%82%88%E3%81%86%E3%81%AB%E3%81%AA%E3%82%8A%E3%81%BE%E3%81%97%E3%81%9F%E3%80%82%20%E3%81%BE%E3%81%9F%E3%80%81%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E3%81%AB%E3%81%8B%E3%81%8B%E3%82%8B%E6%99%82%E9%96%93%E3%82%82%E3%81%8B%E3%81%AA%E3%82%8A%E9%95%B7%E3%81%8F%E3%81%AA%E3%81%A3%E3%81%A6%E3%81%84%E3%81%BE%E3%81%99%E3%80%82%0A%0A%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E6%99%82%E9%96%93%E3%81%AF%E3%81%A9%E3%81%86%E3%81%97%E3%82%88%E3%81%86%E3%82%82%E3%81%AA%E3%81%84%E3%81%A8%E6%80%9D%E3%81%84%E3%81%BE%E3%81%99%E3%81%8C%E3%80%81%20%E5%86%8D%E7%94%9F%E5%89%8D%E5%BE%8C%E3%81%AE%E3%83%97%E3%83%81%E3%83%97%E3%83%81%E3%81%AF%E4%B8%8B%E8%A8%98%E5%8F%82%E8%80%83%E3%82%B5%E3%82%A4%E3%83%88%E3%81%AE%E3%82%AF%E3%83%AA%E3%83%83%E3%82%AF%E3%83%8E%E3%82%A4%E3%82%BA%E5%AF%BE%E7%AD%96%E3%82%92%E8%A1%8C%E3%81%88%E3%81%B0%E6%B6%88%E3%81%9B%E3%82%8B%E3%81%8B%E3%82%82%E3%81%97%E3%82%8C%E3%81%BE%E3%81%9B%E3%82%93%E3%80%82&op=translate
 * 
 * 我曾經用我自己讀取過的WAV文件來播放從SPIFFS讀取的WAV文件，但是即使使用配備了ESP32-PICO的M5StickC，也可以使用稱為ESP8266Audio的庫來播放MP3。
 * 因此，這次，我將預先將ESP8266Audio中保存在SPIFFS中的MP3文件加載並播放。
 * 
 * 我自己以前的WAV文件播放限制為8bit和8000Hz，因此與之相比，聲音質量有了很大提高，但是在播放前後，氣泡包裹和噪音開始出現。 編譯也需要很多時間。
 * 我認為編譯時間無濟於事，但可以通過採取以下措施防止在參考點上的點擊噪音，消除播放前後的氣泡包裹現象。
 * 
 */

/*
 它使用從SPIFFS讀取文件的 AudioFileSourceSPIFFS 類，播放MP3的AudioGeneratorMP3類以及在I2S中輸出的AudioOutputI2S類。 
 ESP8266Audio可以讀取MP3以外的WAV，FLAC，MIDI和AAC等文件格式，您需要使用與每種文件格式相對應的generator類。

 使用generator-> isRunning（）檢查它是否正在播放，如果沒有播放，則按下該按鈕，則正在播放MP3文件。
 播放完成後，調用generator-> stop（）關閉文件。因此，必須重新打開該文件以進行第二次和後續播放。
 使用source-> isOpen（）檢查文件是否已打開，如果不是，請使用source-> open（）打開文件。

 AudioFileSourceSPIFFS類每次播放都會讀取文件，因此，如果您想多次播放適合ESP32-PICO的內存（520KB）的文件，
 請按如下所示在setup（）中讀取該文件，並使用AudioFileSourcePROGMEM上課，我認為您應該使用它。
*/


#include <M5StickC.h>
#include "FS.h"
#include "SPIFFS.h"
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

// オーディオファイル名
const char AUDIO_FILE_NAME[] = "/ring_mono_8K.mp3";

const int I2S_NUM_0 = 0;
// 電源ボタンが1秒未満押された
const uint8_t AXP_WAS_PRESSED = 2;

AudioGeneratorMP3 *generator = NULL;
AudioFileSourcePROGMEM *source = NULL;
AudioOutputI2S *output = NULL;
uint8_t *audioData = NULL;
size_t audioDataSize = 0;

// メッセージ出力
void showMessage(const char *message) {
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(5, 35);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.print(message);
}

void readAudioFile(fs::FS& fs, const char* path) {
    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return;
    }

    audioDataSize = file.size();
    audioData = new uint8_t[audioDataSize];
    file.read(audioData, audioDataSize);
}

void playAudio() {
    Serial.println("playAudio");

    stopAudio();
    if (!source->isOpen()) {
        source->open(audioData, audioDataSize);
    }
    generator->begin(source, output);
}

void stopAudio() {
    Serial.println("stopAudio");

    if (generator->isRunning()) {
        generator->stop();
    }
}

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    showMessage("ESP8266Audio");

    // シリアルモニターの設定
    Serial.begin(115200);
    audioLogger = &Serial;

    // SPIFFSの設定
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    readAudioFile(SPIFFS, AUDIO_FILE_NAME);

    source = new AudioFileSourcePROGMEM(audioData, audioDataSize);
    output = new AudioOutputI2S(I2S_NUM_0, AudioOutputI2S::INTERNAL_DAC);
    generator = new AudioGeneratorMP3();
}

void loop() 
{
    delay(10);

    if (generator->isRunning()) {
        if (!generator->loop()) {
            stopAudio();
        }
    }
    else {
        // ボタンの状態を更新
        M5.update();

        // Aボタンが押されたら音声データ再生
        if (M5.BtnA.wasPressed()) {
            playAudio();
        }

        // 電源ボタンが押されたらリセット
        if (M5.Axp.GetBtnPress() == AXP_WAS_PRESSED) {
            esp_restart();
        }
    }
}
