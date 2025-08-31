#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

// WiFi Credentials
const char* ssid = "Airtel_sach_1806";
const char* password = "air03704";












// Flask Server URLs
const char* serverUrl = "https://a9d0-2401-4900-36c3-f722-88b1-7bbe-9349-128a.ngrok-free.app";
const char* uploadUrl = "/img";
const char* commandUrl = "/get_command";

// Motor Control Pins
#define IN1 1  
#define IN2 2 
#define IN3 3  
#define IN4 4  
#define ENA 5  
#define ENB 6  

// Motor Speed Configuration
const int SPEED = 100;  
const int TURN_DURATION = 300; 

void setup() {
    Serial.begin(115200);
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ WiFi connected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Initialize Motor Pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    stopMotor();

    // Camera Configuration
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
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_count = 1;

    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("❌ Camera init failed!");
        return;
    }
    Serial.println("📸 Camera Ready!");
}

void sendImage() {
    Serial.println("📷 Capturing Image...");
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("❌ Camera capture failed!");
        return;
    }

    Serial.printf("✅ Captured image: %d bytes\n", fb->len);
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String fullUrl = String(serverUrl) + String(uploadUrl);
        http.begin(fullUrl);
        http.addHeader("Content-Type", "image/jpeg");
        
        int httpResponseCode = http.POST(fb->buf, fb->len);
        Serial.printf("📡 Server Response: %d\n", httpResponseCode);
        http.end();
    }
    esp_camera_fb_return(fb);
}

void fetchCommand() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String fullUrl = String(serverUrl) + String(commandUrl);
        http.begin(fullUrl);

        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            String response = http.getString();
            response.trim();
            Serial.println("📩 Received Command: " + response);

            if (response.startsWith("{") && response.endsWith("}")) {
                int startIndex = response.indexOf(":\"") + 3;
                int endIndex = response.lastIndexOf("\"");
                if (startIndex > 2 && endIndex > startIndex) {
                    String command = response.substring(startIndex, endIndex);
                    Serial.println("✅ Extracted Command: " + command);
                    executeCommand(command.charAt(0));
                }
            } else {
                Serial.println("⚠️ Invalid JSON Format!");
            }
        } else {
            Serial.printf("❌ Error fetching command: %s\n", http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        Serial.println("❌ WiFi Disconnected!");
    }
}

void executeCommand(char command) {
    if (command == 'F') startMotor();
    else if (command == 'B') moveBackward();
    else if (command == 'S') stopMotor();
    else if (command == 'R') turnRight();
    else if (command == 'L') turnLeft();
}

void startMotor() {
    Serial.println("✅ Motor Started");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, SPEED);
    analogWrite(ENB, SPEED);
}

void moveBackward() {
    Serial.println("🚗 Moving Backward");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, SPEED);
    analogWrite(ENB, SPEED);
}

void stopMotor() {
    Serial.println("🛑 Motor Stopped");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void turnRight() {
    Serial.println("↪️ Turning Right");
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    delay(TURN_DURATION);
    stopMotor();
}

void turnLeft() {
    Serial.println("↩️ Turning Left");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    delay(TURN_DURATION);
    stopMotor();
}

void loop() {
    sendImage();
    fetchCommand();
    delay(1000);
}
