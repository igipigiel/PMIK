/*
 * RGB_LED.h
 *
 *  Created on: 1 gru 2019
 *      Author: Marcin
 */

#ifndef RGB_LED_H_
#define RGB_LED_H_

#include "stm32l4xx_hal.h"
#include <stdbool.h>

#define LED_COUNT 7
#define ZERO 0b0001
#define ONE 0b0011
#define COLOR_PICKER 0
#define STROBOTRON 1
#define RAINBOW 2
#define IDLE 3
#define FREQUENCY 25.0

typedef struct RGB {
	uint8_t red, green, blue;
} RGB;

void initializeLEDs(SPI_HandleTypeDef *hspiInit, float HInit, float SInit, float VInit);
void startRainbow();
void startColorPicker(int destR,int destG,int destB,float time);
void startStrobotron(float frequency);
void startIdle();
void stopIdle();
uint8_t getR();
uint8_t getG();
uint8_t getB();
void toggleLEDsActive();
void setIlluminance(float illuminance);
void setColorBit(uint32_t color);
void makeBufferFromHSV(float H, float S, float V);
void makeBufferFromRGB(uint8_t red, uint8_t green, uint8_t blue);
void sendBuffer();
bool toggleLamp();
void setLampActivation(bool state);
void makeRainbowStep();
void makeColorPickerStep();
int makeLampStep();

#endif /* RGB_LED_H_ */
