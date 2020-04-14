/*
 * RGB_LED.c
 *
 *  Created on: 1 gru 2019
 *      Author: Marcin
 */
#include "RGB_LED.h"

static bool isActive = true;
static uint8_t buffer[24 * LED_COUNT];
static uint16_t buffIdx;
static SPI_HandleTypeDef *hspi;
static float H;
static float S;
static float V;
static float goalV;
static float R;
static float G;
static float B;
static int goalR;
static int goalG;
static int goalB;
static float dR;
static float dG;
static float dB;
static int mode = IDLE;
static int prevMode = IDLE;
static float strobFreq;
static int strobCounter = 0;

/*
 * Getter for value of red color, which is displayed on the LEDs
 * @return red color's value
 */
uint8_t getR() {
	return (uint8_t) R;
}

/*
 * Getter for value of green color, which is displayed on the LEDs.
 * @return green color's value
 */
uint8_t getG() {
	return (uint8_t) G;
}

/*
 * Getter for value of blue color, which is displayed on the LEDs
 * @return blue color's value
 */
uint8_t getB() {
	return (uint8_t) B;
}

/*
 * Function sets needed variables and display initial color given in HSV format.
 */
void initializeLEDs(SPI_HandleTypeDef *hspiInit, float HInit, float SInit,
		float VInit) {
	hspi = hspiInit;
	H = HInit;
	S = SInit;
	V = VInit;
	goalV = VInit;
	makeBufferFromHSV(H, S, V);
	sendBuffer();
}

/*
 * Function starts the rainbow mode,
 * which goes through every color on repeat
 */
void startRainbow() {
	V = 0.7;
	mode = RAINBOW;
	prevMode = RAINBOW;
}

/*
 * Function starts the color picker mode, which changes displayed color to chosen one in given time.
 * @param destR chosen value for red color
 * @param destG chosen value for green color
 * @param destB chosen value for blue color
 * @param time chosen time, in which LEDs will change their color
 */
void startColorPicker(int destR, int destG, int destB, float time) {
	goalR = destR;
	goalG = destG;
	goalB = destB;
	dR = (goalR - R) / (time * FREQUENCY);
	dG = (goalG - G) / (time * FREQUENCY);
	dB = (goalB - B) / (time * FREQUENCY);
	mode = COLOR_PICKER;
}

/*
 * Function starts the strobotron mode, which changes displayed color to random one on repeat with given frequency.
 * @param frequency chosen frequency in which colors will change
 */
void startStrobotron(float frequency) {
	V = 0.7;
	strobFreq = frequency;
	mode = STROBOTRON;
}

/*
 * Function starts idle mode.
 */
void startIdle() {
	if (mode != IDLE) {
		prevMode = mode;
		mode = IDLE;
	}
}

/*
 * Function stops idle mode.
 */
void stopIdle() {
	if (mode == IDLE)
		mode = prevMode;
}

/*
 * Function stops currently displayed mode.
 */
void toggleLEDsActive() {
	isActive = !isActive;
}

/*
 * Function sets illuminance in strobotron and rainbow mode to change lamp's brightness
 * @param illuminance value of illuminance between 0 and 1 used to change lamp's brightness
 */
void setIlluminance(float illuminance) {
	goalV = illuminance;
}

/*
 * Function sets one bit (ZERO or ONE value) of the color in RGB format in the SPI buffer
 * @param color value of the color that is being set to the buffer
 */
void setColorBit(uint32_t color) {
	--buffIdx;
	if (color % 2 == 0)
		buffer[buffIdx] = ZERO;
	else
		buffer[buffIdx] = ONE;
}

/*
 * Function converts color from HSV format to RGB format and sets the result to global variables
 * @param H hue of the color in HSV format
 * @param S saturation of the color in HSV format
 * @param V value of the color in HSV format
 */
void HSVtoRGB(float H, float S, float V) {
	float R_p, G_p, B_p;
	float C = V * S;
	float mod = H / 60.0;
	while (mod > 0)
		mod -= 2;
	mod += 1;
	if (mod < 0)
		mod *= -1;
	float X = C * (1 - mod);
	float m = V - C;
	if (H >= 0 && H < 60) {
		R_p = C;
		G_p = X;
		B_p = 0;
	} else if (H >= 60 && H < 120) {
		R_p = X;
		G_p = C;
		B_p = 0;
	} else if (H >= 120 && H < 180) {
		R_p = 0;
		G_p = C;
		B_p = X;
	} else if (H >= 180 && H < 240) {
		R_p = 0;
		G_p = X;
		B_p = C;
	} else if (H >= 240 && H < 300) {
		R_p = X;
		G_p = 0;
		B_p = C;
	} else if (H >= 300 && H < 360) {
		R_p = C;
		G_p = 0;
		B_p = X;
	} else
		return;

	R = (uint8_t) ((R_p + m) * 255);
	G = (uint8_t) ((G_p + m) * 255);
	B = (uint8_t) ((B_p + m) * 255);
}

