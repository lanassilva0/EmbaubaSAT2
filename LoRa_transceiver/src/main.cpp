#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

// ============================
// CONFIGURAÇÃO DE PINOS
// ============================

const int csPin = 5;     // Chip Select (NSS)
const int resetPin = 14; // RESET
const int irqPin = 2;    // DIO0

// ============================
// CONFIGURAÇÃO DE ENDEREÇOS
// ============================

// Endereçamento LoRa (igual ao receptor)
byte localAddress = 0x01; // Endereço do transmissor(Satélite)
byte destination = 0x0A;  // Endereço do receptor(Estacao base)
byte msgCount = 0;

// ============================
// CONFIGURAÇÃO DE ENVIO
// ============================
int intervalo = 5000; // Intervalo entre mensagens (5 segundos)
long ultimoEnvio = 0;

// ============================
// ESTRUTURA DE DADOS(OBDH)
// ============================

struct DadosOBDH
{

  float temp;
  float umi;
};

// ============================
// FUNÇÃO: Simula dados do OBDH
// ============================
DadosOBDH simularDadosOBDH()
{

  DadosOBDH dados;
  dados.temp = random(200, 300) / 10.0; // 20.0 a 30.0 ºC
  dados.umi = random(400, 800) / 10.0;  // 40.0 a 80.0 %
  return dados;
}

// ============================
// FUNÇÃO: Inicializa LoRa
// ============================

void Inicializar_LORA()
{

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(927E6))
  {
    Serial.println("Erro ao iniciar LoRa. Verifique os fios!");
    while (1)
      ;
  }

  // Parâmetros de compatíblidade com o protocolo padrão(Entre modelos diferentes de lora )
  LoRa.setSpreadingFactor(7);     // SF7 (igual ao padrão do E32)
  LoRa.setSignalBandwidth(125E3); // 125kHz (largura de banda padrão)
  LoRa.setCodingRate4(5);         // CR 4/5
  LoRa.enableCrc();               // Habilita CRC
  LoRa.setSyncWord(0x34);         // Sync word padrão do E32

  Serial.println("LoRa iniciado com sucesso!");
}

void enviarMensagem(const DadosOBDH &dados)
{
  // StaticJsonDocument<200> doc;
  StaticJsonDocument<128> doc;
  doc["temperatura"] = dados.temp;
  doc["umidade"] = dados.umi;
  doc["origem"] = "OBDH";

  String payload;
  serializeJson(doc, payload);

  // Enviar pacote
  LoRa.beginPacket();
  LoRa.write(destination);
  LoRa.write(localAddress);
  LoRa.write(msgCount++);
  LoRa.write(payload.length());
  LoRa.print(payload);
  LoRa.endPacket();

  Serial.println("Mensagem enviada ");
  // Serial.println(payload);
}

// ============================
// SETUP
// ============================
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Transmissor LoRa - OBDH Simulado");
  Inicializar_LORA();
}

void loop()
{
  long agora = millis();
  if (agora - ultimoEnvio > intervalo)
  {
    ultimoEnvio = agora;

    // Simula e empacota os dados
    DadosOBDH dados = simularDadosOBDH();

    enviarMensagem(dados); // Chama a função que já monta o JSON
  }
}