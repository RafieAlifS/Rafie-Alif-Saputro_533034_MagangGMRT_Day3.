//Rafie Alif Saputro
//24/533034/PA/22562


#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

Adafruit_MPU6050 mpu;

// Servo objects
Servo servo1, servo2, servo3, servo4, servo5;

// Servo pins
#define SERVO1_PIN 13
#define SERVO2_PIN 12
#define SERVO3_PIN 14
#define SERVO4_PIN 26
#define SERVO5_PIN 25

// PIR pin
#define PIR_PIN 27

// Initial servo position
int initialPos = 90; // center = 90° (0 offset)

// State
unsigned long lastYawMove = 0;
bool yawReturnPending = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Setup MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  // Attach servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo5.attach(SERVO5_PIN);

  // Move to initial position
  servo1.write(initialPos);
  servo2.write(initialPos);
  servo3.write(initialPos);
  servo4.write(initialPos);
  servo5.write(initialPos);

  pinMode(PIR_PIN, INPUT);

  Serial.println("System Ready!");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y*a.acceleration.y + a.acceleration.z*a.acceleration.z)) * 180 / PI;
  float yaw = g.gyro.z; // Simplified — actual yaw integration is complex

  bool motionDetected = digitalRead(PIR_PIN);

  // Roll Control (Servo 1 & 2)
  int rollPos = map((int)roll, -90, 90, 0, 180);
  int oppositeRollPos = 180 - rollPos;
  servo1.write(oppositeRollPos);
  servo2.write(oppositeRollPos);

  // Pitch Control (Servo 3 & 4)
  int pitchPos = map((int)pitch, -90, 90, 0, 180);
  servo3.write(pitchPos);
  servo4.write(pitchPos);

  // Yaw control
  if (abs(yaw) > 5) { // Small deadzone
    int yawPos = constrain(initialPos + (yaw / 2), 0, 180);
    servo5.write(yawPos);
    lastYawMove = millis();
    yawReturnPending = true;
  }

  // Return to 0
  if (yawReturnPending && millis() - lastYawMove > 1000) {
    servo5.write(initialPos);
    yawReturnPending = false;
  }

  //PIR Sensor
  if (motionDetected) {
    Serial.println("Motion detected!");
    for (int i = 1; i <= 5; i++) {
      getServo(i).write(135); // move to +45° from center
    }
    delay(1000);
    for (int i = 1; i <= 5; i++) {
      getServo(i).write(initialPos);
    }
  }

  delay(100);
}

// Helper function to get servo by index
Servo& getServo(int index) {
  switch(index) {
    case 1: return servo1;
    case 2: return servo2;
    case 3: return servo3;
    case 4: return servo4;
    case 5: return servo5;
    default: return servo1;
  }
}
