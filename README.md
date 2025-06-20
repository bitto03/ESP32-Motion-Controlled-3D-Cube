# ESP32-Motion-Controlled-3D-Cube

A real-time 3D visualization project using the MPU6050 sensor and ESP32. The project displays a 3D cube in a web browser that responds to motion and orientation captured by the sensor. The data is streamed using WebSocket for smooth and fast updates.

## Features

- Real-time 3D cube rotation in the browser based on motion.
- MPU6050 (Accelerometer + Gyroscope) sensor data visualization.
- Web interface served directly from the ESP32.
- WebSocket communication for low-latency updates.
- Stylish, responsive UI built with HTML, CSS, and Three.js.

## Hardware Required

- ESP32 (Dev Board)
- MPU6050 Sensor
- Jumper Wires

## Circuit Connections

MPU6050 → ESP32
VCC → 3.3V
GND → GND
SCL → GPIO 22
SDA → GPIO 21


## How to Use

1. Upload the Arduino code to the ESP32 using the Arduino IDE.
2. Connect the ESP32 to a WiFi network (change `ssid` and `password` in the code).
3. Open the Serial Monitor to find the ESP32's IP address.
4. Enter the IP address in your browser to access the visualization interface.
5. Move or tilt the MPU6050 sensor to see the 3D cube rotate in real-time.

## Libraries Used

- WiFi.h
- WebServer.h
- WebSocketsServer.h
- Adafruit_MPU6050
- Adafruit_Sensor
- Three.js (loaded from CDN)

## Notes

- Make sure your browser and ESP32 are on the same WiFi network.
- The WebSocket runs on port 81.
- Sensor data is sent at 20Hz (50ms intervals).
- Uses complementary filtering for stable orientation estimation.
