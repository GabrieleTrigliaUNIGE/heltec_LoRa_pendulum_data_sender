#include <Arduino.h>
#include <RadioLib.h>
#include <U8g2lib.h>
#include "lorawan_config.h"

// Pin SX1262 sulla Heltec WiFi LoRa 32 V4: NSS, DIO1(=DIO0 nella variant), RESET, BUSY
#define LORA_NSS   8
#define LORA_DIO1  14
#define LORA_RST   12
#define LORA_BUSY  13

// GPIO che alimenta i periferici on-board (OLED, ecc.) su molte Heltec V3/V4
#define VEXT_PIN   36

// OLED I2C software, stessi pin della variant (SCL_OLED=18, SDA_OLED=17, RST_OLED=21)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 17, /* reset=*/ 21);

// Scrive fino a 4 righe di testo sul display, cancellando quello precedente
void displayLines(const char *l1, const char *l2 = "", const char *l3 = "", const char *l4 = "") {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 10, l1);
  u8g2.drawStr(0, 24, l2);
  u8g2.drawStr(0, 38, l3);
  u8g2.drawStr(0, 52, l4);
  u8g2.sendBuffer();
}

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
  delay(100); // dai tempo all'OLED di alimentarsi prima di inizializzarlo

  u8g2.begin();
  displayLines("Avvio...", "Init display OK");

  Serial.println(F("\n[SX1262] Inizializzazione radio..."));
  displayLines("Init radio SX1262...");
  int16_t state = radio.begin();
  printResult("[SX1262] begin()", state);
  if (state != RADIOLIB_ERR_NONE) {
    char buf[32];
    snprintf(buf, sizeof(buf), "radio.begin() err %d", state);
    displayLines("ERRORE RADIO", buf);
    while (true) delay(1000);
  }
  displayLines("Radio OK", "Avvio join OTAA...");

  Serial.println(F("[LoRaWAN] Join OTAA in corso..."));
  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  uint8_t attempt = 0;
  state = node.activateOTAA();
  while (state != RADIOLIB_LORAWAN_NEW_SESSION) {
    attempt++;
    printResult("[LoRaWAN] activateOTAA()", state);
    Serial.println(F("Nuovo tentativo tra 15s..."));

    char line1[24], line2[24], line3[24];
    snprintf(line1, sizeof(line1), "Join tentativo #%u", attempt);
    snprintf(line2, sizeof(line2), "Errore: %d", state);
    snprintf(line3, sizeof(line3), "DevEUI:%08lX", (uint32_t)(devEUI & 0xFFFFFFFF));
    displayLines(line1, line2, line3, "Retry tra 15s...");

    delay(15000);
    state = node.activateOTAA();
  }
  Serial.println(F("[LoRaWAN] Join riuscito!"));
  displayLines("JOIN OK!", "In attesa invii...");

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

    Serial.print(F("[LoRaWAN] Payload inviato (hex): "));
    for (size_t i = 0; i < sizeof(payload); i++) {
      if (payload[i] < 0x10) Serial.print('0');
      Serial.print(payload[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    Serial.println(F("[LoRaWAN] Invio uplink..."));
    displayLines("Invio uplink...", "", "", "");
    int16_t state = node.sendReceive(payload, sizeof(payload), 1, downlink, &downlinkLen);

    char line1[24], line2[24];
    snprintf(line1, sizeof(line1), "Uptime: %lus", counter);

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("[LoRaWAN] Uplink inviato, nessun downlink."));
      snprintf(line2, sizeof(line2), "Uplink OK, no downlink");
    } else if (state > 0) {
      Serial.print(F("[LoRaWAN] Uplink inviato, downlink ricevuto ("));
      Serial.print(downlinkLen);
      Serial.println(F(" byte)"));
      snprintf(line2, sizeof(line2), "Uplink OK, DL %u byte", (unsigned)downlinkLen);
    } else {
      printResult("[LoRaWAN] sendReceive()", state);
      snprintf(line2, sizeof(line2), "Errore uplink: %d", state);
    }
    displayLines(line1, line2);
  }

  delay(100);
}
