#include "statistics.h"


void sendStatisticsMessage(MessageType type, Message_t message) {

	// Constructing message to transmit
	Message msg = {
		.messageID = type,
		.message   = message
	};

	// Posting message to queue for transmit
	xQueueSendToBack(statisticsQueue, &msg, portMAX_DELAY);
}

void prvStatisticsTask(void *prvParam) {

	// Initializing message queue with 10 slots
	statisticsQueue = xQueueCreate(10, sizeof(Message));

	while(1) {

		// The next message to transmit
		Message nextMessage;

		// Reading next message from the queue
		if(xQueueReceive(statisticsQueue, &nextMessage, 0)) {

			// Differentiating based on message type identifier
			switch(nextMessage.messageID) {
			case GameStartedMsg:

				// Sending message type identifier
				USART_Tx(UART0, nextMessage.messageID);

				// Sending startTick
				USART_Tx(UART0, nextMessage.message.gameStartedMessage.startTick);
				USART_Tx(UART0, nextMessage.message.gameStartedMessage.startTick >> 8);
				USART_Tx(UART0, nextMessage.message.gameStartedMessage.startTick >> 16);
				USART_Tx(UART0, nextMessage.message.gameStartedMessage.startTick >> 24);

				// Sending tickDelayMs
				USART_Tx(UART0, nextMessage.message.gameStartedMessage.tickDelayMs);

				// Sending mapIndex
				USART_Tx(UART0, nextMessage.message.gameStartedMessage.mapIndex);
				break;

			case GameFinishedMsg:

				// Sending message type identifier
				USART_Tx(UART0, nextMessage.messageID);

				// Sending stopTick
				USART_Tx(UART0, nextMessage.message.gameFinishedMessage.stopTick);
				USART_Tx(UART0, nextMessage.message.gameFinishedMessage.stopTick >> 8);
				USART_Tx(UART0, nextMessage.message.gameFinishedMessage.stopTick >> 16);
				USART_Tx(UART0, nextMessage.message.gameFinishedMessage.stopTick >> 24);

				// Sending shotsTotal
				USART_Tx(UART0, nextMessage.message.gameFinishedMessage.shotsTotal);
				break;

			// The following message types have identical structures
			case SegmentSelectedMsg:	// [[ fallthrough ]]
			case SegmentFiredMsg:		// [[ fallthrough ]]
			case SegmentHitMsg:			// [[ fallthrough ]]
			case SegmentMissedMsg:		// [[ fallthrough ]]

				// Sending message type identifier
				USART_Tx(UART0, nextMessage.messageID);

				// Sending gameTick
				USART_Tx(UART0, nextMessage.message.segmentSelectedMessage.gameTick);
				USART_Tx(UART0, nextMessage.message.segmentSelectedMessage.gameTick >> 8);
				USART_Tx(UART0, nextMessage.message.segmentSelectedMessage.gameTick >> 16);
				USART_Tx(UART0, nextMessage.message.segmentSelectedMessage.gameTick >> 24);

				// Sending segmentID
				USART_Tx(UART0, nextMessage.message.segmentSelectedMessage.segmentID);
				break;

			default: break;
			}
		}
	}
}
