#include "LoRaWan_APP.h"
#include "config.h"

uint8_t devEui[] = { 0x22, 0x32, 0x33, 0x00, 0x00, 0x88, 0x88, 0x02 };
uint8_t appEui[] = { 0x50, 0x3c, 0x3f, 0xaf, 0x13, 0x8a, 0xc5, 0xac };
uint8_t appKey[] = { 0x9C, 0x82, 0x92, 0xF2, 0x4E, 0x7C, 0x78, 0xFB, 0xB6, 0x1A, 0xB2, 0xE0, 0x38, 0x4E, 0x43, 0x89 };

uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67 };
uint32_t devAddr =  ( uint32_t )0x007e6ae1;

/* Configurazione Rete */ 
uint16_t userChannelsMask[6] = { 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t  loraWanClass = CLASS_A;

/* Variabile di sistema vitale per il compilatore della libreria Heltec */
uint32_t appTxDutyCycle = APP_TX_DUTYCYCLE; 

/* Impostazioni Rete */
bool overTheAirActivation = true;
bool loraWanAdr = true;         
bool isTxConfirmed = false;      
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

// ======================================================================
// 📊 VARIABILI PER CAMPIONAMENTO E DUTY CYCLE MANUALE
// ======================================================================
#define MAX_SAMPLES 30 
const unsigned long SAMPLE_PERIOD_MS = 50; // 50 ms = 20 Hz
const unsigned long ETSI_DELAY_MS = APP_TX_DUTYCYCLE; // 30 Secondi di Duty Cycle
unsigned long lastTxTime = 0;
bool isFirstPacket = true;

RTC_DATA_ATTR int packetCounter = 0;

static void prepareTxFrame(uint8_t port)
{
    Serial.println("\n>>> APERTURA FINESTRA DI OSSERVAZIONE (1600 ms) <<<");

    packetCounter++;

    char buf[20];
    sprintf(buf, "Pkt N: %d", packetCounter);

    #ifdef USE_DISPLAY
    printDisplayMessage("Campionamento...", "Attendere 1.5s", buf);
    #endif

    appDataSize = MAX_SAMPLES * 6;

    int indicePayload = 0;
  
    for(int i = 0; i < MAX_SAMPLES; i++) {
        unsigned long startTime = millis();
        
        MPUData data = readIMU();

        appData[indicePayload]     = (data.x >> 8) & 0xFF; 
        appData[indicePayload + 1] = data.x & 0xFF;        
        appData[indicePayload + 2] = (data.y >> 8) & 0xFF; 
        appData[indicePayload + 3] = data.y & 0xFF;        
        appData[indicePayload + 4] = (data.z >> 8) & 0xFF; 
        appData[indicePayload + 5] = data.z & 0xFF;        

        indicePayload += 6;

        // Pausa attiva di compensazione: aspetta esattamente 50ms dall'inizio del ciclo
        while(millis() - startTime < SAMPLE_PERIOD_MS) { }
    }

    #ifdef USE_DISPLAY
        printDisplayMessage("Elaborato!", "In trasmissione", "LoRaWAN...");
    #endif
}

// ======================================================================
// 🚀 SETUP & LOOP
// ======================================================================
void setup() {
    Serial.begin(115200);
    delay(3000); 
    
    Serial.println("\n==========================================");
    Serial.println(" AVVIO NODO PENDOLO ");
    Serial.println("==========================================");

    Mcu.begin();
    
    #ifdef USE_DISPLAY
    setupDisplay();
    printDisplayMessage("Connessione...", "Join OTAA");
    #endif

    #ifdef USE_IMU
    Serial.println("Inizializzazione MPU6050...");
    if (!setupIMU()) {
        Serial.println("ERRORE: MPU6050 non trovato!");
        #ifdef USE_DISPLAY
        printDisplayMessage("Errore I2C", "Sensore KO");
        #endif
        while (1) { delay(10); } 
    }
    #endif

    deviceState = DEVICE_STATE_INIT;
}

void loop()
{
    switch( deviceState )
    {
        case DEVICE_STATE_INIT:
        {
#if(LORAWAN_DEVEUI_AUTO)
            LoRaWAN.generateDeveuiByChipID();
#endif
            LoRaWAN.init(loraWanClass, loraWanRegion);

            deviceState = DEVICE_STATE_JOIN;
            break;
        }
        case DEVICE_STATE_JOIN:
        {
            Serial.println("Tentativo di Join OTAA alla rete...");
            LoRaWAN.join();
            break;
        }
        case DEVICE_STATE_SEND:
        {
            prepareTxFrame(appPort);
            // ========================================================
            // 🔍 STAMPA DI DEBUG DEL PAYLOAD (HEX DUMP)
            // ========================================================
            Serial.printf(">>> FINESTRA CHIUSA. Payload: %d byte. Trasmissione...\n", appDataSize);
            Serial.println("--- PAYLOAD HEX DUMP (Ogni riga e' un campione: X, Y, Z) ---");
            
            for (int i = 0; i < appDataSize; i++) {
                Serial.printf("%02X ", appData[i]);
        
            // Vai a capo ogni 6 byte (dimensione esatta di una struct MPUData a 16 bit)
            if ((i + 1) % 6 == 0) {
                Serial.println(); 
        }
    }
    Serial.println("------------------------------------------------------------");
    
            LoRaWAN.send();
            deviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE:
        {
            // MICRO-SONNO di 500ms. L'ESP32 NON fa il Deep Sleep, 
            // ma salva energia e tiene attivi USB e OLED.
            txDutyCycleTime = 500; 
            LoRaWAN.cycle(txDutyCycleTime);
            deviceState = DEVICE_STATE_SLEEP;
            break;
        }
        case DEVICE_STATE_SLEEP:
        {
            // Ripristinato il VERO sleep per scaricare i pacchetti radio (risolve il blocco OTAA!)
            LoRaWAN.sleep(loraWanClass);
            
            // Quando si sveglia, la libreria tenta di spedire di nuovo. 
            // Noi intercettiamo l'azione e imponiamo la legge di ETSI (1 invio ogni 30 secondi).
            if (deviceState == DEVICE_STATE_SEND) {
                if (isFirstPacket || (millis() - lastTxTime >= ETSI_DELAY_MS)) {
                    isFirstPacket = false;
                    lastTxTime = millis();
                    // Via libera per il SEND
                } else {
                    // Non è ancora passato un minuto. Torna a fare micro-sonni.
                    deviceState = DEVICE_STATE_CYCLE;
                }
            }
            break;
        }
        default:
        {
            deviceState = DEVICE_STATE_INIT;
            break;
        }
    }
}



// 04 9A 40 76 FC D0 
// 04 16 40 A0 FC 7A 
// 03 84 40 92 FC 16 
// 04 90 40 B2 FC 7C 
// 04 4E 40 98 FC 64 
// 04 D2 40 CA FD 58 
// 04 68 41 2C FE 22 
// 01 A2 40 3E FB 9E 
// 02 4C 40 18 FA 6E 
// 04 D0 40 F2 FE 74 
// 03 DC 41 52 FE 14 
// 04 9E 41 C4 FF AE