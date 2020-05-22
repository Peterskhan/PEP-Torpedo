#include "game_logic.h"

void updateGameLoop(void) {

	// Increasing gameTick counter
	gameTick++;

	// Action based on current gamestate
	switch(gameState) {
	case GameStart: 	gameStart(); 		break;
	case SegmentSelect:	segmentSelect(); 	break;
	case SegmentFire:	segmentFire(); 		break;
	case SegmentHit:	segmentHit(); 		break;
	case GameOver:		gameOver(); 		break;
	}
}

void gameStart(void) {

	// Initializing single value globals
	selectedSegment = 0;
	remainingShips = 8;
	gameplayMap = NULL;
	gameplayMapIndex = 0;
	gameTick = 0;

	// Initializing dependencies
	initDisplay();
	initInput();

	// Generating map
	generateMap();

	// Assembling statistics message
	Message_t msg = {
		.gameStartedMessage.startTick   = gameTick,
		.gameStartedMessage.tickDelayMs = GAMEPLAY_TICK_DELAY_MS,
		.gameStartedMessage.mapIndex    = gameplayMapIndex
	};

	// Sending statistics message
	sendStatisticsMessage(GameStartedMsg, msg);

	// Updating gamestate
	gameState = SegmentSelect;
}

void generateMap(void) {

	// List of predefined maps.
	static bool maps[PREDEFINED_MAP_COUNT][LCD_SEGMENTS_COUNT] = {0};

	// MAP1
	maps[0][9]  = true;
	maps[0][12] = true;
	maps[0][48] = true;
	maps[0][51] = true;
	maps[0][53] = true;
	maps[0][54] = true;
	maps[0][78] = true;
	maps[0][65] = true;

	// MAP2
	maps[1][0]  = true;
	maps[1][13] = true;
	maps[1][52] = true;
	maps[1][65] = true;
	maps[1][79] = true;
	maps[1][80] = true;
	maps[1][35] = true;
	maps[1][38] = true;

	// MAP3
	maps[2][7]  = true;
	maps[2][10] = true;
	maps[2][30] = true;
	maps[2][31] = true;
	maps[2][87] = true;
	maps[2][90] = true;
	maps[2][47] = true;
	maps[2][50] = true;

	// MAP4
	maps[3][4]  = true;
	maps[3][5]  = true;
	maps[3][14] = true;
	maps[3][15] = true;
	maps[3][30] = true;
	maps[3][31] = true;
	maps[3][79] = true;
	maps[3][80] = true;

	// MAP5
	maps[4][6]  = true;
	maps[4][19] = true;
	maps[4][26] = true;
	maps[4][39] = true;
	maps[4][29] = true;
	maps[4][42] = true;
	maps[4][74] = true;
	maps[4][77] = true;

	// MAP6
	maps[5][0]  = true;
	maps[5][3]  = true;
	maps[5][13] = true;
	maps[5][16] = true;
	maps[5][32] = true;
	maps[5][45] = true;
	maps[5][71] = true;
	maps[5][84] = true;

	// MAP7
	maps[6][9]  = true;
	maps[6][12] = true;
	maps[6][22] = true;
	maps[6][25] = true;
	maps[6][35] = true;
	maps[6][38] = true;
	maps[6][59] = true;
	maps[6][62] = true;

	// MAP8
	maps[7][6]  = true;
	maps[7][19] = true;
	maps[7][34] = true;
	maps[7][37] = true;
	maps[7][73] = true;
	maps[7][76] = true;
	maps[7][85] = true;
	maps[7][88] = true;

	// MAP9
	maps[8][21] = true;
	maps[8][24] = true;
	maps[8][34] = true;
	maps[8][37] = true;
	maps[8][45] = true;
	maps[8][58] = true;
	maps[8][68] = true;
	maps[8][81] = true;

	// MAP10
	maps[9][0]  = true;
	maps[9][3]  = true;
	maps[9][6]  = true;
	maps[9][13] = true;
	maps[9][19] = true;
	maps[9][16] = true;
	maps[9][45] = true;
	maps[9][58] = true;

	// MAP11
	maps[10][65] = true;
	maps[10][78] = true;
	maps[10][68] = true;
	maps[10][81] = true;
	maps[10][71] = true;
	maps[10][84] = true;
	maps[10][32] = true;
	maps[10][45] = true;

	// MAP12
	maps[11][9]  = true;
	maps[11][12] = true;
	maps[11][20] = true;
	maps[11][23] = true;
	maps[11][32] = true;
	maps[11][45] = true;
	maps[11][73] = true;
	maps[11][76] = true;

	// MAP13
	maps[12][34] = true;
	maps[12][37] = true;
	maps[12][21] = true;
	maps[12][24] = true;
	maps[12][60] = true;
	maps[12][63] = true;
	maps[12][71] = true;
	maps[12][84] = true;

	// MAP14
	maps[13][3]  = true;
	maps[13][16] = true;
	maps[13][27] = true;
	maps[13][28] = true;
	maps[13][30] = true;
	maps[13][31] = true;
	maps[13][53] = true;
	maps[13][54] = true;

	// MAP15
	maps[14][4]  = true;
	maps[14][5]  = true;
	maps[14][16] = true;
	maps[14][29] = true;
	maps[14][43] = true;
	maps[14][44] = true;
	maps[14][71] = true;
	maps[14][84] = true;

	// MAP16
	maps[15][6]  = true;
	maps[15][19] = true;
	maps[15][34] = true;
	maps[15][37] = true;
	maps[15][59] = true;
	maps[15][62] = true;
	maps[15][87] = true;
	maps[15][90] = true;

	// Seeding random generator
	srand(time(0));

	// Picking map at random
	gameplayMapIndex = rand() % PREDEFINED_MAP_COUNT;

	// Activating selected map
	gameplayMap = &(maps[gameplayMapIndex][0]);
}

