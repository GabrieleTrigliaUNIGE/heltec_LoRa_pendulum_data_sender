# Heltec WiFi LoRa 32 V4 -> LoRaWAN OTAA via gateway RAK

## Come si incastra il gateway RAK

Il gateway RAK **non parla direttamente** col firmware: fa solo da "packet
forwarder" radio<->IP verso un **Network Server** (ChirpStack, The Things
Stack/TTN, o quello integrato se usi un RAK con LNS builtin). Il join OTAA
(DevEUI/JoinEUI/AppKey) va configurato sul Network Server, non sul gateway.
Il firmware qui sotto deve solo:

1. essere sulla stessa regione/piano di frequenza del gateway (es. EU868);
2. avere DevEUI/JoinEUI/AppKey coerenti con quelli registrati sul Network
   Server, in `include/lorawan_config.h`.

Se il gateway non è ancora collegato a nessun Network Server, il primo passo
è quello (es. ChirpStack self-hosted, o TTN se il gateway ha copertura
internet e puoi registrarlo lì).

## Struttura del progetto

- `platformio.ini` — ambiente `heltec_wifi_lora_32_V4`, libreria RadioLib
- `boards/heltec_wifi_lora_32_V4.json` — definizione board custom (la V4 non
  è ancora "ufficiale" nel platform espressif32 di PlatformIO)
- `variants/heltec_V4/pins_arduino.h` — pin mapping (identico alla V3: stesso
  SoC ESP32-S3 + SX1262, stesso pinout)
- `include/lorawan_config.h` — **da compilare** con le tue credenziali OTAA
- `src/main.cpp` — join OTAA + invio periodico (ogni 60s) via RadioLib

## Passi

1. Apri `include/lorawan_config.h` e imposta:
   - `LORAWAN_REGION` (EU868/US915/...) in base al tuo gateway
   - `RADIOLIB_LORAWAN_JOIN_EUI`, `RADIOLIB_LORAWAN_DEV_EUI`,
     `RADIOLIB_LORAWAN_APP_KEY`/`NWK_KEY` copiati dal Network Server
2. `pio run -t upload` (o build/upload da VS Code + estensione PlatformIO)
3. `pio device monitor` per vedere il join e gli invii

## Note

- Pin radio: NSS=8, DIO1=14, RST=12, BUSY=13 (SX1262).
- `Vext` (GPIO36) alimenta OLED/periferiche on-board: nel codice viene messo
  LOW (acceso) in `setup()`, toglilo se non ti serve o vuoi risparmiare
  energia.
- Quando l'SDK espressif32 di PlatformIO includerà ufficialmente la board V4
  (al momento è ancora in review), si potrà sostituire `boards/` e
  `variants/` con `board = heltec_wifi_lora_32_V4` "nativo".
- Payload di esempio: 4 byte con un contatore. Sostituiscilo con i tuoi dati
  reali (es. letture sensori), tenendo conto dei limiti di payload size del
  data rate assegnato dall'ADR.
