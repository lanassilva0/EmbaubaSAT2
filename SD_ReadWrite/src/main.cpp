#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

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

void setup()
{
  // EEPROM.begin(EPPROM_SIZE);
  Serial.begin(115200);
  delay(1000);

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

void loop()
{
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}