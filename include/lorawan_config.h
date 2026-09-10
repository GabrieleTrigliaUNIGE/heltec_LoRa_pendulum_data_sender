#pragma once

// ============================================================================
// Credenziali LoRaWAN OTAA — da impostare con i valori generati dal tuo
// Network Server (ChirpStack / The Things Stack / altro) a cui il gateway
// RAK inoltra i pacchetti. Il gateway NON entra in questi parametri: è solo
// un forwarder radio<->IP, l'OTAA avviene device<->Network Server.
// ============================================================================

// Regione radio: deve corrispondere al piano di frequenza del tuo gateway RAK
// (tipicamente EU868 in Europa, US915 in USA, ecc.)
#define LORAWAN_REGION EU868
#define LORAWAN_SUBBAND 0   // 0 per EU868; 2 per US915/AU915; 1 per CN470

#define RADIOLIB_LORAWAN_JOIN_EUI  0x503C3FAF138AC5AC
#define RADIOLIB_LORAWAN_DEV_EUI   0x8CFD49FFFEB5B8D4

#define RADIOLIB_LORAWAN_APP_KEY   0x9C,0x82,0x92,0xF2,0x4E,0x7C,0x78,0xFB, \
                                    0xB6,0x1A,0xB2,0xE0,0x38,0x4E,0x43,0x89
#define RADIOLIB_LORAWAN_NWK_KEY   0x9C,0x82,0x92,0xF2,0x4E,0x7C,0x78,0xFB, \
                                    0xB6,0x1A,0xB2,0xE0,0x38,0x4E,0x43,0x89
