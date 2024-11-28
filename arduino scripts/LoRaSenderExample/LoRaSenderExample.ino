#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             5        // dBm
#define LORA_BANDWIDTH                              0
#define LORA_SPREADING_FACTOR                       7
#define LORA_CODINGRATE                             1
#define LORA_PREAMBLE_LENGTH                        8
#define LORA_SYMBOL_TIMEOUT                         0
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 10
#define POTENTIOMETER_PIN                           2

char txpacket[BUFFER_SIZE];
int potentiometerValue;
bool lora_idle = true;

static RadioEvents_t RadioEvents;

void OnTxDone(void);
void OnTxTimeout(void);
void CheckLoRaConnection();

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    // Power on external peripherals and initialize the display
    
    delay(100);
    display.init();
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawString(0, 0, "Initializing LoRa...");
    display.display();

    // Initialize LoRa
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;

    if (!Radio.Init(&RadioEvents)) {
        display.clear();
        display.drawString(0, 0, "LoRa init failed!");
        display.display();
        Serial.println("LoRa initialization failed!");
        
    }

    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                      LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                      true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    display.clear();
    display.drawString(0, 0, "LoRa Initialized");
    display.display();
    delay(1000); // Allow user to see initialization message
}

void loop() {
    if (lora_idle) {
        // Read and map potentiometer value
        potentiometerValue = analogRead(POTENTIOMETER_PIN);
        potentiometerValue = map(potentiometerValue, 0, 4094, 180, 0);

        // Format payload
        sprintf(txpacket, "%d", potentiometerValue);

        // Send the value
        Radio.Send((uint8_t *)txpacket, strlen(txpacket));
        lora_idle = false;

        // Update OLED screen
        

        Serial.printf("Sent potentiometer value: %d\n", potentiometerValue);
    }
    Radio.IrqProcess();
}

void OnTxDone(void) {
    lora_idle = true;
    display.clear();
    display.drawString(0, 0, "Transmission Complete Sending Value:");
    display.drawString(0, 15, String(potentiometerValue));
    display.display();
    Serial.println("Transmission complete.");
}

void OnTxTimeout(void) {
    lora_idle = true;
    display.clear();
    display.drawString(0, 0, "Transmission Timeout");
    display.display();
    Serial.println("Transmission timeout.");
}
