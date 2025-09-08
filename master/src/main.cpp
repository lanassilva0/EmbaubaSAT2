#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>

#define EPPROM_SIZE 512
#define EEPROM_ADDR 0

#define SS 18   // GPIO18 CS
#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)
#include "ScioSense_ENS160.h"
#include "MPU9250.h"
#include <MQUnifiedsensor.h>

#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0                 // Analog input 0 of your arduino
#define type "MQ-135"          // MQ135
#define ADC_Bit_Resolution 10  // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6 // RS / R0 = 3.6 ppm

int ArduinoLED = 13;

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
ScioSense_ENS160 ens160(ENS160_I2CADDR_0);
Adafruit_BMP280 bmp;
ScioSense_ENS160 ens;
// MPU9250 mpu;
MPU9250Setting setting;
Adafruit_MPU6050 mpu;

// Micro SD Card
//  * MicroSD VCC pin to ESP32 +5V
//  * MicroSD GND pin to ESP32 GND
//  * MicroSD MISO pin to ESP32 GPIO19
//  * MicroSD MOSI pin to ESP32 GPIO23
//  * MicroSD SCK pin to ESP32 GPIO18
//  * MicroSD CS pin to ESP32 GPIO5

const int MICROSD_PIN_CS = 5;    // Pino serial
const int MICROSD_PIN_MOSI = 23; // Pino serial
const int MICROSD_PIN_MISO = 19; // Pino serial
const int MICROSD_PIN_SCK = 18;  // Clock pin

File myFile;

void readFile(const char *path)
{
  myFile = SD.open(path);
  if (myFile)
  {
    Serial.printf("Lendo arquivo de %s\n", path);
    while (myFile.available())
    {
      Serial.write(myFile.read());
    }
    myFile.close();
  }
  else
  {
    Serial.println("Erro ao abrir test.txt");
  }
}

void writeFile(const char *path, const char *message)
{
  myFile = SD.open(path, FILE_WRITE);
  if (myFile)
  {
    Serial.printf("Escrevendo em %s ", path);
    myFile.println(message);
    myFile.close();
    Serial.println("concluído.");
  }
  else
  {
    Serial.println("Erro ao abrir o arquivo");
    Serial.println(path);
  }
}
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

// TESTE ENS - INICIO

void testeENS()
{

  Serial.begin(115200);

  while (!Serial)
  {
  }

  // Switch on LED for init
  pinMode(ArduinoLED, OUTPUT);
  digitalWrite(ArduinoLED, LOW);

  Serial.println("------------------------------------------------------------");
  Serial.println("ENS160 - Digital air quality sensor");
  Serial.println();
  Serial.println("Sensor readout in standard mode");
  Serial.println();
  Serial.println("------------------------------------------------------------");
  delay(1000);

  Serial.print("ENS160...");
  ens160.begin();
  Serial.println(ens160.available() ? "done." : "failed!");
  if (ens160.available())
  {
    // Print ENS160 versions
    Serial.print("\tRev: ");
    Serial.print(ens160.getMajorRev());
    Serial.print(".");
    Serial.print(ens160.getMinorRev());
    Serial.print(".");
    Serial.println(ens160.getBuild());

    Serial.print("\tStandard mode ");
    Serial.println(ens160.setMode(ENS160_OPMODE_STD) ? "done." : "failed!");
  }
}

void lerENS160(JsonObject &leituraSensores)
{
  if (ens160.available())
  {
    ens160.measure(true);
    ens160.measureRaw(true);

    Serial.print("AQI: ");
    Serial.print(ens160.getAQI());
    Serial.println("\t");
    Serial.print("TVOC: ");
    Serial.print(ens160.getTVOC());
    Serial.println("ppb\t");
    Serial.print("eCO2: ");
    Serial.print(ens160.geteCO2());
    Serial.println("ppm\t");
    Serial.print("R HP0: ");
    Serial.print(ens160.getHP0());
    Serial.println("Ohm\t");
    Serial.print("R HP1: ");
    Serial.print(ens160.getHP1());
    Serial.println("Ohm\t");
    Serial.print("R HP2: ");
    Serial.print(ens160.getHP2());
    Serial.println("Ohm\t");
    Serial.print("R HP3: ");
    Serial.print(ens160.getHP3());
    Serial.println("Ohm");

    leituraSensores["aqi"] = ens160.getAQI();
    leituraSensores["tvoc"] = ens160.getTVOC();
    leituraSensores["eco2"] = ens160.geteCO2();
    leituraSensores["hp0"] = ens160.getHP0();
    leituraSensores["hp1"] = ens160.getHP1();
    leituraSensores["hp2"] = ens160.getHP2();
    leituraSensores["hp3"] = ens160.getHP3();
  }
  delay(1000);
}

// TESTE ENS - FIM

