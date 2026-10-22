#include <Arduino.h>
#include <assert.h>
#include <DHT.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

const int pinoLDR = 34;

#define PINO_DHT 27
#define TIPO_DHT DHT11
DHT dht(PINO_DHT, TIPO_DHT);

const uint16_t kIrLed = 4;
IRsend irsend(kIrLed);

const uint64_t kCodeLigar18C = 0xB23F10;
const uint64_t kCodeDesligar = 0xB27BE0;
const uint64_t kCodeSubir19C = 0xB23F30;
const uint64_t kCodeDescer18C = 0xB23F10;

uint16_t rawLigar[199] = {4420, 4364, 566, 1572, 568, 526, 568, 1570, 568, 1598, 566, 502, 568, 526, 568, 1572, 566, 528, 568, 502, 568, 1598, 566, 500, 568, 526, 568, 1572, 566, 1598, 566, 500, 568, 1596, 568, 500, 568, 526, 568, 1572, 566, 1598, 566, 1598, 568, 1572, 568, 1598, 566, 1572, 568, 1596, 568, 1572, 568, 526, 568, 502, 568, 526, 568, 500, 568, 528, 568, 500, 568, 526, 568, 500, 568, 502, 594, 1572, 568, 528, 566, 502, 566, 504, 568, 528, 568, 1572, 568, 1598, 566, 1598, 568, 500, 568, 1598, 568, 1572, 566, 1598, 568, 1570, 568, 5172, 4402, 4364, 566, 1572, 566, 528, 566, 1572, 568, 1598, 566, 502, 568, 526, 568, 1570, 568, 526, 566, 502, 568, 1598, 566, 502, 568, 526, 566, 1574, 566, 1598, 564, 504, 568, 1596, 568, 500, 594, 500, 566, 1600, 568, 1570, 568, 1598, 566, 1572, 568, 1596, 568, 1572, 568, 1596, 568, 1598, 568, 500, 568, 526, 568, 502, 566, 500, 568, 526, 568, 502, 568, 526, 568, 500, 568, 526, 568, 1572, 566, 526, 568, 500, 568, 526, 568, 502, 568, 1598, 568, 1570, 568, 1598, 568, 500, 568, 1598, 566, 1574, 568, 1596, 568, 1598, 568};

uint16_t rawDesligar[199] = {4422, 4364, 566, 1572, 566, 528, 566, 1572, 566, 1600, 566, 502, 568, 528, 566, 1572, 566, 528, 566, 502, 566, 1598, 566, 502, 566, 534, 562, 1574, 566, 1598, 566, 502, 566, 1598, 566, 502, 566, 1600, 566, 1572, 566, 1600, 566, 1598, 566, 502, 566, 1600, 566, 1572, 566, 1598, 566, 502, 568, 528, 566, 502, 566, 528, 568, 1572, 566, 528, 566, 502, 568, 1598, 566, 1574, 564, 1600, 566, 502, 566, 528, 566, 502, 566, 528, 566, 502, 566, 528, 566, 502, 566, 502, 592, 1572, 566, 1598, 566, 1576, 562, 1598, 566, 1574, 566, 5174, 4400, 4364, 566, 1572, 566, 528, 568, 1572, 566, 1598, 568, 500, 568, 528, 568, 1572, 566, 526, 568, 502, 568, 1598, 568, 502, 568, 526, 568, 1572, 568, 1598, 566, 502, 568, 1598, 568, 500, 594, 1572, 568, 1598, 568, 1570, 568, 1598, 566, 502, 568, 1598, 568, 1570, 568, 1598, 568, 526, 568, 502, 568, 500, 568, 528, 566, 1572, 568, 526, 568, 500, 568, 1598, 568, 1598, 568, 1572, 566, 526, 568, 500, 568, 500, 568, 526, 568, 500, 568, 526, 568, 500, 568, 526, 568, 1572, 568, 1598, 566, 1572, 568, 1596, 568, 1598, 568};

uint16_t rawSubir[199] = {4420, 4362, 568, 1570, 568, 526, 568, 1570, 568, 1596, 568, 500, 568, 526, 568, 1570, 568, 526, 568, 500, 568, 1598, 568, 500, 568, 526, 568, 1572, 566, 1598, 568, 500, 568, 1596, 568, 500, 568, 526, 568, 1570, 594, 1572, 568, 1596, 568, 1572, 568, 1596, 568, 1572, 568, 1596, 568, 1598, 568, 500, 568, 528, 568, 500, 568, 500, 568, 528, 568, 500, 568, 528, 568, 500, 568, 1598, 566, 1572, 566, 528, 568, 500, 568, 526, 568, 500, 568, 1598, 566, 1572, 568, 526, 568, 500, 568, 1598, 568, 1572, 566, 1598, 568, 1598, 568, 5172, 4376, 4362, 568, 1570, 568, 526, 568, 1596, 568, 1570, 568, 526, 568, 500, 568, 1596, 568, 500, 570, 500, 594, 1570, 568, 526, 570, 500, 568, 1596, 568, 1570, 568, 526, 568, 1570, 568, 526, 570, 500, 568, 1596, 568, 1570, 568, 1596, 568, 1572, 570, 1594, 568, 1596, 568, 1570, 568, 1596, 568, 500, 568, 526, 568, 500, 568, 526, 568, 500, 568, 500, 594, 500, 568, 500, 568, 1596, 568, 1596, 568, 500, 568, 500, 568, 526, 568, 500, 568, 1596, 568, 1596, 568, 500, 570, 500, 568, 1596, 568, 1598, 568, 1570, 568, 1596, 568};

