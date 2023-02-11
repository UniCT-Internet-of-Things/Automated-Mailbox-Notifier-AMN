// GPIOs.
#define WAKEUP_GPIO	GPIO_NUM_13	// PIR toggle to HIGH when it's triggered.
#define LED_GPIO    GPIO_NUM_25 // LED to indicate that the device is awake.

// LoRa pins.
#define PIN_SS		18
#define PIN_RST		23
#define PIN_DIO0	26

// LoRa frequencies:
// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define LORA_WORKING_FREQ	866E6
#define LORA_SYNC_WORD		0xF3
