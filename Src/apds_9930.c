/*
 * apds_9930.c
 *
 *  Created on: Jan 2, 2020
 *      Author: Iga
 */

#include "apds_9930.h"


/*
 * Initializes APDS light sensor with default values
 * @param i2c pointer to I2C handle
 * @return ifSuccessful
 */
int APDS_init(I2C_HandleTypeDef *i2c) {

	APDS_disable(i2c) ;
	if (set_ATIME(i2c,DEFAULT_ATIME) < 0) return -1 ;
	if (set_PTIME(i2c)  < 0) return -1 ;
	if (set_WTIME(i2c, DEFAULT_WTIME) < 0) return -1 ;
	if (set_ALS_thresholds(i2c,0x00,0x00,0xff,0xff) < 0) return -1 ;
	if (set_PROX_thresholds(i2c,0x00,0x00,0xff,0xff) <0) return -1 ;
	if (set_PERS(i2c,0x01,0x01) <0) return -1 ;
	if ( set_AGL(i2c,0) < 0 ) return -1 ;
	if ( set_WLONG(i2c,0) < 0 ) return -1 ;
	if ( set_PDL(i2c,0) < 0 ) return -1 ;
	if ( set_PPULSE ( i2c, DEFAULT_PPULSE) < 0 ) return -1 ;
	if (set_Control( i2c, PDRIVE100, DEFAULT_PDIODE, DEFAULT_PGAIN, DEFAULT_AGAIN ) < 0 ) return -1 ;
	if (set_POFFSET(i2c, DEFAULT_POFFSET) < 0) return -1 ;
	APDS_enable(i2c) ;
	return 1 ;
}

/*
 * Reads the value in register for channel Ch0 - measurement result
 *  @param i2c pointer to I2C handle
 *  @return measured value
 */
uint16_t ALS_readCh0(I2C_HandleTypeDef *i2c){

	uint8_t CH0low, CH0high;

	if (!isEnabled || HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | STATUS, 1, (uint8_t*)&CH0low, sizeof(CH0low), HAL_MAX_DELAY) ;
	while (	(CH0low & 0x01) != 0x01 ) {
		HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | STATUS, 1, (uint8_t*)&CH0low, sizeof(CH0low), HAL_MAX_DELAY) ;
	}

	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | Ch0DATAL, 1, (uint8_t*)&CH0low, sizeof(CH0low), HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | Ch0DATAH, 1, (uint8_t*)&CH0high, sizeof(CH0high), HAL_MAX_DELAY);

	return CH0low + 256*CH0high;


}
/*
 * Reads the value in register for channel Ch01- measurement result
 *  @param i2c pointer to I2C handle
 *  @return measured value
 */

uint16_t ALS_readCh1(I2C_HandleTypeDef *i2c){

	uint8_t CH1low, CH1high;

	if (!isEnabled || HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | STATUS, 1, (uint8_t*)&CH1low, sizeof(CH1low), HAL_MAX_DELAY) ;
		while (	(CH1low & 0x01) != 0x01 ) {
			HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | STATUS, 1, (uint8_t*)&CH1low, sizeof(CH1low), HAL_MAX_DELAY) ;
		}
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | Ch1DATAL, 1, (uint8_t*)&CH1low, sizeof(CH1low), HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0xA0 | Ch1DATAH, 1, (uint8_t*)&CH1high, sizeof(CH1high), HAL_MAX_DELAY);

	return CH1low + 256*CH1high;


}
/*
 * Function scales measured values to the range <0;1>, calculates the average and the illuminance to set
 * @param result0 from channel 0
 * @param result1 from channel 1
 * @return illuminance to set
 */

float scale_results(uint16_t result0, uint16_t result1) {

	float brightness ;
	float scaled0 = ( (float) result0 ) / MAX_RESULT0 ;
	if (result0 > MAX_RESULT0) scaled0 = 1 ;
	float scaled1 = ((float) result1 ) / MAX_RESULT1 ;
	if (result1 > MAX_RESULT1) scaled1 = 1 ;

	brightness  = (scaled0 + scaled1)/2 + 0.05 ;
	if (brightness > 0.92) brightness = 0.92 ;
	return brightness ;

}

/*
 * Enables the APDS sensor in the ambient light mode
 * @param i2c pointer to I2C handle
 * @return ifSuccessful
 */

int APDS_enable(I2C_HandleTypeDef *i2c) {

	uint8_t reg  ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = PON | AEN | WEN ;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | 0x00, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	isEnabled = 1;
	return 0 ;
}

/*
 * Disables the APDS sensor
 * @param i2c pointer to I2C handle
 * @return ifSuccessful
 */

int APDS_disable(I2C_HandleTypeDef *i2c) {
	uint8_t reg  ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = 0x00;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | 0x00, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	isEnabled = 0 ;
	return 0 ;
}

/*
 * Sets the integration time for ambient light in APDS
 * @param i2c pointer to I2C handle
 * @return ifSuccessful
 */

