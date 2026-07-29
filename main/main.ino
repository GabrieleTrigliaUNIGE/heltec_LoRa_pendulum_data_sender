#include <Arduino.h>
#include "config.h"
#include "LoRaWan_APP.h"

// ======================================================================
// 🔑 CHIAVI LORAWAN (OTAA)
// ======================================================================
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
uint32_t appTxDutyCycle = 15000; // Questo valore non bloccherà più il codice

/* Impostazioni Rete (Forzate per Streaming) */
bool overTheAirActivation = true;
bool loraWanAdr = false;         // FONDAMENTALE DISATTIVARLO: Dobbiamo forzare SF bassi per trasmettere 192 byte
bool isTxConfirmed = false;      // Evitiamo di congestionare il canale richiedendo ricevute
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;

// ======================================================================
// VARIABILI PER CAMPIONAMENTO A 20Hz E BUFFERING
// ======================================================================
#define MAX_SAMPLES 16 // 16 campioni * 12 byte = 192 byte 
MPUData sampleBuffer[MAX_SAMPLES];

int currentIndex = 0;
unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_PERIOD_MS = 50; // 50 ms = 20 Hz

// ======================================================================
// COSTRUZIONE DEL PAYLOAD
// ======================================================================
static void prepareTxFrame(uint8_t port)
{
    // Il buffer è già stato riempito nel loop, calcoliamo solo la dimensione
    appDataSize = MAX_SAMPLES * sizeof(MPUData); 
    
    // Copiamo in blocco tutta la mappa di memoria (192 byte) nel buffer LoRa
    memcpy(appData, sampleBuffer, appDataSize);

    Serial.printf("\n>>> BUFFER PIENO! Trasmissione in corso di %d byte (%d campioni) via LoRa...\n", appDataSize, MAX_SAMPLES);
    
    // Azzeriamo l'indice per far ripartire il campionamento
    currentIndex = 0;
}

// ======================================================================
// 🚀 SETUP & LOOP
// ======================================================================
void setup() {
    Serial.begin(115200);
    delay(3000); 
    
    Serial.println("\n==========================================");
    Serial.println(" AVVIO NODO LORA PENDOLO - 20Hz STREAMING");
    Serial.println("==========================================");

    Mcu.begin();
    
    #ifdef ENABLE_DISPLAY
    setupDisplay();
    printDisplayMessage("Connessione...", "Join OTAA in corso");
    #endif

    #ifdef ENABLE_IMU
    Serial.println("Inizializzazione MPU6050...");
    if (!setupIMU()) {
        Serial.println("ERRORE: MPU6050 non trovato!");
        #ifdef ENABLE_DISPLAY
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
            // La libreria usa SF di default basso (SF7) al primo avvio
            break;
        }
        case DEVICE_STATE_JOIN:
        {
            Serial.println("Tentativo di Join OTAA...");
            LoRaWAN.join();
            break;
        }
        case DEVICE_STATE_SEND:
        {
            prepareTxFrame(appPort);
            LoRaWAN.send();
            deviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE:
        {
            // INTERCETTATO: Non diamo il controllo temporale a LoRaWAN
            // Passiamo direttamente alla nostra logica di campionamento
            deviceState = DEVICE_STATE_SLEEP; 
            break;
        }
        case DEVICE_STATE_SLEEP:
        {
            // =====================================================
            // LOGICA DI CAMPIONAMENTO CONTINUO (NON BLOCCANTE)
            // =====================================================
            if (millis() - lastSampleTime >= SAMPLE_PERIOD_MS) {
                lastSampleTime = millis(); // Resetta il cronometro dei 50ms

                #ifdef ENABLE_IMU
                sampleBuffer[currentIndex] = readIMU();
                #endif

                // Aggiorniamo il display SOLO sul primo campione del ciclo per evitare lag
                #ifdef ENABLE_DISPLAY
                if (currentIndex == 0) {
                    updateDisplayData(sampleBuffer[currentIndex]);
                }
                #endif

                currentIndex++;

                // Se abbiamo raccolto 16 campioni (192 byte totali, ~800 millisecondi trascorsi)
                // Usciamo da questa fase e inneschiamo la trasmissione radio
                if (currentIndex >= MAX_SAMPLES) {
                    deviceState = DEVICE_STATE_SEND;
                }
            }
            
            // FONDAMENTALE: Non chiamiamo mai LoRaWAN.sleep()
            // L'ESP32 rimarrà sempre acceso, scansionando i 50ms
            break;
        }
        default:
        {
            deviceState = DEVICE_STATE_INIT;
            break;
        }
    }
}