/*
 * hcsr04.c
 *
 *  Created on: Jan 8, 2020
 *      Author: Iga
 */
#include "hcsr04.h"

GPIO_PinState last;
static int startHcsr;
static int counter1, counter2;
static int not_detected_counter;
static int detected_counter;

/*
 * Initializes the sensor
 */

void init_hcsr() {
	startHcsr = 0;
	counter1 = 0;
	counter2 = 0;
	detected_counter = 0;
	not_detected_counter = 0;
}

/*
 * Getter for the value of the start signal
 * @return start state
 */
int get_init_signal_state() {
	return startHcsr;
}

/*
 * Starts the init signal for the sensor
 */
void start_init_signal() {
	startHcsr = 1;
}

/*
 * Generates the init signal for the sensor
 */

void init_singal_step() {

	if (startHcsr == 1) {
		startHcsr = 2;
		HAL_GPIO_WritePin(Trigg_GPIO_Port, Trigg_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(Trigg_GPIO_Port, Trigg_Pin, GPIO_PIN_RESET);
	}
}

/*
 * Measures the distance between an object and the sensor
 * writes it in counter2 and after 5 measurements without an object in range stops the lamp
 * */
void distance() {

	GPIO_PinState current;
	if ((current = HAL_GPIO_ReadPin(Trigg_GPIO_Port, Echo_Pin))
			== GPIO_PIN_SET) {
		++counter1;
	} else if (last == GPIO_PIN_SET) {
		counter2 = counter1;
		counter1 = 0;

		if (counter2 > 4 && counter2 < 600) {
			not_detected_counter = 0;
			++detected_counter;
		} else {
			detected_counter = 0;
			++not_detected_counter;
		}

		if (not_detected_counter > 50) {
			startIdle();
			// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			not_detected_counter = 0;
		}
		if (detected_counter > 2) { //2
			stopIdle();
//			sendColorMessage();
			// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			detected_counter = 0;
		}
//		if (counter2 > 14 && counter2 < 600) {
//
//		} else setIlluminance(0) ;
		startHcsr = 0;
		//HAL_UART_Transmit_IT(&huart2, str, size);
	}
	last = current;
}

