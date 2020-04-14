#ifndef __STM32F4_WS2812_H
#define __STM32F4_WS2812_H
//--------------------------------------------------------------
#include "stm32l4xx.h"
/* TIM3 CH1 connected by default to pin PC6 => DMA1 on Channel5 Stream4 */
//--------------------------------------------------------------
#define WS2812_TIM3_CH1
#define WS2812_TIM3_CH1_CON_DIODES    8 /* Number of connected diodes */
//--------------------------------------------------------------
#define WS2812_TIM3          TIM3
//--------------------------------------------------------------
/* @brief: RGB color structure */
typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
}WS2812_RGB_Color_t;
//--------------------------------------------------------------
#define HSV_YELLOW 	60
#define HSV_GREEN 	120
#define HSV_CYJAN 	180
#define HSV_BLUE		240
#define HSV_MAGENTA 300
//--------------------------------------------------------------
/* @brief: HSV color structure */
typedef struct {
  uint16_t hue;     		/* from 0 to 359 */
  uint8_t saturation;   /* 0...100 */
  uint8_t value;      	/* 0...100 */
}WS2812_HSV_Color_t;
//--------------------------------------------------------------
#ifdef WS2812_TIM3_CH1
	extern WS2812_RGB_Color_t WS2812_LED_BUF_CH1[WS2812_TIM3_CH1_CON_DIODES];
#endif
//--------------------------------------------------------------
#define  WS2812_RGB_COL_OFF      (WS2812_RGB_Color_t) {0x00,0x00,0x00}

#define  WS2812_RGB_COLOR_BLUE     (WS2812_RGB_Color_t) {0x00,0x00,0xFF}
#define  WS2812_RGB_COLOR_GREEN    (WS2812_RGB_Color_t) {0x00,0xFF,0x00}
#define  WS2812_RGB_COLOR_RED      (WS2812_RGB_Color_t) {0xFF,0x00,0x00}
#define  WS2812_RGB_COLOR_WHITE    (WS2812_RGB_Color_t) {0xFF,0xFF,0xFF}
#define  WS2812_RGB_COLOR_CYAN     (WS2812_RGB_Color_t) {0x00,0xFF,0xFF}
#define  WS2812_RGB_COLOR_MAGENTA  (WS2812_RGB_Color_t) {0xFF,0x00,0xFF}
#define  WS2812_RGB_COLOR_YELLOW   (WS2812_RGB_Color_t) {0xFF,0xFF,0x00}
//--------------------------------------------------------------
#define  WS2812_HSV_COLOR_NONE      (WS2812_HSV_Color_t) {0,  0,  0}

#define  WS2812_HSV_COLOR_BLUE     (WS2812_HSV_Color_t) {240,100,100}
#define  WS2812_HSV_COLOR_GREEN    (WS2812_HSV_Color_t) {120,100,100}
#define  WS2812_HSV_COLOR_RED      (WS2812_HSV_Color_t) {0,  100,100}
#define  WS2812_HSV_COLOR_CYAN     (WS2812_HSV_Color_t) {180,100,100}
#define  WS2812_HSV_COLOR_MAGENTA  (WS2812_HSV_Color_t) {300,100,100}
#define  WS2812_HSV_COLOR_YELLOW   (WS2812_HSV_Color_t) {60, 100,100}
//--------------------------------------------------------------
/*
	* Timings for WS2812
	* LO impuls: send high state for 0.35us and low state for 80us
	* HI impuls: send high state for 0.80us and low state for 35us
	* For timer 3 period and prescale can be from 0 to 0xFFFF
	* Clock is 2*APB1 => 2*42MHz => 84MHz
	* Pwm frequecy calculate as TIM-CLK/(period+1)/(prescale+1)
*/
#define  WS2812_TIM_PRESCALE    0  /* Max clock timer what equals 84MHz so 11.9ns */
#define  WS2812_TIM_PERIODE   	104
#define  WS2812_LOW_TIME        27 /* Low for 30 * 11.9ns */
#define  WS2812_HIGH_TIME       60 /* high for 67 * 11.9ns */
//--------------------------------------------------------------
#define  WS2812_ONE_LED_BITS   24	/* @brief: number of bits per one led */
#define  WS2812_PAUSE      		 2  /* @brief: pouse after send */
//--------------------------------------------------------------
/* @brief: buffer size */
#define  WS2812_TIM3_CH1_BUF_LENGTH   (WS2812_TIM3_CH1_CON_DIODES+WS2812_PAUSE)*WS2812_ONE_LED_BITS
//--------------------------------------------------------------
typedef struct{
	WS2812_RGB_Color_t *rgbPointer;
	uint8_t ws2812Channel;
	uint32_t countMax;
	uint32_t ws2812DmaStatus;
}WS2812_DiodeParam_t;
//--------------------------------------------------------------
#ifdef WS2812_TIM3_CH1
	extern WS2812_DiodeParam_t TIM3_CH1_t;
#endif
//--------------------------------------------------------------
/* @brief: Initialization of Diodes */
void WS2812InitDiodes(void);
/* @brief: Set single led with pass RGB color */
void WS2812InitSingleLedRGB(WS2812_DiodeParam_t * diodeStruct, uint32_t diodeNumber,
																											WS2812_RGB_Color_t rgb_color, uint8_t refreshDiodes);
/* @brief: Set all leds with pass RGB color */
void WS2812InitAllLedsRGB(WS2812_DiodeParam_t * diodeStruct,
																											WS2812_RGB_Color_t rgb_color, uint8_t refreshDiodes);
/* @brief: Set single led with HSV color */
void WS2812InitSingleLedHSV(WS2812_DiodeParam_t * diodeStruct, uint32_t diodeNumber,
																											WS2812_HSV_Color_t hsvStruct, uint8_t refreshDiodes);
/* @brief: Set all leds with HSV color */
void WS2812InitAllLedsHSV(WS2812_DiodeParam_t * diodeStruct,
																											WS2812_HSV_Color_t hsvStruct, uint8_t refreshDiodes);
/* @brief: Move diode color to left */
void WS2812MoveDiodeLeft(WS2812_DiodeParam_t * diodeStruct);
/* @brief: Move diode color to right */
void WS2812MoveDiodeRight(WS2812_DiodeParam_t * diodeStruct);
/* @brief: Rotate diodes to left */
void WS2812RotateToLeft(WS2812_DiodeParam_t * diodeStruct);
/* @brief: Rotate diodes to right */
void WS2812RotateToRight(WS2812_DiodeParam_t * diodeStruct);
/* @brief: Refresh diodes */
void WS2812RefreshDiodes(void);
/* @brief: Convert color value in RGB into HSV value */
void WS2812ConvertRGBToHSV(WS2812_HSV_Color_t hsv_color, WS2812_RGB_Color_t *rgb_color);

#endif