void segmentSelect(void) {

	// Counter used for timing the blinking
	static uint8_t timingCounter = 0;

	if(timingCounter++ == 15) {

		// Blinking selected segment
		displayMap[selectedSegment] = !displayMap[selectedSegment];

		// Resetting timing counter
		timingCounter = 0;
	}

	// Reading input state
	uint8_t inputState = None;
	xQueueReceive(inputQueue, &inputState, 0);

	// Action based on input state
	switch(inputState) {
	case MoveLeft: 	displayMap[selectedSegment] = false; moveLeft(); 	break;
	case MoveRight: displayMap[selectedSegment] = false; moveRight(); 	break;
	case MoveUp: 	displayMap[selectedSegment] = false; moveUp();		break;
	case MoveDown: 	displayMap[selectedSegment] = false; moveDown();	break;
	case Fire: 		displayMap[selectedSegment] = false; fire(); 		break;
	default: 															break;
	}

	if(inputState == MoveLeft || inputState == MoveRight || inputState == MoveUp || inputState == MoveDown) {

		// Assembling statistics message
		Message_t msg = {
			.segmentSelectedMessage.gameTick  = gameTick,
			.segmentSelectedMessage.segmentID = selectedSegment
		};

		// Sending statistics message
		sendStatisticsMessage(SegmentSelectedMsg, msg);
	}

	if(inputState == Fire) {

		// Assembling statistics message
		Message_t msg = {
			.segmentFiredMessage.gameTick  = gameTick,
			.segmentFiredMessage.segmentID = selectedSegment
		};

		// Sending statistics message
		sendStatisticsMessage(SegmentFiredMsg, msg);
	}

	// Making sure blinking doesn't hide already uncovered ships
	for(int i = 0; i < 91; i++) {
		if(i != selectedSegment) {
			displayMap[i] = uncoveredMap[i];
		}
	}
}

void moveLeft(void) {

	// Updating selected segment
	selectedSegment = selectedSegment-LCD_LOWER_SEGMENTS_COUNT < 0 ?
				      LCD_SEGMENTS_COUNT + (selectedSegment-LCD_LOWER_SEGMENTS_COUNT) :
					  selectedSegment-LCD_LOWER_SEGMENTS_COUNT;
}

