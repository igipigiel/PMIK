/*
 * UART.c
 *
 *  Created on: 23 gru 2019
 *      Author: Marcin
 */
#include "UART.h"

static char dataToSend[100];
static UART_HandleTypeDef *huart;
char Received[20];
char Data[100];

static enum receive_flag {
	MODE, RGB_CODE, TIME, FREQ, NONE
} flag;

/*
 * Function initializes the UART library.
 */
void initUart(UART_HandleTypeDef *initHuart) {
	huart = initHuart;
}

/*
 * Function sends message through UART.
 * @param dataToSend table containing message string that will be sent
 * @param size size of the message string
 */
void sendMessage(char *dataToSend, uint16_t size) {
	HAL_UART_Transmit_IT(huart, (unsigned char*) dataToSend, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
}

/*
 * Function receives message from UART
 */
void receiveMessage() {
	HAL_UART_Receive_IT(huart, (unsigned char*) Received, CHAR_COUNT); // Ponowne włączenie nasłuchiwania
}

/*
 *  Function sends message through UART with information that the chosen color has been reached.
 *
 */
void sendEndColorPickerMessage() {
	uint16_t size;
	size = sprintf(Data, "Ustawiono wybrany kolor.\n\rPodaj kolejny kolor RGB w formacie HEX XXXXXX.\n\r");
	sendMessage(Data, size);
	flag = RGB_CODE;
	receiveMessage();
}

/*
 * Function handles receiving message with chosen lamp's mode by sending corresponding message and setting UART's mode flag.
 */
void receiveMode() {
	uint16_t size, receiveSize;
	switch (atoi(Received)) {
	case 0: // Jezeli odebrany zostanie znak 0
		size = sprintf(Data, "Wybrano tryb color picker.\n\rPodaj wybrany kolor RGB w formacie HEX XXXXXX.\n\r");
		flag = RGB_CODE;
		receiveSize = 10;
		break;
	case 1: // Jezeli odebrany zostanie znak 1
		size = sprintf(Data, "Wybrano tryb strobotron. Podaj czestotliwosc pracy trybu w Hz.\n\r");
		flag = FREQ;
		receiveSize = CHAR_COUNT;
		break;
	case 2: // Jezeli odebrany zostanie znak 2
		size = sprintf(Data, "Wybrano tryb rainbow.\n\r");
		startRainbow();
		flag = NONE;
		break;
	default: // Jezeli odebrano nieobslugiwany znak
		flag = MODE;
		receiveSize = 1;
		break;
	}
	sendMessage(Data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	if (flag != NONE)
		receiveMessage();
}

uint8_t R, G, B;

/*
 * Function handles receiving message with chosen RGB code by sending corresponding message and waiting for another response.
 */
uint32_t receiveRGBCode() {
	uint16_t size;
	uint32_t code = 0;
	code = (uint32_t) strtol(Received, NULL, 16);
	R = (code >> 4 * 4) % 0x100;
	G = (code >> 4 * 2) % 0x100;
	B = code % 0x100;
	size = sprintf(Data, "Wybrano kolor R=%d G=%d B=%d\n\rPodaj czas zmiany koloru w s.\n\r", R, G, B);
	sendMessage(Data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	flag = TIME;
	receiveMessage();
	return code;
}

/*
 * Function handles receiving message with numerical value, such as time or frequency.
 */
float receiveNumber() {
	float number = 0;
	number = atof(&Received);
	return number;
}

/*
 * Function handles receiving message with time value by sending corresponding message and starting the color picker lamp mode.
 */
void receiveTime() {
	uint16_t size;
	float time = receiveNumber();
	size = sprintf(Data, "Wybrano czas %.4fs.", time);
	sendMessage(Data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	startColorPicker(R, G, B, time);
	flag = NONE;
}

/*
 * Function handles receiving message with frequency value by sending corresponding message and starting the strobotron lamp mode.
 */
void receiveFrequency() {
	uint16_t size;
	float freq = receiveNumber();
	size = sprintf(Data, "Wybrano czestotliwosc %.4fHz.", freq);
	sendMessage(Data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	startStrobotron(freq);
	flag = NONE;
}

/*
 * Function starts the program by sending initial message through UART and waiting for a response.
 */
void startProgram() {
	int size;
	size =
			sprintf(dataToSend,
					"SMART LAMPA\n\rPodaj tryb pracy:\n\r0 - color picker\n\r1 - strobotron\n\r2 - rainbow\n\r"); // Stworzenie wiadomosci do wyslania oraz przypisanie ilosci wysylanych znakow do zmiennej size.
	sendMessage(dataToSend, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
	flag = MODE;
	receiveMessage();
}

/*
 * Function sends message through UART containg currently set RGB LEDs' color.
 */
void sendColorMessage() {
	int size;
	size = sprintf(Data, "Zatrzymano tryb na kolorze R=%d G=%d B=%d.\n\r", getR(), getG(), getB());
	sendMessage(Data, size); // Rozpoczecie nadawania danych z wykorzystaniem przerwan
}

/*
 * Function handles callback from the UART global interrupt.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	uint16_t size = 0; // Rozmiar wysylanej wiadomosci
	switch (flag) {
	case MODE:
		receiveMode();
		break;
	case RGB_CODE:
		receiveRGBCode();
		break;
	case TIME:
		receiveTime();
		break;
	case FREQ:
		receiveFrequency();
		break;
	default:
		break;
	}
}
