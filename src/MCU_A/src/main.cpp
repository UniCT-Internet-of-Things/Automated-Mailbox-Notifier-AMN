#include <Arduino.h>
#include <LoRa.h>
#include <display.h>

#include <const.h>
#include "../../include/common.h"

Display* display;
notification_t notification;
RTC_DATA_ATTR uint32_t sequence_number = 0;

inline void waitAck() {
	int expired = millis() + 5000;
	bool ack_missing = true;

	while(millis() < expired && ack_missing) {
		if(LoRa.parsePacket() > 1) {
			while(LoRa.available())
				LoRa.readBytes((uint8_t*) &notification, sizeof(notification));

			if(notification.type == MessageType::ACK) {
				sequence_number = notification.sequence_number;
				ack_missing = false;
			}
		}

		delay(1);
	}

	// Self wakeup if the ack is missing after the 5 seconds waiting.
	if(ack_missing)
		esp_sleep_enable_timer_wakeup(5000000);

	else
		sequence_number++;

	Serial.printf("Ack %sreceived...\n", ack_missing ? "not " : "");
}

void setup(){
	pinMode(WAKEUP_GPIO, INPUT);
	pinMode(LED_GPIO, OUTPUT);
	digitalWrite(LED_GPIO, HIGH);

	esp_sleep_enable_ext0_wakeup(WAKEUP_GPIO, HIGH);

	Serial.begin(115200);
	display = new Display();
	display->clear();

	// Print sequence number
	char message[256];
	sprintf(message, "Sequence number: %d", sequence_number);
	display->writeLine(0, 0, 1, message, 0);

	// Initialize LoRa
	display->writeLine(0, 0, 1, "Initializing LoRa...", 1);
	LoRa.setPins(PIN_SS, PIN_RST, PIN_DIO0);
	while(!LoRa.begin(LORA_WORKING_FREQ)){
		Serial.print(".");
		delay(1000);
	}
	delay(1000);
	LoRa.setSyncWord(LORA_SYNC_WORD);
	display->writeLine(0, 0, 1, "LoRa initialized.", 2);
	display->writeLine(0, 0, 1, "Notifyng receiver...", 3);

	// Send LoRa notification and battery percentage
	notification.type = MessageType::MESSAGE;
	notification.sequence_number = sequence_number;
	notification.payload[0] = 50;			// A battery test value (50%).
	notification.payload[1] = 1;			// Means that a letter has been inserted.
	notification.payload_length = 2;

	LoRa.beginPacket();
	LoRa.write((uint8_t*) &notification, sizeof(notification));
	LoRa.endPacket();

	waitAck();

	// Delete display content and go to deep sleep
	display->writeLine(0, 0, 1, "Going to sleep...", 4);
	digitalWrite(LED_GPIO, LOW);
	delay(2000);

	display->clear();
	delete display;

	esp_deep_sleep_start();
}

void loop(){}