void moveRight(void) {

	// Updating selected segment
	selectedSegment = (selectedSegment + LCD_LOWER_SEGMENTS_COUNT) % (LCD_SEGMENTS_COUNT);
}

void moveUp(void) {

	// Updating selected segment
	selectedSegment = (selectedSegment/13)*13 + ((selectedSegment+1)%13);
}

void moveDown(void) {

	// Updating selected segment
	int8_t base = (selectedSegment/13)*13;
	int8_t offset = selectedSegment-base-1 > 0 ? (selectedSegment-1)%13 : (selectedSegment+12)%13;
	selectedSegment =  base + offset;
}

void fire(void) {

	// Updating gamestate
	gameState  = SegmentFire;

	// Increasing number of total shots
	if(!uncoveredMap[selectedSegment]) shotsTotal++;
}

void segmentFire(void) {

	// Bitmask for the Ring LCD Segments
	static uint8_t spinnerState = 0x01;

	// Counter used for timing the spinner
	static uint8_t timingCounter = 0;

	// Triggered on every Nth call of the function
	if(timingCounter++ == 15) {

		// Setting the LCD Ring Segments
		spinnerBitmask = spinnerState;

		// Shifting the spinner by one position
		spinnerState = spinnerState << 1;

		// Resetting timing counter
		timingCounter = 0;
	}

	// Reset spinnerState on overflow and check if hit
	if(spinnerState == 0x00) {
		spinnerState = 0x01;

		// Updating gamestate
		if(gameplayMap[selectedSegment]) {
			gameState = SegmentHit;
			spinnerBitmask = 0x00;

			// Assembling statistics message
			Message_t msg = {
				.segmentHitMessage.gameTick  = gameTick,
				.segmentHitMessage.segmentID = selectedSegment
			};

			// Sending statistics message
			sendStatisticsMessage(SegmentHitMsg, msg);

		} else {
			gameState = SegmentSelect;
			spinnerBitmask = 0x00;

			// Assembling statistics message
			Message_t msg = {
				.segmentMissedMessage.gameTick  = gameTick,
				.segmentMissedMessage.segmentID = selectedSegment
			};

			// Sending statistics message
			sendStatisticsMessage(SegmentMissedMsg, msg);
		}
	}
}

void segmentHit(void) {

	// Bitmask for the Ring LCD Segments
	static uint8_t spinnerState = 0xff;

	// Counter used for timing the blinks
	static uint8_t timingCounter = 0;

	// Counter used for counting the blinks
	static uint8_t blinkCounter = 0;

	// Triggered on every Nth call of the function
	if(timingCounter++ == 15) {

		// Setting the LCD Ring Segments
		spinnerBitmask = spinnerState;

		// Turning off the segments
		if(spinnerState) {
			spinnerState = 0x00;
		}
		// Turning on the segments
		else {
			spinnerState = 0xff;
			blinkCounter++;
		}

		// Resetting timing counter
		timingCounter = 0;
	}

	// Triggered when blinking should finish
	if(blinkCounter == 3) {

		// Turning off the segments
		spinnerBitmask = 0x00;

		// Decreasing the number of remaining ships
		if(!uncoveredMap[selectedSegment]) remainingShips--;

		// Indicating the hit on the uncovered map
		uncoveredMap[selectedSegment] = true;

		// Resetting blink counter
		blinkCounter = 0;

		// Moving to the next game state
		if(remainingShips != 0) {
			gameState = SegmentSelect;
		} else {
			gameState = GameOver;
		}
	}
}

void gameOver(void) {

	// Assembling statistics message
	Message_t msg = {
		.gameFinishedMessage.stopTick   = gameTick,
		.gameFinishedMessage.shotsTotal = shotsTotal
	};

	// Sending statistics message
	sendStatisticsMessage(GameFinishedMsg, msg);

	// Resetting game state
	gameState = GameStart;
}

void prvGameloopTask(void *prvParam) {

	while(1) {

		// Updating the gameloop
		updateGameLoop();

		// Delay for 10ms (100 Hz)
		vTaskDelay(pdMS_TO_TICKS(GAMEPLAY_TICK_DELAY_MS));
	}
}