int set_ATIME(I2C_HandleTypeDef *i2c, uint8_t time) {

	uint8_t reg  ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = time ;
	/*if (time == 2.73) reg = 0xff ;
	else if (time == 27.3) reg = 0xf6 ;
		else if (time == 101) reg = 0xDB ;
			else if (time == 175) reg = 0xC0 ;
				else if (time == 699) reg = 0x00;
				else return -1 ;*/

	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | ATIME, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Sets the integration time for proximity measurement in APDS
 * @param i2c pointer to I2C handle
 * @return ifSuccessful
 */

int set_PTIME(I2C_HandleTypeDef *i2c){
	uint8_t reg  ;
		if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
		reg = 0xff;
		HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PTIME, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
		return 0 ;
}

/*
 * Sets the wait time in APDS
 * @param i2c pointer to I2C handle
 * @param time to set
 * @return ifSuccessful
 */

int set_WTIME(I2C_HandleTypeDef *i2c, uint8_t time) {

	uint8_t reg  ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = time;
	/*if (time == 2.73) reg = 0xff ;
	else if (time == 202) reg = 0xB6 ;
		else if (time == 699) reg = 0x00;*/

	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | WTIME, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Sets the interrupt thresholds time for ambient light in APDS
 * @param i2c pointer to I2C handle
 * @param low1 threshold lower byte
 * @param low2 threshold upper byte
 * @param high1 threshold lower byte
 * @param high2 threshold upper byte
 * @return if successful
 */


int set_ALS_thresholds(I2C_HandleTypeDef *i2c,uint8_t low1,uint8_t low2,
		uint8_t high1,uint8_t high2) {

	uint8_t reg ;

	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = low1;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | AILTL, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	reg = low2;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | AILTH, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	reg = high1;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | AIHTL, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	reg = high2;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | AIHTH, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0;
}

/*
 * Sets the interrupt thresholds time for proximity in APDS
 * @param i2c pointer to I2C handle
 * @param low1 threshold lower byte
 * @param low2 threshold upper byte
 * @param high1 threshold lower byte
 * @param high2 threshold upper byte
 * @return if successful
 */

int set_PROX_thresholds(I2C_HandleTypeDef *i2c,uint8_t low1,uint8_t low2,
		uint8_t high1,uint8_t high2) {

	uint8_t reg ;

	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = low1;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PILTL, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	reg = low2;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PILTH, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	reg = high1;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PIHTL, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	reg = high2;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PIHTH, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0;
}

/*
 * Sets the interrupt persistance in APDS
 * @param i2c pointer to I2C handle
 * @param PPERS proximity persistence
 * @param APERS ambient light persistence
 * @return if successful
 */
int set_PERS(I2C_HandleTypeDef *i2c,uint8_t PPERS,uint8_t APERS) {

	uint8_t reg ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = (PPERS <<4) | APERS;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PERS, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Sets the AGL bit
 * @param i2c pointer to I2C handle
 * @param enable
 * @return if successful
 */
int set_AGL(I2C_HandleTypeDef *i2c, int enable) {
	uint8_t reg = 0 ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0x80 | CONFIG, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	if (enable == 1) reg = reg | AGL ;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | CONFIG, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Sets the WLONG bit for longer wait time
 * @param i2c pointer to I2C handle
 * @param enable
 * @return if successful
 */

int set_WLONG(I2C_HandleTypeDef *i2c, int enable) {
	uint8_t reg = 0 ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0x80 | CONFIG, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	if (enable == 1) reg = reg| WLONG ;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | CONFIG, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Sets the PDL bit
 * @param i2c pointer to I2C handle
 * @param enable
 * @return if successful
 */
int set_PDL(I2C_HandleTypeDef *i2c, int enable) {
	uint8_t reg = 0;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	HAL_I2C_Mem_Read(i2c, 0x39<<1, 0x80 | CONFIG, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	if (enable == 1) reg = reg | PDL ;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | CONFIG, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Sets the proximity pulse value
 * @param i2c pointer to I2C handle
 * @param pulse
 * @return if successful
 */
int set_PPULSE(I2C_HandleTypeDef *i2c, uint8_t pulse){

	uint8_t reg  ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = pulse ;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | PPULSE, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;
}

/*
 * Write the value to the Control register
 * @param i2c pointer to I2C handle
 * @param enable
 * @return if successful
 */
int set_Control(I2C_HandleTypeDef *i2c, uint8_t PDRIVE,uint8_t PDIODE,
		uint8_t PGAIN, uint8_t AGAIN) {

	uint8_t reg  ;
		if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
		reg = PDRIVE | PDIODE | PGAIN | AGAIN ;
		HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | CONTROL, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
		return 0 ;

}

/*
 * Sets the proximity offset
 * @param i2c pointer to I2C handle
 * @param offset
 * @return if successful
 */

int set_POFFSET(I2C_HandleTypeDef *i2c, uint8_t offset){

	uint8_t reg  ;
	if (HAL_I2C_IsDeviceReady(i2c,0x39<<1,3,HAL_MAX_DELAY) != HAL_OK) return -1 ;
	reg = offset ;
	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | POFFSET, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return 0 ;

}

/*
 * Reads the value in the Status register
 * @param i2c pointer to I2C handle
 * @return value of the register
 */

uint8_t read_status(I2C_HandleTypeDef *i2c){

	uint8_t reg = 0x00;

	HAL_I2C_Mem_Write(i2c, 0x39<<1, 0x80 | STATUS, 1, (uint8_t*)&reg, sizeof(reg), HAL_MAX_DELAY);
	return reg ;

}

