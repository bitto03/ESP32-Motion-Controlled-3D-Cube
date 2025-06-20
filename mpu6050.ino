#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
WebServer server(80);
WebSocketsServer webSocket(81);

// WiFi credentials - replace with your network details
const char* ssid = "Bitto";
const char* password = "LAIBA2026";

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="utf-8">
    <title>ESP32 Motion-Controlled 3D Cube</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        box-sizing: border-box;
      }
      body {
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        background: linear-gradient(135deg, #1a2a6c, #2c3e50);
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        color: #fff;
        overflow-x: hidden;
      }
      .header {
        text-align: center;
        padding: 20px;
        background: rgba(0, 0, 0, 0.4);
        border-bottom: 1px solid rgba(255, 255, 255, 0.1);
      }
      h1 {
        font-size: 2.2rem;
        margin-bottom: 5px;
        text-shadow: 0 0 10px rgba(0, 200, 255, 0.7);
      }
      .subtitle {
        color: #bbb;
        font-size: 1.1rem;
        max-width: 800px;
        margin: 0 auto;
      }
      .container {
        display: flex;
        flex-wrap: wrap;
        justify-content: center;
        padding: 20px;
        gap: 30px;
        max-width: 1600px;
        margin: 0 auto;
      }
      .visualization {
        flex: 1;
        min-width: 400px;
        background: rgba(0, 15, 30, 0.7);
        border-radius: 15px;
        padding: 15px;
        box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
        border: 1px solid rgba(0, 200, 255, 0.3);
      }
      .visualization-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 15px;
        padding-bottom: 10px;
        border-bottom: 1px solid rgba(255, 255, 255, 0.1);
      }
      .canvas-container {
        position: relative;
        width: 100%;
        height: 450px;
        background: #000;
        border-radius: 10px;
        overflow: hidden;
      }
      #cubeCanvas {
        width: 100%;
        height: 100%;
      }
      .status-indicator {
        display: flex;
        align-items: center;
        gap: 8px;
        font-size: 1.1rem;
      }
      .status-dot {
        width: 12px;
        height: 12px;
        border-radius: 50%;
        background: #ff296d;
      }
      .status-dot.connected {
        background: #4cff00;
        box-shadow: 0 0 10px #4cff00;
      }
      .sensor-data {
        flex: 1;
        min-width: 300px;
        background: rgba(0, 15, 30, 0.7);
        border-radius: 15px;
        padding: 20px;
        box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
        border: 1px solid rgba(0, 200, 255, 0.3);
      }
      .sensor-grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
        gap: 20px;
        margin-top: 20px;
      }
      .sensor-card {
        background: rgba(0, 30, 60, 0.6);
        border-radius: 10px;
        padding: 15px;
        border: 1px solid rgba(0, 150, 255, 0.3);
      }
      .sensor-title {
        color: #0cf;
        font-size: 1.2rem;
        margin-bottom: 10px;
        text-align: center;
      }
      .data-row {
        display: flex;
        justify-content: space-between;
        padding: 8px 0;
        border-bottom: 1px solid rgba(255, 255, 255, 0.1);
      }
      .data-label {
        color: #aaa;
      }
      .data-value {
        font-weight: bold;
        font-family: monospace;
      }
      .instructions {
        background: rgba(0, 15, 30, 0.7);
        border-radius: 15px;
        padding: 20px;
        margin: 20px auto;
        max-width: 1200px;
        box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5);
        border: 1px solid rgba(0, 200, 255, 0.3);
      }
      .instructions h2 {
        color: #0cf;
        margin-bottom: 15px;
        text-align: center;
      }
      .steps {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
        gap: 20px;
      }
      .step {
        background: rgba(0, 30, 60, 0.6);
        border-radius: 10px;
        padding: 15px;
        border: 1px solid rgba(0, 150, 255, 0.3);
      }
      .step h3 {
        color: #4cff00;
        margin-bottom: 10px;
        display: flex;
        align-items: center;
        gap: 10px;
      }
      .step-number {
        background: #0cf;
        color: #000;
        width: 30px;
        height: 30px;
        border-radius: 50%;
        display: flex;
        align-items: center;
        justify-content: center;
        font-weight: bold;
      }
      .footer {
        text-align: center;
        padding: 20px;
        margin-top: auto;
        color: #bbb;
        font-size: 0.9rem;
        background: rgba(0, 0, 0, 0.4);
        border-top: 1px solid rgba(255, 255, 255, 0.1);
      }
      .highlight {
        color: #4cff00;
        font-weight: bold;
      }
      @media (max-width: 768px) {
        .container {
          flex-direction: column;
        }
        .visualization, .sensor-data {
          min-width: 100%;
        }
      }
    </style>
  </head>
  <body>
    <div class="header">
      <h1>ESP32 Motion-Controlled 3D Cube</h1>
      <p class="subtitle">Real-time 3D visualization using MPU6050 6DOF sensor and WebSocket communication</p>
    </div>
    
    <div class="container">
      <div class="visualization">
        <div class="visualization-header">
          <h2>3D CUBE VISUALIZATION</h2>
          <div class="status-indicator">
            <div class="status-dot"></div>
            <span id="status">DISCONNECTED</span>
          </div>
        </div>
        <div class="canvas-container">
          <canvas id="cubeCanvas"></canvas>
        </div>
      </div>
      
      <div class="sensor-data">
        <h2>SENSOR DATA</h2>
        <div class="sensor-grid">
          <div class="sensor-card">
            <h3 class="sensor-title">ACCELEROMETER</h3>
            <div class="data-row">
              <span class="data-label">X-axis:</span>
              <span id="ax-value" class="data-value">0.00 m/s²</span>
            </div>
            <div class="data-row">
              <span class="data-label">Y-axis:</span>
              <span id="ay-value" class="data-value">0.00 m/s²</span>
            </div>
            <div class="data-row">
              <span class="data-label">Z-axis:</span>
              <span id="az-value" class="data-value">0.00 m/s²</span>
            </div>
          </div>
          
          <div class="sensor-card">
            <h3 class="sensor-title">GYROSCOPE</h3>
            <div class="data-row">
              <span class="data-label">X-axis:</span>
              <span id="gx-value" class="data-value">0.00 rad/s</span>
            </div>
            <div class="data-row">
              <span class="data-label">Y-axis:</span>
              <span id="gy-value" class="data-value">0.00 rad/s</span>
            </div>
            <div class="data-row">
              <span class="data-label">Z-axis:</span>
              <span id="gz-value" class="data-value">0.00 rad/s</span>
            </div>
          </div>
          
          <div class="sensor-card">
            <h3 class="sensor-title">ORIENTATION</h3>
            <div class="data-row">
              <span class="data-label">Pitch:</span>
              <span id="pitch-value" class="data-value">0.00°</span>
            </div>
            <div class="data-row">
              <span class="data-label">Roll:</span>
              <span id="roll-value" class="data-value">0.00°</span>
            </div>
            <div class="data-row">
              <span class="data-label">Yaw:</span>
              <span id="yaw-value" class="data-value">0.00°</span>
            </div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="instructions">
      <h2>INSTRUCTIONS</h2>
      <div class="steps">
        <div class="step">
          <h3><span class="step-number">1</span> Hardware Setup</h3>
          <p>Connect your MPU6050 sensor to the ESP32:</p>
          <p>- VCC → 3.3V</p>
          <p>- GND → GND</p>
          <p>- SCL → GPIO 22</p>
          <p>- SDA → GPIO 21</p>
        </div>
        <div class="step">
          <h3><span class="step-number">2</span> Operation</h3>
          <p>Move the sensor to rotate the 3D cube in real-time:</p>
          <p>- Tilt forward/backward to rotate around X-axis</p>
          <p>- Tilt left/right to rotate around Y-axis</p>
          <p>- Rotate clockwise/counter-clockwise for Z-axis</p>
        </div>
        <div class="step">
          <h3><span class="step-number">3</span> Connection</h3>
          <p>Ensure your device is connected to the same WiFi network as the ESP32.</p>
          <p>Visit the IP address shown in the Serial Monitor to access this interface.</p>
          <p>Connection status: <span id="connection-status">Not connected</span></p>
        </div>
      </div>
    </div>
    
    <div class="footer">
      <p>ESP32 Motion Control System | MPU6050 6DOF Sensor | WebSocket Communication</p>
    </div>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js"></script>
    <script>
      // WebSocket connection
      let ws;
      let isConnected = false;
      let reconnectInterval;
      
      // Three.js variables
      let scene, camera, renderer, cube, line;
      let rotationX = 0, rotationY = 0, rotationZ = 0;
      const filterFactor = 0.96;
      let lastUpdate = 0;
      
      // DOM elements
      const statusDot = document.querySelector('.status-dot');
      const statusText = document.getElementById('status');
      const connectionStatus = document.getElementById('connection-status');
      
      // Initialize WebSocket connection
      function initWebSocket() {
        const host = window.location.hostname;
        ws = new WebSocket('ws://' + host + ':81/');
        
        ws.onopen = () => {
          isConnected = true;
          statusText.textContent = 'CONNECTED';
          statusDot.classList.add('connected');
          connectionStatus.textContent = 'Connected to ESP32';
          connectionStatus.style.color = '#4cff00';
          clearInterval(reconnectInterval);
        };
        
        ws.onmessage = handleSensorData;
        
        ws.onclose = () => {
          isConnected = false;
          statusText.textContent = 'DISCONNECTED';
          statusDot.classList.remove('connected');
          connectionStatus.textContent = 'Disconnected - attempting to reconnect...';
          connectionStatus.style.color = '#ff296d';
          reconnectInterval = setInterval(initWebSocket, 2000);
        };
        
        ws.onerror = (error) => {
          console.error('WebSocket Error:', error);
        };
      }
      
      // Initialize Three.js scene
      function initScene() {
        // Get canvas element
        const canvas = document.getElementById('cubeCanvas');
        
        // Create scene
        scene = new THREE.Scene();
        scene.background = new THREE.Color(0x0a0a1a);
        
        // Create camera
        camera = new THREE.PerspectiveCamera(75, canvas.clientWidth / canvas.clientHeight, 0.1, 1000);
        camera.position.z = 10;
        
        // Create renderer
        renderer = new THREE.WebGLRenderer({ 
          canvas: canvas,
          antialias: true 
        });
        renderer.setSize(canvas.clientWidth, canvas.clientHeight);
        
        // Add lighting
        const ambientLight = new THREE.AmbientLight(0x333333);
        scene.add(ambientLight);
        
        const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
        directionalLight.position.set(5, 10, 7);
        scene.add(directionalLight);
        
        const pointLight = new THREE.PointLight(0x00aaff, 1, 100);
        pointLight.position.set(-5, -5, 10);
        scene.add(pointLight);
        
        // Create cube
        const geometry = new THREE.BoxGeometry(4, 4, 4);
        
        // Materials for each face
        const materials = [
          new THREE.MeshPhongMaterial({ color: 0xff0033, transparent: true, opacity: 0.8 }), // red
          new THREE.MeshPhongMaterial({ color: 0x00aa44, transparent: true, opacity: 0.8 }), // green
          new THREE.MeshPhongMaterial({ color: 0x0066ff, transparent: true, opacity: 0.8 }), // blue
          new THREE.MeshPhongMaterial({ color: 0xffcc00, transparent: true, opacity: 0.8 }), // yellow
          new THREE.MeshPhongMaterial({ color: 0xaa00ff, transparent: true, opacity: 0.8 }), // purple
          new THREE.MeshPhongMaterial({ color: 0xff7700, transparent: true, opacity: 0.8 })  // orange
        ];
        
        cube = new THREE.Mesh(geometry, materials);
        scene.add(cube);
        
        // Create wireframe
        const edges = new THREE.EdgesGeometry(geometry);
        const lineMaterial = new THREE.LineBasicMaterial({ 
          color: 0xffffff,
          linewidth: 2
        });
        line = new THREE.LineSegments(edges, lineMaterial);
        cube.add(line);
        
        // Add coordinate axes
        const axesHelper = new THREE.AxesHelper(8);
        scene.add(axesHelper);
        
        // Add grid helper
        const gridHelper = new THREE.GridHelper(20, 20, 0x444444, 0x222222);
        scene.add(gridHelper);
      }
      
      // Handle incoming sensor data
      function handleSensorData(event) {
        if (!event.data) return;
        
        try {
          const data = JSON.parse(event.data);
          const now = Date.now();
          const dt = (now - lastUpdate) / 1000;
          lastUpdate = now;
          
          if (dt < 0.2) {  // Prevent large jumps
            // Complementary filter for stable rotation
            rotationX = filterFactor * (rotationX + data.gx * dt) + 
                        (1 - filterFactor) * Math.atan2(data.ay, data.az);
            rotationY = filterFactor * (rotationY + data.gy * dt) + 
                        (1 - filterFactor) * Math.atan2(data.ax, data.az);
            rotationZ = filterFactor * (rotationZ + data.gz * dt);
            
            // Update sensor data display
            document.getElementById('ax-value').textContent = data.ax.toFixed(2) + " m/s²";
            document.getElementById('ay-value').textContent = data.ay.toFixed(2) + " m/s²";
            document.getElementById('az-value').textContent = data.az.toFixed(2) + " m/s²";
            
            document.getElementById('gx-value').textContent = data.gx.toFixed(2) + " rad/s";
            document.getElementById('gy-value').textContent = data.gy.toFixed(2) + " rad/s";
            document.getElementById('gz-value').textContent = data.gz.toFixed(2) + " rad/s";
            
            // Calculate orientation angles in degrees
            const pitch = Math.atan2(data.ay, Math.sqrt(data.ax * data.ax + data.az * data.az)) * 180 / Math.PI;
            const roll = Math.atan2(-data.ax, data.az) * 180 / Math.PI;
            const yaw = rotationZ * 180 / Math.PI;
            
            document.getElementById('pitch-value').textContent = pitch.toFixed(2) + "°";
            document.getElementById('roll-value').textContent = roll.toFixed(2) + "°";
            document.getElementById('yaw-value').textContent = yaw.toFixed(2) + "°";
          }
        } catch (e) {
          console.error('Data parse error:', e);
        }
      }
      
      // Animation loop
      function animate() {
        requestAnimationFrame(animate);
        
        // Apply rotations to cube
        if (isConnected) {
          cube.rotation.x = rotationX;
          cube.rotation.y = rotationY;
          cube.rotation.z = rotationZ;
        } else {
          // Idle animation when disconnected
          cube.rotation.x += 0.005;
          cube.rotation.y += 0.007;
        }
        
        renderer.render(scene, camera);
      }
      
      // Handle window resize
      function onWindowResize() {
        const canvas = document.getElementById('cubeCanvas');
        camera.aspect = canvas.clientWidth / canvas.clientHeight;
        camera.updateProjectionMatrix();
        renderer.setSize(canvas.clientWidth, canvas.clientHeight);
      }
      
      // Initialize everything
      function init() {
        initScene();
        initWebSocket();
        animate();
        window.addEventListener('resize', onWindowResize);
      }
      
      // Start the application
      window.onload = init;
    </script>
  </body>
  </html>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Client disconnected\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\nESP32 Motion-Controlled 3D Cube");
  Serial.println("===============================");

  // Initialize MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip!");
    while (1) {
      delay(1000);
      Serial.println("Please check MPU6050 connections and restart");
    }
  }
  
  // Configure sensor settings
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.println("Accelerometer range set to 8G");
  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.println("Gyro range set to 500 degrees/s");
  
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("Filter bandwidth set to 21Hz");
  
  Serial.println("MPU6050 initialized successfully!");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi. Please check credentials.");
    Serial.println("Resetting in 10 seconds...");
    delay(10000);
    ESP.restart();
  }
  
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
  
  Serial.println("System ready. Open http://" + WiFi.localIP().toString() + " in your browser");
}

void loop() {
  static uint32_t lastSensorUpdate = 0;
  
  server.handleClient();
  webSocket.loop();

  // Send sensor data at 20Hz
  if (millis() - lastSensorUpdate > 50) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    // Create JSON data package
    String json = "{\"ax\":" + String(a.acceleration.x, 2) + 
                  ",\"ay\":" + String(a.acceleration.y, 2) + 
                  ",\"az\":" + String(a.acceleration.z, 2) + 
                  ",\"gx\":" + String(g.gyro.x, 2) + 
                  ",\"gy\":" + String(g.gyro.y, 2) + 
                  ",\"gz\":" + String(g.gyro.z, 2) + "}";
    
    webSocket.broadcastTXT(json);
    lastSensorUpdate = millis();
  }
}