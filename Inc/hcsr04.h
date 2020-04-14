/*
 * hcsr04.h
 *
 *  Created on: Jan 8, 2020
 *      Author: Iga
 */

#ifndef HCSR04_H_
#define HCSR04_H_

#include "stm32l4xx_hal.h"
#include "RGB_LED.h"
#include "UART.h"
#include <stdbool.h>
#define Trigg_Pin GPIO_PIN_4
#define Trigg_GPIO_Port GPIOC
#define Echo_Pin GPIO_PIN_5
#define Echo_GPIO_Port GPIOC

int get_init_signal_state();
void init_hcsr();
void distance();
void start_init_signal();
void init_singal_step();
void send_string(char *s);

#endif /* HCSR04_H_ */
