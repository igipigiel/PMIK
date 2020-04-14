/*
 * UART.h
 *
 *  Created on: 23 gru 2019
 *      Author: Marcin
 */

#ifndef UART_H_
#define UART_H_

#include "stm32l4xx_hal.h"
#include "RGB_LED.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CHAR_COUNT 10

void initUart(UART_HandleTypeDef *initHuart);
void sendMessage(char *dataToSend, uint16_t size);
void receiveMessage();
void receiveMode();
void sendEndColorPickerMessage();
void sendColorMessage();
uint32_t receiveRGBCode();
float receiveNumber();
void startProgram();

#endif /* UART_H_ */
