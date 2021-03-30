/*
 * org link: https://saasan.github.io/blog/2020/06/07/m5stickc-speaker-hat%E3%81%A7spiffs%E3%81%8B%E3%82%89%E8%AA%AD%E3%81%BF%E8%BE%BC%E3%82%93%E3%81%A0wav%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E3%82%92%E5%86%8D%E7%94%9F%E3%81%99%E3%82%8B.html
 * 
 * 使用以前的M5StickC +揚聲器帽播放音頻數據並不是在草圖上直接寫入音頻數據的明智方法。 
 * 這次，將讀取並播放預先保存在SPIFFS中的WAV文件。
 * 
 *  首先，將您要播放的音頻數據轉換為以下格式的WAV文件。
      單耳的
      採樣頻率8000Hz
      無符號8位PCM
      沒有元數據
    由於它只有一個揚聲器，因此它是單聲道的，並且由於M5StickC（或ESP32）的DAC是8位的，因此WAV文件也是8位的。
    採樣頻率可能不必為8000Hz，但我選擇此值是因為它不能在常用的44100Hz上很好地工作。

    此外，由於此草圖不考慮任何元數據，因此如果存在元數據，則錯誤消息“無效的波形文件標題”將顯示在串行監視器上，並且不會讀取該文件。
 *   
 *   WAVE_FILE_NAME是保存在SPIFFS中的WAV文件的文件名。 寫入後，按A按鈕（前面的“ M5”按鈕）播放WAV文件。
 */

#include <vector>
#include <M5StickC.h>
#include "FS.h"
#include "SPIFFS.h"

// WAVファイル名
const char WAVE_FILE_NAME[] = "/ring_mono_8K.wav";

// スピーカー出力ピンの番号
const uint8_t SPEAKER_PIN = GPIO_NUM_26;

// LOWでLED点灯、HIGHでLED消灯
const uint8_t LED_ON = LOW;
const uint8_t LED_OFF = HIGH;

// 電源ボタンが1秒未満押された
const uint8_t AXP_WAS_PRESSED = 2;

// PWM出力のチャンネル
const uint8_t PWM_CHANNEL = 0;

// PWM出力の分解能(ビット数)
const uint8_t PWM_RESOLUTION = 8;

// PWM出力の周波数
const uint32_t PWM_FREQUENCY = getApbFrequency() / (1U << PWM_RESOLUTION);

// 音声データのサンプリング周波数(Hz)
const uint32_t SOUND_SAMPLING_RATE = 8000;

// 音声データ再生時の待ち時間(マイクロ秒)
const uint32_t DELAY_INTERVAL = 1000000 / SOUND_SAMPLING_RATE;

// WAVファイルのヘッダー   // WAV文件頭
typedef struct {
    uint32_t riff;              // "RIFF" (0x52494646)
    uint32_t fileSize;          // ファイルサイズ-8
    uint32_t wave;              // "WAVE" (0x57415645)
    uint32_t fmt;               // "fmt " (0x666D7420)
    uint32_t fmtSize;           // fmtチャンクのバイト数
    uint16_t format;            // 音声フォーマット (非圧縮リニアPCMは1)
    uint16_t channels;          // チャンネル数
    uint32_t samplingRate;      // サンプリングレート
    uint32_t avgBytesPerSecond; // 1秒あたりのバイト数の平均
    uint16_t blockAlign;        // ブロックサイズ
    uint16_t bitsPerSample;     // 1サンプルあたりのビット数
    uint32_t data;              // "data" (0x64617461)
    uint32_t dataSize;          // 波形データのバイト数
} wavfileheader_t;

// PCMフォーマット  // PCM格式
const uint16_t WAVE_FORMAT_PCM = 0x0001;

// モノラル  // Mono
const uint16_t WAVE_MONAURAL = 0x0001;

// 音声データ  // 語音數據
std::vector<uint8_t> soundData;


// メッセージ出力 // 訊息輸出
void showMessage(const char* message) {
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(5, 30);
    M5.Lcd.setTextFont(4);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.print(message);
}

