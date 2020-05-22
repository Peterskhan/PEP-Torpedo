#pragma once

// Standard includes
#include <stdint.h>

// Board includes
#include <em_usart.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Describes the possible message types.
 */
typedef enum MessageType {
	GameStartedMsg,
	GameFinishedMsg,
	SegmentSelectedMsg,
	SegmentFiredMsg,
	SegmentHitMsg,
	SegmentMissedMsg
} MessageType;

/**
 * @brief Describes the message sent when the game starts.
 */
typedef struct GameStartedMessage {
	uint32_t startTick;		/**< The value of the game tick counter when the game started. 	*/
	uint8_t  tickDelayMs;	/**< The time delay between game ticks in milliseconds. 		*/
	uint8_t  mapIndex;		/**< The index of the map the game is played on. 				*/
} GameStartedMessage;

/**
 * @brief Describes the message sent the the game ends.
 */
typedef struct GameFinishedMessage {
	uint32_t stopTick;		/**< The value of the game tick counter when the game finished.	*/
	uint8_t  shotsTotal;	/**< The total number of shots fired when the game finished.	*/
} GameFinishedMessage;

/**
 * @brief Describes the message sent when a new segment is selected.
 */
typedef struct SegmentSelectedMessage {
	uint32_t gameTick;		/**< The value of the game tick counter when the segment was selected.	*/
	uint8_t  segmentID;		/**< The ID of the segment selected. 									*/
} SegmentSelectedMessage;

/**
 * @brief Describes the message sent when a segment is fired at.
 */
typedef struct SegmentFiredMessage {
	uint32_t gameTick;		/**< The value of the game tick counter when the segment was fired at.	*/
	uint8_t  segmentID;		/**< The ID of the segment fired at. 									*/
} SegmentFiredMessage;

/**
 * @brief Describes the message sent when a segment is hit.
 */
typedef struct SegmentHitMessage {
	uint32_t gameTick;		/**< The value of the game tick counter when the segment was hit.	*/
	uint8_t  segmentID;		/**< The ID of the segment hit.	 									*/
} SegmentHitMessage;

/**
 * @brief Describes the message sent when a segment is missed.
 */
typedef struct SegmentMissedMessage {
	uint32_t gameTick;		/**< The value of the game tick counter when the segment was missed.	*/
	uint8_t  segmentID;		/**< The ID of the segment missed. 										*/
} SegmentMissedMessage;

/**
 * @brief Describes a message that is one of the predefined message types.
 */
typedef union Message_t {
	GameStartedMessage 	   gameStartedMessage;
	GameFinishedMessage    gameFinishedMessage;
	SegmentSelectedMessage segmentSelectedMessage;
	SegmentFiredMessage    segmentFiredMessage;
	SegmentHitMessage      segmentHitMessage;
	SegmentMissedMessage   segmentMissedMessage;
} Message_t;

/**
 * @brief Describes a message with the type identifier and the message body.
 */
typedef struct Message {
	uint8_t   messageID;	/**< The type identifier of the message.	*/
	Message_t message;		/**< The content of the message. 			*/
} Message;

/**
 * @brief FreeRTOS queue holding the statistics messages to transmit.
 */
QueueHandle_t statisticsQueue;

/**
 * @brief Queues the specified message for transmission over the UART.
 * @param [in] The type identifier of the message.
 * @param [in] The message structure to transmit.
 */
void sendStatisticsMessage(MessageType type, Message_t message);

/**
 * @brief The FreeRTOS task that transmits statistics over the UART.
 * @param [in] The FreeRTOS task parameter (unused).
 */
void prvStatisticsTask(void *prvParam);

