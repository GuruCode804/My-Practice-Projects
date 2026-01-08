#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

const char* ssid     = "viral5g";       // Your WiFi SSID
const char* password = "Fmu_HgL@0795";   // Your WiFi Password
const char* webhookUrl = "https://script.google.com/macros/s/AKfycbztOLCuOQOxF1Zs_V-gTEUBTRNlZNOUGixa29m_O--dUFezhI9YbnjiHRES7BkyyOnKfQ/exec";  // Replace with your deployed web app URL

const int trigPin = 18;
const int echoPin = 19;
const int servoPin = 21;
const float maxDistance = 30.0;  // Threshold for object detection (cm)

Servo myServo;  // Servo object

float readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout
  return (duration * 0.034f) / 2.0f;
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi connection failed");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  myServo.attach(servoPin);  // Attach servo
  
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Sweep from 0 to 180 degrees
  for (int pos = 0; pos <= 180; pos += 20) {
    myServo.write(pos);
    delay(50);  // Pause for stability
    float distance = readDistanceCm();
    String detected = (distance > 0 && distance <= maxDistance) ? "yes" : "no";
    
    Serial.print("Angle: " + String(pos) + " | Detected: " + detected + " (Distance: " + String(distance) + " cm)\n");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(webhookUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonPayload = "{\"angle\":" + String(pos) + ",\"object_detected\":\"" + detected + "\"}";
      Serial.println("Sending: " + jsonPayload);  // Debug

      int httpResponseCode = http.POST(jsonPayload);
      Serial.println("HTTP Response: " + String(httpResponseCode));
      http.end();
    }
  }

  // Sweep back from 180 to 0 degrees
  for (int pos = 180; pos >= 0; pos -= 20) {
    myServo.write(pos);
    delay(50);
    float distance = readDistanceCm();
    String detected = (distance > 0 && distance <= maxDistance) ? "yes" : "no";
    
    Serial.print("Angle: " + String(pos) + " | Detected: " + detected + " (Distance: " + String(distance) + " cm)\n");

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(webhookUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonPayload = "{\"angle\":" + String(pos) + ",\"object_detected\":\"" + detected + "\"}";
      Serial.println("Sending: " + jsonPayload);

      int httpResponseCode = http.POST(jsonPayload);
      Serial.println("HTTP Response: " + String(httpResponseCode));
      http.end();
    }
  }
}