// バイトオーダーを入れ替える  // 交換字節順序
uint32_t reverseByteOrder(uint32_t x) {
    return ((x << 24 & 0xFF000000U) |
            (x <<  8 & 0x00FF0000U) |
            (x >>  8 & 0x0000FF00U) |
            (x >> 24 & 0x000000FFU));
}

// WAVファイルのヘッダーを検証する  // 驗證WAV文件頭
bool validateWavHeader(wavfileheader_t& header) 
{
    Serial.printf("riff: 0x%x\n", header.riff);
    Serial.printf("wave: 0x%x\n", header.wave);
    Serial.printf("fmt : 0x%x\n", header.fmt);
    Serial.printf("data: 0x%x\n", header.data);
    Serial.printf("format: %d\n", header.format);
    Serial.printf("channels: %d\n", header.channels);
    Serial.printf("samplingRate: %d\n", header.samplingRate);
    Serial.printf("bitsPerSample: %d\n", header.bitsPerSample);

    return  header.riff             == 0x52494646
            && header.wave          == 0x57415645
            && header.fmt           == 0x666D7420
            && header.data          == 0x64617461
            && header.format        == WAVE_FORMAT_PCM
            && header.channels      == WAVE_MONAURAL
            && header.samplingRate  == SOUND_SAMPLING_RATE
            && header.bitsPerSample == PWM_RESOLUTION;
}


// WAVファイルを読み込む  // 讀取WAV文件
void readWavFile(fs::FS& fs, const char* path, std::vector<uint8_t>& data) 
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()) 
    {
        Serial.println("- failed to open file for reading");
        return;
    }

    // WAVファイルのヘッダー   // WAV文件頭
    wavfileheader_t header;

    // ファイルサイズがヘッダーサイズ以下の場合は終了
    size_t fileSize = file.size();
    if (fileSize <= sizeof(header)) 
    {
        Serial.println("invalid wave file");
        return;
    }

    // ヘッダーサイズ分読み込む  // 讀取標題大小
    file.read((uint8_t*)&header, sizeof(header));

    // バイトオーダーを入れ替え  // 交換字節順序
    header.riff = reverseByteOrder(header.riff);
    header.wave = reverseByteOrder(header.wave);
    header.fmt  = reverseByteOrder(header.fmt);
    header.data = reverseByteOrder(header.data);

    // ヘッダーのチェック // 標頭檢查
    if (!validateWavHeader(header)) 
    {
        Serial.println("invalid wave file header");
        return;
    }

    // ファイルの読み込み  // 讀取檔案
   while (file.available()) 
   { 
        data.push_back(file.read());
   }
}

// 音声データを再生する   // 播放音頻數據
void playSound(std::vector<uint8_t>& soundData) 
{
    for (const auto& level : soundData) 
    {
        ledcWrite(PWM_CHANNEL, level);
        delayMicroseconds(DELAY_INTERVAL);
    }

    ledcWrite(PWM_CHANNEL, 0);
}

void setup() 
{
    M5.begin();
    M5.Lcd.setRotation(1);
    showMessage("SPIFFS WAV");

    // シリアルモニターの設定    // 串行監視器設置
    Serial.begin(115200);

    // スピーカーの設定  //揚聲器設定
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(SPEAKER_PIN, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 0);

    // LEDの設定
    pinMode(M5_LED, OUTPUT);
    digitalWrite(M5_LED, LED_OFF);

    // SPIFFSの設定
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    // 音声データを読み込む   // 讀取音頻數據
    readWavFile(SPIFFS, WAVE_FILE_NAME, soundData);
}

void loop() {
    delay(10);

    // ボタン(KEY)の状態を更新
    M5.update();

    // Aボタンが押されたら音声データ再生  // 按下A按鈕時播放音頻數據
    if (M5.BtnA.wasPressed()) {
        // LED点灯
        digitalWrite(M5_LED, LED_ON);
        // 音声データ再生   // 音頻數據播放
        playSound(soundData);
        // LED消灯
        digitalWrite(M5_LED, LED_OFF);
    }

    // 電源ボタンが押されたらリセット   / 按下電源按鈕時重置
    if (M5.Axp.GetBtnPress() == AXP_WAS_PRESSED) {
        esp_restart();
    }
}