// Varredura de endereços - INÍCIO
void varreduraDeEnderecos()
{
  Serial.println(" Iniciando varredura I2C...");

  byte count = 0;

  for (byte address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
    {
      Serial.print(" Dispositivo encontrado no endereço: 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(" (decimal ");
      Serial.print(address);
      Serial.println(")");
      count++;
      delay(5);
    }
  }

  if (count == 0)
  {
    Serial.println(" Nenhum dispositivo I2C encontrado.");
  }
  else
  {
    Serial.print(" Total de dispositivos encontrados: ");
    Serial.println(count);
  }
}
// Varredura de endereços - FIM

// BMP - INÍCIO
void lerBMP280(JsonObject &leituraSensores)
{
  Serial.println("BMP280:");
  Serial.print("Temperatura : ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressao : ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("Altitude : ");
  Serial.print(bmp.readAltitude());
  Serial.println(" m");
  Serial.println(" ");

  leituraSensores["temperatura"] = bmp.readTemperature();
  leituraSensores["pressao"] = bmp.readPressure();
  leituraSensores["altitude"] = bmp.readAltitude(1013.25);
}
// BMP - FIM

/*
// MPU6500 - INÍCIO
void lerMPU6500()
{

  if (!mpu.setup(0x68, setting))
  { // change to your own address
    Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
    delay(5000);
  }

  if (!mpu.update())
  {
    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(mpu.getYaw(), 2);
    Serial.print(", ");
    Serial.print(mpu.getPitch(), 2);
    Serial.print(", ");
    Serial.println(mpu.getRoll(), 2);
  }

  if (mpu.update())
  {
    Serial.print("Accel X: ");
    Serial.print(mpu.getAccX());
    Serial.print(" | Y: ");
    Serial.print(mpu.getAccY());
    Serial.print(" | Z: ");
    Serial.println(mpu.getAccZ());
  }
}
void testeMPU6500()
{
  Serial.begin(115200);
  Serial.flush();
  Wire.begin();
  delay(2000);

  scan_mpu();

  if (device_count == 0)
  {
    Serial.println("No device found on I2C bus. Please check your hardware connection");
    while (1)
      ;
  }

  // check WHO_AM_I address of MPU
  for (uint8_t i = 0; i < device_count; ++i)
  {
    Serial.print("I2C address 0x");
    Serial.print(addrs[i], HEX);
    byte ca = readByte(addrs[i], WHO_AM_I_MPU9250);
    if (ca == MPU9250_WHOAMI_DEFAULT_VALUE)
    {
      Serial.println(" is MPU9250 and ready to use");
    }
    else if (ca == MPU9255_WHOAMI_DEFAULT_VALUE)
    {
      Serial.println(" is MPU9255 and ready to use");
    }
    else if (ca == MPU6500_WHOAMI_DEFAULT_VALUE)
    {
      Serial.println(" is MPU6500 and ready to use");
    }
    else
    {
      Serial.println(" is not MPU series");
      Serial.print("WHO_AM_I is ");
      Serial.println(ca, HEX);
      Serial.println("Please use correct device");
    }
    static constexpr uint8_t AK8963_ADDRESS{0x0C}; //  Address of magnetometer
    static constexpr uint8_t AK8963_WHOAMI_DEFAULT_VALUE{0x48};
    byte cb = readByte(AK8963_ADDRESS, AK8963_WHO_AM_I);
    if (cb == AK8963_WHOAMI_DEFAULT_VALUE)
    {
      Serial.print("AK8963 (Magnetometer) is ready to use");
    }
    else
    {
      Serial.print("AK8963 (Magnetometer) was not found");
    }
  }
}
// MPU6500 - FIM
*/

// MPU6050 - INÍCIO
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
// MPU6050 - FIM

// MQ135 - INICIO
void testeMQ135()
{
  Serial.begin(9600); // Init serial port

  // Set math model to calculate the PPM concentration and the value of constants
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setA(102.2);
  MQ135.setB(-2.473); // Configure the equation to to calculate NH4 concentration
}

void lerMQ135()
{
  MQ135.update();                            // Update data, the arduino will read the voltage from the analog pin
  float correctionFactor = 0;                // Optional environmental correction
  MQ135.readSensor(false, correctionFactor); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  MQ135.serialDebug();                       // Will print the table on the serial port
  delay(500);                                // Sampling frequency

  MQ135.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++)
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0))
  {
    Serial.println("Warning: Connection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR0 == 0)
  {
    Serial.println("Warning: Connection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }
  /*****************************  MQ CAlibration ********************************************/
  MQ135.serialDebug(true);
}
// MQ135 - FIM

void setup()
{

  EEPROM.begin(EPPROM_SIZE);
  Serial.begin(115200);
  delay(1000);
  Wire.begin(21, 22);

  varreduraDeEnderecos();
  testeMPU6050();
  testeENS();
  // testeMQ135();

  if (!mpu.begin(0x69))
  {
    Serial.println("Cannot connect to MPU6050");
  }

  if (!bmp.begin())
  {
    Serial.println("Cannot connect to BMP280");
    // while (1)
  }

  if (!SD.begin(MICROSD_PIN_CS))
  {
    Serial.println("Erro ao iniciar SD Card");
  }
  else
  {
    Serial.println("Cartão SD inicializado");

    writeFile("/test.txt", "Testando escrita no cartão SD");
    readFile("/test.txt");
  }
}

long currentTime, lastTime;

void saveEEPROM(const StaticJsonDocument<512> &json)
{
  char jsonStr[512];
  serializeJson(json, jsonStr); // converte o JSON para string
  for (int i = 0; i < strlen(jsonStr); i++)
  {
    EEPROM.write(EEPROM_ADDR + i, jsonStr[i]);
  }
  EEPROM.commit();
}

void loop()
{
  StaticJsonDocument<512> jsonLeitura;

  JsonObject leituraSensores = jsonLeitura.to<JsonObject>();

  currentTime = millis();

  if (currentTime - lastTime > 2000)
  {

    lerBMP280(leituraSensores);
    Serial.println("MPU6050 na protoboard:");
    lerMPU6050(leituraSensores);
    Serial.println("ENS160 na placa:");
    lerENS160(leituraSensores);
    // lerMQ135();

    saveEEPROM(jsonLeitura);

    Serial.println();
    delay(1000);

    lastTime = millis();
  }
}