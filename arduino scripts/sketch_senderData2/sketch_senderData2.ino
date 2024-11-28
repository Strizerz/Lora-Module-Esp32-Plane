#include <LoRaWan_APP.h>
// I used to use <LoRa_APP.h> but that subset of the LoRaWAN_APP library
// only seems to be available for the CubeCell boards and not the ESP32-S3

static RadioEvents_t RadioEvents;

void onTxDone(void) {
  Serial.println("[onTxDone] TX done!");
  // Add any code that is to be executed when transmission is complete
}

void onTxTimeout(void) {
  Serial.println("[onTxTimeout] TX Timeout...");
  // Add any code that is to be executed on transmission timeout
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  Serial.println("[onRxDone] RX done!");
  Radio.Sleep();
  lora_idle = true;
  // Add code to process the payload
}

void setup() {

  // Initialise the radio

  RadioEvents.TxDone = onTxDone;
  RadioEvents.TxTimeout = onTxTimeout;
  RadioEvents.RxDone = OnRxDone;

  Radio.Init( &RadioEvents );
  Radio.SetChannel( Frequency );
  Radio.SetTxConfig( MODEM_LORA, OutputPower, 0, SignalBandwidthIndex,
                      SpreadingFactor, CodingRate,
                      PreambleLength, FixedLengthPayload,
                      true, 0, 0, IQInversion, 3000 );
  Radio.SetRxConfig( MODEM_LORA, SignalBandwidthIndex, SpreadingFactor,
                                  CodingRate, 0, PreambleLength,
                                  SymbolTimeout, FixedLengthPayload,
                                  0, true, 0, 0, IQInversion, true);

  Serial.println("[setup] LoRa initialisation complete");
}