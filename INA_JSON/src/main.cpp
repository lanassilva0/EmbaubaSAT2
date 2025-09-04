#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

const float capacidadeBateria = 2000.0;
const float tensaoMax = 4.2;
const float tensaoMin = 3.0;

float corrente = 0.0, potencia = 0.0, tensao = 0.0;
float quedaTensao = 0.0, tensaoEntrada = 0.0;

float SoC = 100.0;
float cargaConsumida = 0.0;
float consumoMedio = 0.0;
float tempoRestante = 0.0;

unsigned long ultimaAtualizacao = 0;
const int numLeituras = 10;
float historicoCorrente[numLeituras];
int indiceLeitura = 0;
bool historicoCompleto = false;

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!ina219.begin())
  {
    Serial.println("INA219 não encontrado");
    while (1)
      ;
  }

  ina219.setCalibration_32V_2A();

  for (int i = 0; i < numLeituras; i++)
  {
    historicoCorrente[i] = 0.0;
  }

  ultimaAtualizacao = millis();
}

void imprimirDados()
{
  Serial.println("monitoramento da bateria do satélite");

  Serial.print("tensão: ");
  Serial.print(tensaoEntrada, 2);
  Serial.println(" V");

  Serial.print("corrente: ");
  Serial.print(abs(corrente), 1);
  Serial.println(" mA");

  Serial.print("potência: ");
  Serial.print(potencia, 1);
  Serial.println(" mW");

  Serial.print("SoC: ");
  Serial.print(SoC, 1);
  Serial.println(" %");

  Serial.print("carga consumida: ");
  Serial.print(cargaConsumida, 1);
  Serial.println(" mAh");

  Serial.print("tempo restante: ");
  if (!historicoCompleto || consumoMedio <= 0.1)
  {
    Serial.println("calculando...");
  }
  else
  {
    int horas = (int)tempoRestante;
    int minutos = (int)((tempoRestante - horas) * 60);
    Serial.print(horas);
    Serial.print(" horas ");
    Serial.print(minutos);
    Serial.println(" minutos");
  }

  Serial.println();
}

void loop()
{
  tensao = ina219.getBusVoltage_V();
  quedaTensao = ina219.getShuntVoltage_mV() / 1000.0;
  corrente = ina219.getCurrent_mA();
  potencia = ina219.getPower_mW();
  tensaoEntrada = tensao + quedaTensao;

  historicoCorrente[indiceLeitura] = abs(corrente);
  indiceLeitura++;

  if (indiceLeitura >= numLeituras)
  {
    indiceLeitura = 0;
    historicoCompleto = true;
  }

  consumoMedio = 0.0;
  int leiturasValidas = historicoCompleto ? numLeituras : indiceLeitura;

  for (int i = 0; i < leiturasValidas; i++)
  {
    consumoMedio += historicoCorrente[i];
  }

  consumoMedio = (consumoMedio / leiturasValidas);

  unsigned long agora = millis();
  float deltaT = (agora - ultimaAtualizacao) / 3600000.0;
  ultimaAtualizacao = agora;

  cargaConsumida = cargaConsumida + abs(corrente) * deltaT;
  SoC = constrain(100.0 - ((cargaConsumida / capacidadeBateria) * 100.0), 0.0, 100.0);

  if (historicoCompleto && consumoMedio > 0.1)
  {
    float cargaRestante = capacidadeBateria * (SoC / 100.0);
    tempoRestante = cargaRestante / consumoMedio;
  }
  else
  {
    tempoRestante = 0.0;
  }

  imprimirDados();
  delay(3000);
}