/*
 * Function sets the SPI buffer with color given in HSV format
 * @param H hue of the color in HSV format
 * @param S saturation of the color in HSV format
 * @param V value of the color in HSV format
 */
void makeBufferFromHSV(float H, float S, float V) {
	HSVtoRGB(H, S, V);
	makeBufferFromRGB((uint8_t) R, (uint8_t) G, (uint8_t) B);
}

/*
 * Function sets the SPI buffer with color given in RGB format
 * @param red chosen value for red color
 * @param green chosen value for green color
 * @param blue chosen value for blue color
 */
void makeBufferFromRGB(uint8_t red, uint8_t green, uint8_t blue) {
	uint16_t j = 0;
	uint16_t t = 0;
	buffIdx = 24 * LED_COUNT;
	uint32_t GRB_bits = (green << 16) + (red << 8) + blue;
	for (j = 0; j < LED_COUNT; ++j) {
		for (t = 0; t < 24; ++t) {
			setColorBit(GRB_bits >> t);
		}
	}
	t++;
}

/*
 * Function sends buffer to the LEDs' driver using SPI interface
 */
void sendBuffer() {
	HAL_SPI_Transmit(hspi, buffer, 24 * LED_COUNT, 100);
}

/*
 * Function makes one step in the rainbow mode - changes H and V values and sends new color to the driver
 */
void makeRainbowStep() {
	H += 2;
	if (V - goalV < -0.05)
		V += 0.05;
	else if (V - goalV > 0.05)
		V -= 0.05;

	if (H > 360)
		H = 0;
	makeBufferFromHSV(H, S, V);
	sendBuffer();
}

/*
 * Function makes one step in the color picker mode - checks, if the color was reached and if not, sets color closer to the goal value on the LEDs.
 */
void makeColorPickerStep() {
	if ((dR != 0
			&& ((goalR - R >= 0 && goalR - R < dR)
					|| (goalR - R <= 0 && goalR - R > dR)))
			|| (dG != 0
					&& ((goalG - G >= 0 && goalG - G < dG)
							|| (goalG - G <= 0 && goalG - G > dG)))
			|| (dB != 0
					&& ((goalB - B >= 0 && goalB - B < dB)
							|| (goalB - B <= 0 && goalB - B > dB)))
			|| (dR == 0 && dG == 0 && dB == 0)) {
		R = goalR;
		G = goalG;
		B = goalB;
		mode = IDLE;
	} else {
		R += dR;
		G += dG;
		B += dB;

		if (R > 255)
			R = 255;
		else if (R < 0)
			R = 0;
		if (G > 255)
			G = 255;
		else if (G < 0)
			G = 0;
		if (B > 255)
			B = 255;
		else if (B < 0)
			B = 0;
	}
	makeBufferFromRGB((uint8_t) R, (uint8_t) G, (uint8_t) B);
	sendBuffer();

}

/*
 * Function makes one step in the strobotron mode - checks, if the time has passed and if yes, sets random color on the LEDs.
 */
void makeStrobotronStep() {
	if (++strobCounter > FREQUENCY / strobFreq) {
		strobCounter = 0;
		H = rand() % 360;
		makeBufferFromHSV(H, S, V);
		sendBuffer();
	}
}

/*
 * Function makes one step in the idle mode - switches LEDs off, if they are still on.
 */
void makeIdleStep() {
	if (V != 0) {
		V = 0;
		makeBufferFromHSV(H, S, V);
		sendBuffer();
	}
}

/*
 * Function changes lamp's activation mode - false value stops lamp's work.
 * return new lamp activation mode
 */
bool toggleLamp() {
	isActive = !isActive;
	return isActive;
}

/*
 * Function sets lamp's actvation mode - false value stops lamp's work.
 * @param state new activation mode
 */
void setLampActivation(bool state) {
	isActive = state;
}

/*
 * Function makes one step of the RGB LED programme - checks, if conditions are achieved and if yes, sets color to the RGB LEDs.
 */
int makeLampStep() {
	if (!isActive)
		return;
	switch (mode) {
	case RAINBOW:
		makeRainbowStep();
		break;

	case COLOR_PICKER:
		makeColorPickerStep();
		break;

	case STROBOTRON:
		makeStrobotronStep();
		break;

	case IDLE:
		makeIdleStep();
		break;
	}
	return mode;
}
