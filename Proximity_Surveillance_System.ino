#include "esp_camera.h"
#include <WiFi.h>

// ================= ULTRASONIC =================
#define TRIG_PIN 12
#define ECHO_PIN 14

unsigned long lastMeasure = 0;
bool photoTaken = false;
unsigned long lastCaptureTime = 0;
const unsigned long captureCooldown = 5000;

// ================= CAMERA =================
#include "board_config.h"

// ================= WIFI =================
const char *ssid = "viral5g";
const char *password = "Fmu_HgL@0795";

void startCameraServer();
void setupLedFlash();

// ================= SAFE DISTANCE =================
float readDistanceSafe() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Serial.println();

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

  // 🔴 FIXED: Use QVGA directly (stable)
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Open http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastMeasure > 700) {
    lastMeasure = millis();

    float distance = readDistanceSafe();
    if (distance > 0) {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

      if (distance <= 50) {
        if (!photoTaken && millis() - lastCaptureTime > captureCooldown) {

          camera_fb_t *fb = esp_camera_fb_get();
          if (fb) {
            Serial.println("📸 Auto-captured image");
            esp_camera_fb_return(fb);   // 🔴 SAFE: release immediately
            lastCaptureTime = millis();
            photoTaken = true;
          }
        }
      } else {
        photoTaken = false;
      }
    }
  }
  delay(10);
}