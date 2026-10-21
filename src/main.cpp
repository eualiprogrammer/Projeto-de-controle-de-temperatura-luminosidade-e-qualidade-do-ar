#include <Arduino.h>

// Definição do pino onde o LDR está conectado
const int pinoLDR = 34; 

void setup() {
  //BLOCO DO LDR

  // Inicia a comunicação serial com velocidade de 9600 bps
  Serial.begin(9600);
  
  // Configura o pino do LDR como entrada
  pinMode(pinoLDR, INPUT);
  
  Serial.println("Iniciando leitura do LDR no ESP32...");

  //FIM DO CODIGO LDR
}

void loop() {

  // BLOCO DO LDR

  // Lê o valor analógico (0 a 4095)
  int valorLDR = analogRead(pinoLDR);
  
  // Converte a leitura para porcentagem de luminosidade (opcional)
  float porcentagem = (valorLDR / 4095.0) * 100.0;

  // Exibe os valores no Monitor Serial
  Serial.print("Valor Bruto: ");
  Serial.print(valorLDR);
  Serial.print(" | Luminosidade: ");
  Serial.print(porcentagem, 1); // 1 casa decimal
  Serial.println("%");

  // Exemplo de lógica simples para acionar algo se estiver escuro
  if (valorLDR < 1500) {
    Serial.println("Status: Ambiente Escuro");
  } else {
    Serial.println("Status: Ambiente Claro");
  }

  Serial.println("-----------------------------------");
  
  // Aguarda 1 segundo antes da próxima leitura
  delay(1000);

  //FIM DO BLOCO LDR

}

