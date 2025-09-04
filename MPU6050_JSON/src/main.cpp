#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include "MPU9250.h"

MPU9250Setting setting;

void testeMPU6050()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    // while (1) {
    //  delay(10);
    // }
  }

  Serial.println("MPU6050 Found!");
  mpu_temp = mpu.getTemperatureSensor();
  mpu_temp->printSensorDetails();

  mpu_accel = mpu.getAccelerometerSensor();
  mpu_accel->printSensorDetails();

  mpu_gyro = mpu.getGyroSensor();
  mpu_gyro->printSensorDetails();
}

void lerMPU6050(JsonObject &leituraSensores)
{
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  Serial.print("Accel X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" m/s², ");
  Serial.print("Y: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" m/s², ");
  Serial.print("Z: ");
  Serial.print(accel.acceleration.z);
  Serial.println(" m/s²");

  Serial.print("Gyro  X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" rad/s, ");
  Serial.print("Y: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" rad/s, ");
  Serial.print("Z: ");
  Serial.print(gyro.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  leituraSensores["aceleracao"][0] = accel.acceleration.x;
  leituraSensores["aceleracao"][1] = accel.acceleration.y;
  leituraSensores["aceleracao"][2] = accel.acceleration.z;
  leituraSensores["giroscopio"][0] = gyro.gyro.x;
  leituraSensores["giroscopio"][1] = gyro.gyro.y;
  leituraSensores["giroscopio"][2] = gyro.gyro.z;
  leituraSensores["temperatura"] = temp.temperature;

  Serial.println();
  delay(1000);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  testeENS();

  if (!mpu.begin(0x69))
  {
    Serial.println("Cannot connect to MPU6050");
  }
}

long currentTime, lastTime;

void loop()
{
  StaticJsonDocument<512> jsonLeitura;

  JsonObject leituraSensores = jsonLeitura.to<JsonObject>();

  currentTime = millis();

  if (currentTime - lastTime > 2000)
  {
    lerMPU6050(leituraSensores);

    Serial.println();
    delay(1000);

    lastTime = millis();
  }
}