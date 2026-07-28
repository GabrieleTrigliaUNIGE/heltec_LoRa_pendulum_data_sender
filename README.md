# Heltec V4 MPU6050 LoRa Node

Questo progetto fa parte dello sviluppo di un nodo sensore basato su **Heltec WiFi LoRa V4** (con chip ESP32-S3) e accelerometro **MPU6050**. L'obiettivo finale dell'intero sistema è il monitoraggio dinamico di un pendolo, campionandone i dati accelerometrici sui tre assi (X, Y, Z) per poi trasmetterli a un gateway tramite protocollo LoRaWAN/LoRa P2P. I dati raccolti saranno poi resi disponibili via MQTT per l'analisi in tempo reale su un nodo subscriber (es. MacBook o dashboard Node-RED/Grafana).

Questa specifica sezione del progetto implementa il **campionamento locale**, la **gestione modulare dell'hardware** e il **feedback visivo** (su display OLED e porta seriale nativa USB-CDC) per preparare e validare i dati prima della successiva fase di trasmissione RF.

---

## Architettura del Software

Il codice è stato ingegnerizzato seguendo un approccio altamente modulare in C++, pensato per facilitare il debug e preparare l'inserimento dello stack LoRa e del client MQTT. 

Il progetto utilizza un unico punto di ingresso per le configurazioni, demandando le implementazioni a file sorgente separati:

- `src/config.h`: Il cuore delle definizioni. Contiene i macro-flag di attivazione moduli, il pinout dell'hardware (per Heltec V4 e sensori I2C personalizzati), la struttura dati `MPUData` (che incapsula X, Y, Z) e i prototipi delle funzioni.
- `src/display.cpp`: Implementazione del display OLED (U8g2 con bit-banging I2C software per evitare conflitti bus).
- `src/imu.cpp`: Inizializzazione e lettura sicura dell'MPU6050 (con risoluzione dei problemi di instradamento I2C tipici dell'ESP32-S3).
- `src/main.cpp`: Il controllore di alto livello, che orchestra le letture dei sensori e la stampa dei dati.

---

## Collegamenti Hardware (Wiring)

Il modulo radio SX1262 e il display OLED condividono le stesse linee di alimentazione interne. Per garantire una comunicazione stabile con l'MPU6050, il bus I2C è stato mappato su pin sicuri tramite il *pin muxing* dell'ESP32-S3, evitando interferenze.

| MPU6050 Pin | Heltec V4 Pin (ESP32-S3) | Note |
| :---: | :---: | :--- |
| **VCC** | `3.3V` | Alimentazione logica (Se instabile, usare un condensatore) |
| **GND** | `GND` | Massa comune |
| **AD0** | `GND` | Massa comune per il corretto indirizzo (0x68) |
| **SDA** | `GPIO 41` | Linea Dati (Riassegnata via software) |
| **SCL** | `GPIO 42` | Linea Clock (Riassegnata via software) |

> **Attenzione per Heltec V4:** Il display OLED e l'antenna LoRa sono alimentati attraverso il pin `VEXT` (`GPIO 36`). È fondamentale portarlo a stato logico basso (`LOW`) nel `setup()` per alimentare le periferiche.
>
> ⚠️ **MOLTO IMPORTANTE:** Non accendere mai la scheda (né via USB né via batteria) senza aver preventivamente avvitato l'antenna LoRa. Una trasmissione senza carico (antenna) danneggerà irrimediabilmente l'amplificatore RF del modulo SX1262.

---

## Ambiente di Sviluppo

Il progetto è sviluppato e compilato utilizzando **PlatformIO** (estensione per Visual Studio Code). 

### Dipendenze (`platformio.ini`)
Le librerie principali utilizzate sono:
- `adafruit/Adafruit MPU6050` (e dipendenze associate)
- `olikraus/U8g2` (per la gestione ottimizzata del display OLED)

### Flag di Compilazione (Build Flags)
Per abilitare correttamente l'uscita della console seriale tramite l'interfaccia USB nativa dell'ESP32-S3 (senza passare per l'UART legacy), il file `platformio.ini` include:
```ini
build_flags = 
    -D ARDUINO_USB_MODE=1
    -D ARDUINO_USB_CDC_ON_BOOT=1
