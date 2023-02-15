#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Servo.h>
#include <LoRa.h>

#include <telegram_api.h>
#include <const.h>
#include "../../include/common.h"

// TODO: sistemare leva buggata, sistemare wifi manager, sistemare altro firmware con payload > 0.

// Threads.
void button_thread(void*), lora_thread(void*);
TaskHandle_t button_thread_handle, lora_thread_handle;

WiFiClientSecure secured_client;
UniversalTelegramBot *bot;
notification_t notification;

uint32_t last_sequence_number = 0;
uint8_t battery_percentage = 100;

void initializeSecureClient(){
	Serial.print("Initializing WiFi");
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
	bot = new UniversalTelegramBot(BOT_TOKEN, secured_client);

	while(WiFi.status() != WL_CONNECTED){
		Serial.print(".");
		delay(500);
	}

	Serial.println();
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
}

void initializeLoraModule(){
	LoRa.setPins(SS, RST, DIO0);

	Serial.print("Initializing LoRa");
	while (!LoRa.begin(866E6)) {
		Serial.println(".");
		delay(500);
	}

	LoRa.setSyncWord(0xF3);
	Serial.println("\nLoRa Initializing OK!");
}

// Spawn the needed threads and kill the spawner thread.
inline void spawn_threads(){
	/*
		Parameters:
			Function to implement the task.
			Name of the task.
			Stack size in bytes (words in vanilla FreeRTOS).
			Task input parameter.
			Priority of the task.
			Task handle.
			Core where the task should run.
	*/

	xTaskCreatePinnedToCore(button_thread,	"button_thread",	10240,	NULL,	1,	&button_thread_handle,	APP_CPU);
	xTaskCreatePinnedToCore(lora_thread,		"lora_thread",		10240,	NULL,	1,	&lora_thread_handle,		PRO_CPU);

	// Deleting the spawner thread (setup thread).
	vTaskDelete(NULL);
}

void sendAck(){
	notification.type = MessageType::ACK;
	notification.sequence_number = last_sequence_number;

	LoRa.beginPacket();
	LoRa.write((uint8_t *) &notification, sizeof(notification));
	LoRa.endPacket();
}

void servoWrite(uint8_t pin, uint16_t degrees){
	Servo servo;

	servo.attach(pin);
	servo.write(degrees);
	delay(1000);

	servo.detach();
}

// Process a LoRa packet and send notifications.
void parseLoraPacket(){
	int packet_size = LoRa.parsePacket();
	if (packet_size < 1) return;

	Serial.println("packet received.");
	while (LoRa.available()){
		LoRa.readBytes((uint8_t *) &notification, sizeof(notification));
	}

	Serial.printf("Received packet with sequence number %d.\n", notification.sequence_number);
	
	if(notification.type == MessageType::MESSAGE && notification.sequence_number > last_sequence_number){
		last_sequence_number = notification.sequence_number;

		// The percentage is in notification.payload[0];
		battery_percentage = notification.payload[0];

		// notification.payload[1] > 0 means that a letter has been detected.
		if(notification.payload[1] > 0){
			Serial.println("Rising lever...");
			servoWrite(SERVO_PIN, SERVO_90);

			char tmp[64];
			sprintf(tmp, "C'è posta per te! (batteria: %d%)", battery_percentage);

			Serial.print("Sending telegram notification... ");
			if(bot->sendMessage(CHAT_ID, tmp, ""))
				Serial.println("Ok");

			else
				Serial.println("Failed");
		}
	}

	Serial.printf("Sending ACK with sequence number %d.\n", last_sequence_number);
	sendAck();
}

void loop(){}
void setup(){
	Serial.begin(115200);

	pinMode(BUTTON_PIN, INPUT_PULLUP);

	Serial.println("Servo test.");
	servoWrite(SERVO_PIN, SERVO_0);
	delay(500);
	servoWrite(SERVO_PIN, SERVO_90);
	delay(500);
	servoWrite(SERVO_PIN, SERVO_0);

	initializeSecureClient();
	initializeLoraModule();

	spawn_threads();
}

void button_thread(void *parameters){
	Serial.println("button_thread");

	while(true){
		Serial.println("await button.");
		while(digitalRead(BUTTON_PIN))
			vTaskDelay(1000 / portTICK_PERIOD_MS);

		Serial.println("button pressed.");
		servoWrite(SERVO_PIN, SERVO_0);
	}
}

void lora_thread(void *parameters){
	Serial.println("lora_thread");

	while(true){
		parseLoraPacket();
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}