uint16_t rawDescer[199] = {4422, 4364, 566, 1572, 566, 528, 568, 1572, 566, 1598, 566, 502, 568, 528, 568, 1572, 566, 528, 566, 502, 566, 1598, 566, 502, 566, 528, 568, 1572, 566, 1600, 566, 502, 568, 1598, 566, 506, 562, 528, 568, 1572, 592, 1572, 566, 1598, 566, 1572, 566, 1598, 566, 1572, 566, 1598, 566, 1598, 566, 502, 568, 528, 566, 502, 568, 502, 566, 528, 568, 502, 566, 528, 566, 502, 566, 528, 568, 1572, 566, 528, 566, 502, 568, 528, 566, 502, 566, 1600, 566, 1572, 566, 1598, 566, 502, 568, 1598, 566, 1572, 566, 1600, 566, 1598, 566, 5174, 4374, 4364, 566, 1572, 566, 528, 568, 1598, 566, 1572, 566, 528, 566, 502, 566, 1598, 566, 502, 566, 502, 594, 1572, 566, 528, 566, 502, 566, 1598, 566, 1572, 566, 528, 568, 1572, 566, 528, 566, 502, 566, 1598, 566, 1572, 566, 1598, 566, 1572, 566, 1598, 566, 1598, 566, 1572, 566, 1598, 566, 502, 568, 528, 568, 502, 566, 528, 566, 502, 568, 502, 594, 502, 566, 502, 568, 528, 566, 1572, 568, 528, 566, 502, 566, 528, 566, 502, 568, 1598, 566, 1574, 566, 1598, 566, 502, 568, 1598, 566, 1598, 566, 1572, 566, 1598, 566};

#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 10;
#else
const uint16_t kRecvPin = 14;
#endif

const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;

#if DECODE_AC
const uint8_t kTimeout = 50;
#else
const uint8_t kTimeout = 15;
#endif

const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance;
#define LEGACY_TIMING_INFO false

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;

unsigned long ultimoTempoSensores = 0;
const unsigned long intervaloSensores = 2000;

unsigned long ultimoTempoEnvioIR = 0;
const unsigned long intervaloEnvioIR = 5000;
uint8_t etapaEnvioIR = 0;

void setup() {
#if defined(ESP8266)
  Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
#elif ARDUINO_USB_CDC_ON_BOOT
  Serial.begin(kBaudRate);
#else
  Serial.begin(kBaudRate, SERIAL_8N1);
#endif

  while (!Serial)
    delay(50);

  assert(irutils::lowLevelSanityCheck() == 0);

  irsend.begin();

  Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);

#if DECODE_HASH
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif
  irrecv.setTolerance(kTolerancePercentage);
  irrecv.enableIRIn();

  pinMode(pinoLDR, INPUT);
  dht.begin();
}

void loop() {
  if (irrecv.decode(&results)) {
    uint32_t now = millis();
    Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);

    if (results.overflow)
      Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);

    Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_STR "\n");

    if (kTolerancePercentage != kTolerance)
      Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);

    Serial.print(resultToHumanReadableBasic(&results));

    String description = IRAcUtils::resultAcToString(&results);
    if (description.length()) Serial.println(D_STR_MESGDESC ": " + description);

    yield();

#if LEGACY_TIMING_INFO
    Serial.println(resultToTimingInfo(&results));
    yield();
#endif

    Serial.println(resultToSourceCode(&results));
    Serial.println();
    yield();
  }

  unsigned long tempoAtual = millis();

  if (tempoAtual - ultimoTempoEnvioIR >= intervaloEnvioIR) {
    ultimoTempoEnvioIR = tempoAtual;
    irrecv.pause();

    switch (etapaEnvioIR) {
      case 0:
        Serial.println("Enviando IR: Ligar AC (18C) - COOLIX 0xB23F10");
        irsend.sendCOOLIX(kCodeLigar18C, kCoolixBits);
        break;
      case 1:
        Serial.println("Enviando IR: Seta Subir (19C) - COOLIX 0xB23F30");
        irsend.sendCOOLIX(kCodeSubir19C, kCoolixBits);
        break;
      case 2:
        Serial.println("Enviando IR: Seta Descer (18C) - COOLIX 0xB23F10");
        irsend.sendCOOLIX(kCodeDescer18C, kCoolixBits);
        break;
      case 3:
        Serial.println("Enviando IR: Desligar AC - COOLIX 0xB27BE0");
        irsend.sendCOOLIX(kCodeDesligar, kCoolixBits);
        break;
    }

    irrecv.resume();
    etapaEnvioIR = (etapaEnvioIR + 1) % 4;
  }

  if (tempoAtual - ultimoTempoSensores >= intervaloSensores) {
    ultimoTempoSensores = tempoAtual;

    int valorLDR = analogRead(pinoLDR);
    float porcentagem = (valorLDR / 4095.0) * 100.0;
    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature();

    Serial.print("LDR Bruto: ");
    Serial.print(valorLDR);
    Serial.print(" | Luminosidade: ");
    Serial.print(porcentagem, 1);
    Serial.print("% (");
    if (valorLDR < 1500) {
      Serial.print("Ambiente Escuro");
    } else {
      Serial.print("Ambiente Claro");
    }
    Serial.print(") | ");

    if (isnan(umidade) || isnan(temperatura)) {
      Serial.println("DHT11: Erro de leitura!");
    } else {
      Serial.print("Temp: ");
      Serial.print(temperatura, 1);
      Serial.print(" °C | Umid: ");
      Serial.print(umidade, 1);
      Serial.println(" %");
    }
    Serial.println("--------------------------------------------------");
  }
}