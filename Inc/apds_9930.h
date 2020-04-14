/*
 * apds_9930.h
 *
 *  Created on: Jan 2, 2020
 *      Author: Iga
 */

#ifndef APDS_9930_H_
#define APDS_9930_H_

#include "stm32l4xx_hal.h"


#define  ENABLE         0x00
#define  ATIME          0x01
#define  PTIME          0x02
#define  WTIME          0x03
#define  AILTL          0x04
#define  AILTH          0x05
#define  AIHTL          0x06
#define  AIHTH          0x07
#define  PILTL          0x08
#define  PILTH          0x09
#define  PIHTL          0x0A
#define  PIHTH          0x0B
#define  PERS           0x0C
#define  CONFIG         0x0D
#define  PPULSE         0x0E
#define  CONTROL        0x0F
#define  ID             0x12
#define  STATUS         0x13
#define  Ch0DATAL       0x14
#define  Ch0DATAH       0x15
#define  Ch1DATAL       0x16
#define  Ch1DATAH       0x17
#define  PDATAL         0x18
#define  PDATAH         0x19
#define  POFFSET        0x1E

//COMMAND REGISTER
#define COM 			0x80
#define TYPE_special	0x60
#define TYPE_auto_inc  	0x20
#define FUN_none 		0x00
#define FUN_ALSITCLR 	0x06

// ENABLE bits
#define PON  			0x01  // enable device
#define AEN  			0x02  // enable ALS
#define PEN 			0x04  // enable proximity
#define WEN  			0x08  // wait enable
#define AIEN 			0x10  // ALS interrupt enable
#define PIEN 			0x20  // proximity interrupt enable
#define SAI  			0x40  // sleep after interrupt

//CONFIG bits
#define PDL  			0x01
#define WLONG 			0x02
#define AGL 			0x04

//CONTROL bits for setting ALS gain
#define AGAIN1  		0x00
#define AGAIN8  		0x01
#define AGAIN16  		0x02
#define AGAIN120  		0x03

//CONTROL bits for setting PROX gain
#define PGAIN1  		0x00
#define PGAIN2 			0x04
#define PGAIN4  		0x08
#define PGAIN18  		0x0C

//CONTROL bits for setting PDRIVE
#define PDRIVE100		0x00
#define PDRVIE50		0x40
#define PDRIVE25		0x80
#define PDRIVE12		0xC0

//interrupt clear
#define CLEAR_ALS_INT           0xE6
#define CLEAR_ALL_INTS          0xE7

/* Default values */
#define DEFAULT_ATIME           0xF6
#define DEFAULT_WTIME           0xB6
#define DEFAULT_PTIME           0xFF
#define DEFAULT_PPULSE          0x08
#define DEFAULT_POFFSET         0       // 0 offset
#define DEFAULT_CONFIG          0
#define DEFAULT_PDRIVE          0x00
#define DEFAULT_PDIODE          0x20
#define DEFAULT_PGAIN           0x0C
#define DEFAULT_AGAIN           AGAIN1
#define DEFAULT_PILT            0       // Low proximity threshold
#define DEFAULT_PIHT            50      // High proximity threshold
#define DEFAULT_AILT            0xFFFF  // Force interrupt for calibration
#define DEFAULT_AIHT            0
#define DEFAULT_PERS            0x22    // 2 consecutive prox or ALS for int.

/* ALS coefficients */
#define DF                      52
#define GA                      0.49
#define ALS_B                   1.862
#define ALS_C                   0.746
#define ALS_D                   1.291

/* Ranges for scaling results */
#define MAX_RESULT0				1500
#define MAX_RESULT1				400

/*  APDS functions  */
int APDS_init(I2C_HandleTypeDef *i2c) ;
uint16_t ALS_readCh0(I2C_HandleTypeDef *i2c) ;
uint16_t ALS_readCh1(I2C_HandleTypeDef *i2c) ;
float scale_results(uint16_t result0, uint16_t result1) ;
int APDS_enable(I2C_HandleTypeDef *i2c) ;
int APDS_disable(I2C_HandleTypeDef *i2c) ;
int set_ATIME(I2C_HandleTypeDef *i2c, uint8_t time) ;
int set_PTIME(I2C_HandleTypeDef *i2c);
int set_WTIME(I2C_HandleTypeDef *i2c, uint8_t time) ;
int set_ALS_thresholds(I2C_HandleTypeDef *i2c,uint8_t low1,uint8_t low2,uint8_t high1,uint8_t high2) ;
int set_PROX_thresholds(I2C_HandleTypeDef *i2c,uint8_t low1,uint8_t low2,uint8_t high1,uint8_t high2);
int set_PERS(I2C_HandleTypeDef *i2c,uint8_t PPERS,uint8_t APERS) ;
int set_AGL(I2C_HandleTypeDef *i2c, int enable) ;
int set_WLONG(I2C_HandleTypeDef *i2c,int enable) ;
int set_PDL(I2C_HandleTypeDef *i2c,int enable);
int set_PPULSE(I2C_HandleTypeDef *i2c, uint8_t pulse);
int set_Control(I2C_HandleTypeDef *i2c, uint8_t PDRIVE, uint8_t PDIODE, uint8_t PGAIN, uint8_t AGAIN) ;
int set_POFFSET(I2C_HandleTypeDef *i2c, uint8_t offset);
uint8_t read_status(I2C_HandleTypeDef *i2c);

/* Flags */

static int isEnabled ;


#endif /* APDS_9930_H_ */
