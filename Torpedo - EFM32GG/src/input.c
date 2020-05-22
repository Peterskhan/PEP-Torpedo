#include "input.h"

void initInput() {

	// Enabling clock for UART
	CMU_ClockEnable(cmuClock_UART0, true);

	// Enabling clock for GPIO
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Initializing UART configuration
	USART_InitAsync_TypeDef UARTInit = {
		.autoCsEnable = false,
		.baudrate = 115200,
		.databits = usartDatabits8,
		.enable = usartEnable,
		.mvdis = false,
		.oversampling = usartOVS16,
		.parity = usartNoParity,
		.prsRxCh = usartPrsRxCh0,
		.prsRxEnable = false,
		.refFreq = 0,
		.stopbits = usartStopbits1
	};

	USART_InitAsync(UART0,&UARTInit);

	// Enable UART0 interrupt vector in NVIC
	NVIC_ClearPendingIRQ(UART0_RX_IRQn);
	NVIC_EnableIRQ(UART0_RX_IRQn);

	// Enabling interrupt requests for UART
	USART_IntClear(UART0, UART_IEN_RXDATAV);
	USART_IntEnable(UART0, UART_IEN_RXDATAV);

	// Configuring UART
	UART0->ROUTE |= (USART_ROUTE_TXPEN | USART_ROUTE_RXPEN);
	UART0->ROUTE |= (USART_ROUTE_LOCATION_LOC1);

	// Configuring GPIO
	GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 1); // TX
	GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);    // RX
	GPIO_PinModeSet(gpioPortF, 7, gpioModePushPull, 1); // Enabling to debugger

	// Create input queue
	inputQueue = xQueueCreate(1, sizeof(uint8_t));
}

void UART0_RX_IRQHandler(void) {

	// Clearing interrupt flags
	USART_IntClear(UART0,UART_IF_RXDATAV);

	// Input state to queue
	uint8_t inputState;

	// Setting input state
	switch(USART_RxDataGet(UART0)) {
	case ACTION_LEFT: 	inputState = MoveLeft;  break;
	case ACTION_RIGHT: 	inputState = MoveRight;	break;
	case ACTION_UP: 	inputState = MoveUp; 	break;
	case ACTION_DOWN: 	inputState = MoveDown;	break;
	case ACTION_FIRE: 	inputState = Fire; 		break;
	default:  			inputState = None;		break;
	}

	// Placing input state into queue
	xQueueSendFromISR(inputQueue, &inputState, NULL);
}
