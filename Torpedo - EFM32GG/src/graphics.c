#include "graphics.h"

void initDisplay(void) {

	// Initializing the LCD display with no voltage boost
	SegmentLCD_Init(false);

	// Initializing spinner bitmask
	spinnerBitmask = 0x00;

	// Initializing number display
	shotsTotal = 0;

	// Initializing display & uncovered maps
	for(int i = 0; i < LCD_SEGMENTS_COUNT; i++) {
		displayMap[i] = false;
		uncoveredMap[i] = false;
	}
}

void updateLowerLCD(void) {

	// Initializing segment configuration
	SegmentLCD_LowerCharSegments_TypeDef lowerSegments[7] = {0};

	// Checking display segments
	for(int i = 0; i < LCD_SEGMENTS_COUNT; i++) {

		if(displayMap[i]) {
			// Acquiring digit and segment ID
			uint8_t digitID =   i / LCD_LOWER_SEGMENTS_COUNT;
			uint8_t segmentID = i % LCD_LOWER_SEGMENTS_COUNT;

			// Setting up segment configuration
			switch(segmentID) {
			case 0:  lowerSegments[digitID].a = true; break;
			case 1:  lowerSegments[digitID].b = true; break;
			case 2:  lowerSegments[digitID].c = true; break;
			case 3:  lowerSegments[digitID].d = true; break;
			case 4:  lowerSegments[digitID].e = true; break;
			case 5:  lowerSegments[digitID].f = true; break;
			case 6:  lowerSegments[digitID].g = true;
			         lowerSegments[digitID].m = true; break;
			case 7:  lowerSegments[digitID].h = true; break;
			case 8:  lowerSegments[digitID].j = true; break;
			case 9:  lowerSegments[digitID].k = true; break;
			case 10: lowerSegments[digitID].n = true; break;
			case 11: lowerSegments[digitID].p = true; break;
			case 12: lowerSegments[digitID].q = true; break;
			}
		}
	}

	// Activating segments
	SegmentLCD_LowerSegments(lowerSegments);
}

void updateUpperLCD(void) {

	// Storing last displayed value of total shots
	static int8_t lastValue = -1;

	// Only printing the value if it has been changed
	if(lastValue != shotsTotal) {
		SegmentLCD_Number(shotsTotal);
		lastValue = shotsTotal;
	}
}

void updateSpinnerLCD(void) {

	for(int i = 0; i < 8; i++) {

		// Activating spinner LCD segments based on
		// the provided bitmask
		SegmentLCD_ARing(i, spinnerBitmask & (1<<i));
	}
}

void updateDisplay(void) {

	// Updating all parts of the LCD display.
	updateLowerLCD();
	updateUpperLCD();
	updateSpinnerLCD();
}

void prvGraphicsTask(void *prvParam) {

	while(1) {

		// Updating the display
		updateDisplay();

		// Delay for 20ms (50 Hz)
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}
