# 📷 ESP32-S3 Image Capture & Upload to Flask Server

This project enables an **ESP32-S3** with a camera module to capture images and upload them to a **Flask server**. The server receives, stores, and serves metadata about the uploaded images.


## 🧭 Table of Contents
- [📷 Overview](#-overview)
- [📁 Folder Structure](#-folder-structure)
- [⚙️ Setup & Installation](#️-setup--installation)
  - [🔧 Flask Server Setup](#-flask-server-setup)
  - [📡 ESP32-S3 Setup](#-esp32-s3-setup)
- [🚀 API Endpoints](#-api-endpoints)
- [🧪 Testing](#-testing)
- [🛠️ Troubleshooting](#️-troubleshooting)
- [✅ Conclusion](#-conclusion)


## 📷 Overview

This project contains two primary components:

### 1️⃣ ESP32-S3 (Client)
- Captures an image using a camera module.
- Sends the image as raw binary data to a remote Flask server using HTTP POST.

### 2️⃣ Flask Server (Host)
- Receives and stores images in a local directory (`img/`).
- Exposes REST API endpoints to upload and list stored images.

---

## 📁 Folder Structure

esp32-flask-image-upload/
├── img/                  # Folder to store uploaded images
├── imgCap/               # ESP32-S3 Arduino code
│   ├── camera\_pins.h     # Camera pin configuration
│   └── esp32\_code.ino    # Main Arduino sketch for image capture/upload
├── server.py             # Flask server implementation
└── README.md             # Project documentation


## ⚙️ Setup & Installation

### 🔧 Flask Server Setup

#### 1. Install Python & Flask
Ensure Python is installed. Then install Flask:

pip install flask

#### 2. Install Additional Dependencies (if needed)

pip install requests

#### 3. Start the Flask Server

python server.py

Expected output:

 * Running on http://127.0.0.1:5000
 * Running on http://<your-local-ip>:5000

#### 4. (Optional) Expose Server Remotely with ngrok

ngrok http 5000

Use the generated **ngrok public URL** in your ESP32-S3 code to enable remote access.

### 📡 ESP32-S3 Setup

#### 1. Install Arduino IDE & ESP32 Board Package

* Download the **Arduino IDE** from the [official site](https://www.arduino.cc/en/software).
* In Arduino IDE, go to **File > Preferences**, and add the ESP32 board package URL:

  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

* Then go to **Tools > Board > Boards Manager**, search for `ESP32`, and install it.

#### 2. Install Required Arduino Libraries

Install the following libraries via **Library Manager**:

* `WiFi` (comes with ESP32)
* `ArduinoJson` (if needed for extensions)

#### 3. Configure the ESP32 Sketch

In `imgCap/esp32_code.ino`, modify the server URL:

const char* serverUrl = "http://192.168.1.100:5000/img";  // Local IP
// OR for ngrok tunnel:
const char* serverUrl = "https://your-ngrok-subdomain.ngrok-free.app/img";

Upload the code to your ESP32-S3 using Arduino IDE.

## 🚀 API Endpoints

### 📤 Upload Image

* **URL:** `/img`
* **Method:** `POST`
* **Content-Type:** `image/jpeg`
* **Description:** Accepts raw JPEG binary data and stores it on the server.

### 📄 List Uploaded Images

* **URL:** `/list_images`
* **Method:** `GET`
* **Response:** JSON object with image filenames:


{
  "images": [
    "captured_20250831_153245.jpg",
    "captured_20250831_154012.jpg"
  ]
}

## 🧪 Testing

### ✅ Upload Manually via cURL

To test without the ESP32-S3:


curl -X POST -H "Content-Type: image/jpeg" --data-binary @test.jpg http://127.0.0.1:5000/img

### ✅ List Uploaded Images


curl http://127.0.0.1:5000/list_images


Expected output:


{
  "images": ["captured_20250831_153245.jpg"]
}

## 🛠️ Troubleshooting

### 🚫 Flask Server Not Starting?

* Run this to verify Flask is installed:

  pip show flask
  
* Change port if 5000 is already in use:

  app.run(host='0.0.0.0', port=5001)
  

### ❌ ESP32-S3 Not Sending Images?

* Double-check Wi-Fi credentials.
* Make sure the `serverUrl` matches your actual Flask/ngrok URL.
* Use `Serial.println()` in Arduino to debug.

### 🌐 ngrok Not Working?

* Ensure Flask server is running *before* launching ngrok.
* Restart ngrok if the tunnel is unresponsive:


  ngrok http 5000
  
* Update the ESP32 code with the new ngrok URL.

## ✅ Conclusion

With this setup, your **ESP32-S3** device can:

* Capture photos via onboard camera,
* Send them over Wi-Fi to a Python Flask server,
* Store and organize them neatly for future access.

This project can be expanded for **IoT surveillance**, **smart doorbells**, or **edge-AI camera projects**.

