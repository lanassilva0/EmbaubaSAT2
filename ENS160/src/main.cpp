#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "ScioSense_ENS160.h"

ScioSense_ENS160 ens160(ENS160_I2CADDR_0);
ScioSense_ENS160 ens;

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

void setup()
{
  Serial.begin(115200);
  delay(1000);

  testeENS();
}

void loop()
{
  StaticJsonDocument<512> jsonLeitura;

  JsonObject leituraSensores = jsonLeitura.to<JsonObject>();

  currentTime = millis();

  if (currentTime - lastTime > 2000)
  {
    lerENS160(leituraSensores);

    Serial.println();
    delay(1000);

    lastTime = millis();
  }
}