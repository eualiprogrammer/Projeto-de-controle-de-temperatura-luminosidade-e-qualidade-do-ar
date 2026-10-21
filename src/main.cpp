#include <Arduino.h>
#include <DHT.h>

#include <Arduino.h>
#include <DHT.h>

// --- Configurações do LDR ---
const int pinoLDR = 34; // Pino ADC1 usado pelo LDR

// --- Configurações do DHT11 ---
#define PINO_DHT 27     // Pino digital seguro conectado ao DHT11
#define TIPO_DHT DHT11  // Define o modelo do sensor (DHT11)

// Inicializa o objeto do sensor DHT
DHT dht(PINO_DHT, TIPO_DHT);

void setup() {
  Serial.begin(115200);
  
  // Configura o LDR e inicia o DHT11
  pinMode(pinoLDR, INPUT);
  dht.begin();
  
  Serial.println("Iniciando leitura do LDR e DHT11 no ESP32...");
}

void loop() {
  // O DHT11 é um sensor lento e precisa de pelo menos 2 segundos entre as leituras
  delay(2000);

  // 1. Leitura do LDR (0 a 4095)
  int valorLDR = analogRead(pinoLDR);
  float luminosidade = (valorLDR / 4095.0) * 100.0;

  // 2. Leitura do DHT11 (Umidade e Temperatura em Celsius)
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // --- Exibição do LDR ---
  Serial.print("LDR Bruto: ");
  Serial.print(valorLDR);
  Serial.print(" | Luminosidade: ");
  Serial.print(luminosidade, 1);
  Serial.print("% (");
  if (valorLDR < 1500) {
    Serial.print("Escuro");
  } else {
    Serial.print("Claro");
  }
  Serial.println(")");

  // --- Exibição do DHT11 ---
  // Verifica se houve falha na leitura do DHT11 (isnan = Is Not A Number)
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro: Falha ao ler o sensor DHT11! Verifique os fios.");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura, 1);
    Serial.print(" °C | Umidade: ");
    Serial.print(umidade, 1);
    Serial.println(" %");
  }

  Serial.println("--------------------------------------------------");
}