#include <Arduino.h>
#include <RadioLib.h>
#include "lorawan_config.h"

// Pin SX1262 sulla Heltec WiFi LoRa 32 V4: NSS, DIO1(=DIO0 nella variant), RESET, BUSY
#define LORA_NSS   8
#define LORA_DIO1  14
#define LORA_RST   12
#define LORA_BUSY  13

// GPIO che alimenta i periferici on-board (OLED, ecc.) su molte Heltec V3/V4
#define VEXT_PIN   36

SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);

const LoRaWANBand_t Region = LORAWAN_REGION;
const uint8_t subBand = LORAWAN_SUBBAND;

LoRaWANNode node(&radio, &Region, subBand);

uint64_t joinEUI = RADIOLIB_LORAWAN_JOIN_EUI;
uint64_t devEUI  = RADIOLIB_LORAWAN_DEV_EUI;
uint8_t appKey[] = { RADIOLIB_LORAWAN_APP_KEY };
uint8_t nwkKey[] = { RADIOLIB_LORAWAN_NWK_KEY };

const uint32_t UPLINK_INTERVAL_MS = 60000UL; // invia ogni 60s

void printResult(const char *label, int16_t state) {
  Serial.print(label);
  Serial.print(state == RADIOLIB_ERR_NONE ? F(" ok") : F(" errore, codice "));
  if (state != RADIOLIB_ERR_NONE) Serial.println(state);
  else Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Accendi Vext (OLED e periferiche); commenta se non ti serve
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW); // LOW = acceso su Heltec V3/V4

  Serial.println(F("\n[SX1262] Inizializzazione radio..."));
  int16_t state = radio.begin();
  printResult("[SX1262] begin()", state);
  if (state != RADIOLIB_ERR_NONE) {
    while (true) delay(1000);
  }

  Serial.println(F("[LoRaWAN] Join OTAA in corso..."));
  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  state = node.activateOTAA();
  while (state != RADIOLIB_LORAWAN_NEW_SESSION) {
    printResult("[LoRaWAN] activateOTAA()", state);
    Serial.println(F("Nuovo tentativo tra 15s..."));
    delay(15000);
    state = node.activateOTAA();
  }
  Serial.println(F("[LoRaWAN] Join riuscito!"));

  node.setADR(true); // lascia gestire data rate/potenza alla rete
}

void loop() {
  static uint32_t lastUplink = 0;

  if (millis() - lastUplink >= UPLINK_INTERVAL_MS) {
    lastUplink = millis();

    // Esempio: payload di 4 byte (sostituisci con i tuoi dati reali, es. sensori)
    uint8_t payload[4];
    uint32_t counter = millis() / 1000;
    payload[0] = (counter >> 24) & 0xFF;
    payload[1] = (counter >> 16) & 0xFF;
    payload[2] = (counter >> 8) & 0xFF;
    payload[3] = counter & 0xFF;

    uint8_t downlink[256];
    size_t downlinkLen = sizeof(downlink);

    Serial.println(F("[LoRaWAN] Invio uplink..."));
    int16_t state = node.sendReceive(payload, sizeof(payload), 1, downlink, &downlinkLen);

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("[LoRaWAN] Uplink inviato, nessun downlink."));
    } else if (state > 0) {
      Serial.print(F("[LoRaWAN] Uplink inviato, downlink ricevuto ("));
      Serial.print(downlinkLen);
      Serial.println(F(" byte)"));
    } else {
      printResult("[LoRaWAN] sendReceive()", state);
    }
  }

  delay(100);
}
