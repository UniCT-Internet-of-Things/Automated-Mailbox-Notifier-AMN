#define PAYLOAD_MAXLEN 21

// Notification data struct.
typedef enum MessageType : uint8_t {
	MESSAGE,
	ACK
} messagetype_t;

// Notification data struct.
struct __attribute((__packed__)) notification_t {
	messagetype_t type = MessageType::MESSAGE;
	uint32_t sequence_number;

	uint8_t payload_length;
	uint8_t payload[PAYLOAD_MAXLEN];
};
