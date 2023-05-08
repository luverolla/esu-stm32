#ifndef INC_ESH_H
#define INC_ESH_H

#include "usart.h"
#include "hashmap.h"
#include "string.h"

#include <stdio.h>

#define ESH_MAX_ARGNUM		20
#define ESH_MAX_ARGLEN		20
#define ESH_MAX_MSGLEN		200
#define ESH_MAX_NAMELEN		20
#define ESH_MAX_CMDNUM		100

#define ESH_PROMPTSTR		"\r\n%s> "

#define ESH_KEY_SPACE		0x20
#define ESH_KEY_ENTER		0x0D
#define ESH_KEY_ARRSX		0x25
#define ESH_KEY_ARRDX		0x27
#define ESH_KEY_BCKSP		0x08

/**
 * Print a formatted string on UART associated with current shell instance
 * @param
 */
#define ESH_SEND_MSG(shell, template, ...) {\
	sprintf(shell->msg, template, ##__VA_ARGS__);\
	HAL_UART_Transmit(shell->huart, (uint8_t*)shell->msg, strlen(shell->msg), HAL_MAX_DELAY);\
}

typedef enum ESH_Result {
	ESH_OK = 0,
	ESH_BADCMD = 1,
	ESH_OVERFLOW = 2
} ESH_Result;

static const char* ESH_ERRMSGS[] = {
	"Ok",
	"Wrong or undefined command",
	"Command overflows maximum number of arguments or arguments' length"
};

typedef struct ESH_Shell {
	UART_HandleTypeDef* huart;
	map_t* commands;
	char name[ESH_MAX_NAMELEN];
	char msg[ESH_MAX_MSGLEN];
	char buf[3];
	char cmd_string[ESH_MAX_ARGNUM * ESH_MAX_ARGLEN];
	uint32_t cmd_len;
	uint32_t curspos;
} ESH_Shell;

typedef void (*ESH_Command) (int, char**);

ESH_Shell* ESH_Init(UART_HandleTypeDef* huart, char* name);
void ESH_Deinit(ESH_Shell* self);

void ESH_AddCommand(ESH_Shell* self, char* name, ESH_Command fun);
ESH_Command ESH_GetCommand(ESH_Shell* self, char* name);
void ESH_RemoveCommand(ESH_Shell* self, char* name);

void ESH_Read(ESH_Shell* self);
void ESH_Start(ESH_Shell* self);
void ESH_WaitNext(ESH_Shell* self);


#endif
