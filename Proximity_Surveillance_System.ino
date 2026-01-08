#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>

// ================= CAMERA MODEL =================
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// ================= WIFI =================
const char* ssid = "viral5g";
const char* password = "Fmu_HgL@0795";

// ================= TELEGRAM =================
#define BOT_TOKEN "8365910543:AAHEtUzw-y5hDMpDKTo3If6rnOfgtwPVqaY"
#define CHAT_ID   "8316607438"

// ===================== ULTRASONIC =====================
#define TRIG_PIN 12
#define ECHO_PIN 14

// ===================== DISTANCE LOGIC =====================
#define MIN_VALID_CM       5
#define MAX_TRIGGER_CM     50
#define CONFIRM_TIME_MS    800
#define COOLDOWN_MS        10000

unsigned long presenceStart = 0;
unsigned long lastPhotoTime = 0;
bool objectPresent = false;

// ===================== CAMERA (AI THINKER) =====================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===================== DISTANCE READ =====================
float readDistanceSafe() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;

  float cm = duration * 0.034 / 2;
  if (cm < MIN_VALID_CM || cm > 300) return -1;

  return cm;
}

// ===================== SEND PHOTO =====================
void sendPhotoToTelegram(camera_fb_t *fb, float distance) {
  WiFiClientSecure client;
  client.setInsecure();

  String boundary = "----ESP32CAMBoundary";
  String startRequest =
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n" +
    String(CHAT_ID) + "\r\n"
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"caption\"\r\n\r\n"
    "📸 Object detected\nDistance: " + String(distance, 1) + " cm\r\n"
    "--" + boundary + "\r\n"
    "Content-Disposition: form-data; name=\"photo\"; filename=\"image.jpg\"\r\n"
    "Content-Type: image/jpeg\r\n\r\n";

  String endRequest = "\r\n--" + boundary + "--\r\n";

  int contentLength = startRequest.length() + fb->len + endRequest.length();

  if (!client.connect("api.telegram.org", 443)) {
    Serial.println("❌ Telegram connection failed");
    return;
  }

  client.println("POST /bot" + String(BOT_TOKEN) + "/sendPhoto HTTP/1.1");
  client.println("Host: api.telegram.org");
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println("Content-Length: " + String(contentLength));
  client.println();
  client.print(startRequest);
  client.write(fb->buf, fb->len);
  client.print(endRequest);

  Serial.println("📤 Photo sent to Telegram");
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_camera_init(&config);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
}

// ===================== LOOP =====================
void loop() {
  float distance = readDistanceSafe();

  if (distance > 0) {
    Serial.printf("Distance: %.2f cm\n", distance);

    if (distance <= MAX_TRIGGER_CM) {
      if (!objectPresent) {
        objectPresent = true;
        presenceStart = millis();
      }

      if (millis() - presenceStart >= CONFIRM_TIME_MS &&
          millis() - lastPhotoTime >= COOLDOWN_MS) {

        Serial.println("📸 Capturing image...");
        camera_fb_t *fb = esp_camera_fb_get();
        if (fb) {
          sendPhotoToTelegram(fb, distance);
          esp_camera_fb_return(fb);
          lastPhotoTime = millis();
        }
      }
    } else {
      objectPresent = false;
    }
  } else {
    objectPresent = false;
  }

  delay(200);
}
