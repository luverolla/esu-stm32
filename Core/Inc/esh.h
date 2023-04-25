#ifndef INC_ESH_H
#define INC_ESH_H

#include "usart.h"
#include "hashmap.h"
#include "string.h"

#include <stdio.h>

#define ESH_MAX_ARGNUM		20
#define ESH_MAX_ARGLEN		20
#define ESH_MAX_MSGLEN		200
#define ESH_MAX_CMDNUM		100

#define ESH_MSG_WELCOME		"\r\nWelcome! Type command\r\n"
#define ESH_MSG_WRONG_CMD	"\r\nWrong command. Retry\r\n"

#define ESH_KEY_SPACE		0x20
#define ESH_KEY_ENTER		0x0D
#define ESH_KEY_ARRSX		0x4B
#define ESH_KEY_ARRDX		0x4D
#define ESH_KEY_BCKSP		0x08

/**
 * Print a formatted string on UART associated with current shell instance
 * @param
 */
#define ESH_SEND_MSG(shell, template, ...) {\
	sprintf(prompt->msg, template, ##__VA_ARGS__);\
	HAL_UART_Transmit(shell->huart, (uint8_t*)shell->msg, strlen(shell->msg), HAL_MAX_DELAY);\
}

typedef struct ESH_Shell {
	UART_HandleTypeDef* huart;
	map_t* commands;
	char msg[ESH_MAX_MSGLEN];
	char buf[1];
	char** cmd_args;
	uint32_t cmd_args_pos;
	uint32_t cmd_len_pos;
	uint32_t cmd_args_count;
	uint32_t cmd_len_count;
} ESH_Shell;

typedef void (*ESH_Command) (int, char**);

ESH_Shell* ESH_Init(UART_HandleTypeDef* huart);
void ESH_Deinit(ESH_Shell* self);

void ESH_AddCommand(ESH_Shell* self, char* name, ESH_Command fun);
ESH_Command ESH_GetCommand(ESH_Shell* self, char* name);
void ESH_RemoveCommand(ESH_Shell* self, char* name);

void ESH_Read(ESH_Shell* self);
void ESH_Start(ESH_Shell* self);
void ESH_WaitNext(ESH_Shell* self);


#endif
