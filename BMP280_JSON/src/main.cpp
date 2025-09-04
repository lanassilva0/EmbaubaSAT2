#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

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

void setup()
{
  Serial.begin(115200);
  delay(1000);

  if (!bmp.begin())
  {
    Serial.println("Cannot connect to BMP280");
    // while (1)
  }
}

void loop()
{
  StaticJsonDocument<512> jsonLeitura;

  JsonObject leituraSensores = jsonLeitura.to<JsonObject>();

  currentTime = millis();

  if (currentTime - lastTime > 2000)
  {

    lerBMP280(leituraSensores);

    Serial.println();
    delay(1000);

    lastTime = millis();
  }
}
