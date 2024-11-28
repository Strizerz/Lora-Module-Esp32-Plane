#include <ESP32Servo.h>
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#define RF_FREQUENCY                                915000000 // Hz
#define LORA_BANDWIDTH                              0
#define LORA_SPREADING_FACTOR                       7
#define LORA_CODINGRATE                             1
#define LORA_PREAMBLE_LENGTH                        8
#define LORA_SYMBOL_TIMEOUT                         0
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define BUFFER_SIZE                                 10
#define SERVO_PIN                                   13

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

char rxpacket[BUFFER_SIZE];
bool lora_idle = true;

Servo myServo; // Create a servo object

static RadioEvents_t RadioEvents;
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    // Initialize display
    delay(100);
    display.init();
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawString(0, 0, "Waiting for signal...");
    display.display();

    // Initialize servo
    myServo.attach(SERVO_PIN); // Attach servo to GPIO 13
    myServo.write(90);         // Initialize servo to the neutral position

    // Initialize LoRa
    RadioEvents.RxDone = OnRxDone;
    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
}

void loop() {
    if (lora_idle) {
        lora_idle = false;
        Radio.Rx(0);
    }
    Radio.IrqProcess();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    // Process the received packet
    memcpy(rxpacket, payload, size);
    rxpacket[size] = '\0';

    // Convert the received value to an integer
    int potentiometerValue = atoi(rxpacket);

    // Update servo position
    myServo.write(potentiometerValue);

    // Display received value on OLED screen
    display.clear();
    display.drawString(0, 0, "Signal Received");
    display.drawString(0, 15, "Value: " + String(potentiometerValue));
    display.drawString(0, 30, "RSSI: " + String(rssi));
    display.drawString(0, 45, "SNR: " + String(snr));
    display.display();

    // Debug output
    Serial.printf("Received potentiometer value: %d, RSSI: %d, SNR: %d\n", potentiometerValue, rssi, snr);

    // Reset LoRa to idle
    Radio.Sleep();
    lora_idle = true;
}
