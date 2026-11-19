#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/RTDBHelper.h>
#include <DHT.h>
#include <IRremoteESP8266.h>
#include <ir_Coolix.h>

// ==================== WI-FI E FIREBASE ====================
#define WIFI_SSID       "IFPE/RECIFE"
#define WIFI_PASSWORD   "ifpe@082021"

#define DATABASE_URL    "https://mc2026-2-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define DATABASE_SECRET "R7IDULlJk4z07kZI9GiZOajiLprl6LIWNPxPBtGq"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ==================== PINOS ====================
const int pinoLDR = 34;
const int pinoLuz = 26;

#define PINO_DHT 27
#define TIPO_DHT DHT11
DHT dht(PINO_DHT, TIPO_DHT);

const uint16_t kIrLed = 4;
IRCoolixAC ac(kIrLed);

// ==================== CONFIGURAÇÕES ====================
// Histerese do LDR para controle da luz
const int ldrAcenderLuz = 2800;
const int ldrApagarLuz  = 3200;

const unsigned long intervaloSensores = 2000; // Lê sensores a cada 2s
const unsigned long intervaloFirebase = 5000; // Sincroniza Firebase a cada 5s

// ==================== VARIÁVEIS DE ESTADO ====================
unsigned long ultimoTempoSensores = 0;
unsigned long ultimoTempoFirebase = 0;

int   ldrBruto = 0;
float luminosidade = 0.0;
float temperatura = 0.0;
float umidade = 0.0;
bool  dhtValido = false;
bool  luzAcesa = false;

// Controle do último comando enviado ao ar-condicionado
bool ultimoAcLigado = false;
int  ultimaTempEnviada = -1;
bool primeiroEnvioAC = true;

// ==================== FUNÇÕES ====================
void gerenciarLuz(int valorLDR) {
  if (!luzAcesa && valorLDR < ldrAcenderLuz) {
    luzAcesa = true;
    digitalWrite(pinoLuz, HIGH);
  } else if (luzAcesa && valorLDR > ldrApagarLuz) {
    luzAcesa = false;
    digitalWrite(pinoLuz, LOW);
  }
}

void processarComandoAC(bool deveLigar, int tempDesejada) {
  tempDesejada = constrain(tempDesejada, (int)kCoolixTempMin, (int)kCoolixTempMax);

  bool mudouEstado = primeiroEnvioAC || (deveLigar != ultimoAcLigado);
  bool mudouTemp   = deveLigar && (tempDesejada != ultimaTempEnviada);

  if (!mudouEstado && !mudouTemp) {
    return; // Estado igual ao anterior -> não reenvia IR
  }

  if (deveLigar) {
    ac.on();
    ac.setMode(kCoolixCool);
    ac.setFan(kCoolixFanAuto);
    ac.setTemp((uint8_t)tempDesejada);
    ac.send();
    Serial.printf("[IR TX] AC LIGADO em %d °C\n", tempDesejada);
  } else {
    ac.off();
    ac.send();
    Serial.println("[IR TX] AC DESLIGADO");
  }

  ultimoAcLigado = deveLigar;
  ultimaTempEnviada = tempDesejada;
  primeiroEnvioAC = false;
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(pinoLDR, INPUT);
  pinMode(pinoLuz, OUTPUT);
  digitalWrite(pinoLuz, LOW);

  dht.begin();
  ac.begin();

  // Conexão Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  // Configuração do Firebase
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  Firebase.begin(&config, &auth);
}

// ==================== LOOP ====================
void loop() {
  unsigned long tempoAtual = millis();

  // 1. Leitura local dos sensores (a cada 2s)
  if (tempoAtual - ultimoTempoSensores >= intervaloSensores) {
    ultimoTempoSensores = tempoAtual;

    ldrBruto = analogRead(pinoLDR);
    luminosidade = (ldrBruto / 4095.0f) * 100.0f;
    gerenciarLuz(ldrBruto);

    float u = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(u) && !isnan(t)) {
      umidade = u;
      temperatura = t;
      dhtValido = true;
    } else {
      dhtValido = false;
      Serial.println("[DHT11] Erro de leitura!");
    }

    Serial.printf("LDR: %d (%.1f%%) | Luz: %s | Temp: %.1f C | Umid: %.1f%%\n",
                  ldrBruto, luminosidade, luzAcesa ? "ACESA" : "APAGADA",
                  temperatura, umidade);
  }

  // 2. Sincronização com o Firebase (a cada 5s)
  if (tempoAtual - ultimoTempoFirebase >= intervaloFirebase) {
    ultimoTempoFirebase = tempoAtual;

    if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
      // --- ENVIA SENSORES PARA O FIREBASE ---
      Firebase.setInt(fbdo, "/sensores/ldrBruto", ldrBruto);
      Firebase.setFloat(fbdo, "/sensores/luminosidade", luminosidade);
      Firebase.setBool(fbdo, "/sensores/luzAcesa", luzAcesa);

      if (dhtValido) {
        Firebase.setFloat(fbdo, "/sensores/temperatura", temperatura);
        Firebase.setFloat(fbdo, "/sensores/umidade", umidade);
      }

      // --- LÊ COMANDOS DO AR-CONDICIONADO NO FIREBASE ---
      bool acLigadoFB = false;
      int  tempAlvoFB = 22;

      bool leuEstado = Firebase.getBool(fbdo, "/ac/ligado", &acLigadoFB);
      bool leuTemp   = Firebase.getInt(fbdo, "/ac/temperatura", &tempAlvoFB);

      if (leuEstado && leuTemp) {
        processarComandoAC(acLigadoFB, tempAlvoFB);
      } else {
        Serial.printf("[Firebase] Erro ao ler /ac: %s\n", fbdo.errorReason().c_str());
      }
    }
  }
}