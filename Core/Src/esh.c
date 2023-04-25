#include "esh.h"
#include "usart.h"

#include <stdlib.h>

// == PRIVATE MACROS ==
#define CANCEL_AT(str, pos) {\
	for(int i = pos; i < (strlen(str) - pos); i++) {\
		str[i] = str[i + 1];\
	}\
	str[strlen(str) - 1] = '\0';\
}

#define INSERT_AT(str, pos, val) {\
	for(int i = strlen(str) + 1; i > pos; i--) {\
		str[i] = str[i - 1];\
	}\
	str[pos] = val;\
}


// == PRIVATE FUNCTIONS ==
void _esh_handle_enter(ESH_Shell* self)
{
	// first argument is command name
	ESH_Command cmd_fn = map_get(self->commands, self->cmd_args[0]);
	if (cmd_fn != NULL)
		cmd_fn(self, self->cmd_args_count + 1, self->cmd_args);
	else
		ESH_SEND_MSG(self, ESH_MSG_WRONG_CMD);

	// resetting
	for (int i = 0; i < self->cmd_args_count + 1; i++)
		for (int j = 0; j < strlen(self->cmd_args[i]); j++)
			self->cmd_args[i][j] = '\0';

	self->cmd_len_count = 0;
	self->cmd_args_count = 0;

	esh_start(self);
}

void _esh_handle_bcksp(ESH_Shell* self)
{
	if (self->cmd_len_pos == 0)
	{
		if (self->cmd_args_pos > 0)
		{
			self->cmd_args_count -= 1;
			self->cmd_args_pos -= 1;
			self->cmd_len_count = strlen(self->cmd_args[self->cmd_args_count]);
			self->curspos = self->cmd_len_count;
		}

		return;
	}

	// shorthands
	char* curr_arg = self->cmd_args[self->cmd_args_count];
	uint32_t start = self->curspos - 1;
	CANCEL_AT(curr_arg, start);

	curr_arg[len] = '\0';
	self->cmd_len_pos -= 1;
	self->cmd_len_count -= 1;

	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 1);
}

void _esh_handle_space(ESH_Shell* self)
{
	self->cmd_len_count = 0;
	self->cmd_args_count++;
	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 1);
}

void _esh_handle_arrsx(ESH_Shell* self)
{
	if (self->cmd_len_pos == 0)
	{
		if (self->cmd_args_pos > 0)
		{
			self->cmd_args_pos -= 1;
			self->cmd_len_pos = strlen(self->cmd_args[self->cmd_args_count]);
		}

		return;
	}

	self->cmd_len_pos -= 1;
	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 1);
}

void _esh_handle_arrdx(ESH_Shell* self)
{
	char* curr_arg = self->cmd_args[self->cmd_args_count];
	if (self->cmd_len_pos == strlen(curr_arg))
	{
		if (self->cmd_args_pos < self->cmd_args_count)
		{
			self->cmd_args_pos += 1;
			self->cmd_len_pos = 0;
		}

		return;
	}

	self->cmd_len_pos += 1;
	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 1);
}

// == PUBLIC FUNCTIONS ==
ESH_Shell* esh_create(UART_HandleTypeDef* huart)
{
	ESH_Shell* obj = (ESH_Shell*)malloc(sizeof(ESH_Shell));
	obj->huart = huart;
	obj->cmd_args_count = 0;
	obj->cmd_len_count = 0;
	obj->curspos = 0;
	obj->commands = map_create(ESH_MAX_CMDNUM);

	obj->cmd_args = calloc(ESH_MAX_ARGNUM, sizeof(char*));
	for (int i = 0; i < ESH_MAX_ARGNUM; i++)
		obj->cmd_args[i] = (char*)calloc(20, sizeof(char));

	return obj;
}

void esh_destroy(ESH_Shell* self)
{
	map_destroy(self->commands);

	for (int i = 0; i < ESH_MAX_ARGNUM; i++)
		free(self->cmd_args[i]);
	free(self->cmd_args);

	free(self);
}

void esh_add_cmd(ESH_Shell* self, char* name, ESH_Command fun)
{
	map_insert(self->commands, name, fun);
}

ESH_Command esh_get_cmd(ESH_Shell* self, char* name)
{
	return map_get(self->commands, name);
}

void esh_remove_cmd(ESH_Shell* self, char* name)
{
	map_delete(self->commands, name);
}

void esh_read(ESH_Shell* self)
{
	char* buf = self->buf;

	// Enter key pressed -> all command args have been written and sent
	if (*buf == ESH_KEY_ENTER)
		_esh_handle_enter(self);

	// Space key pressed -> previous argument completely written, starts another
	else if (*buf == ESH_KEY_SPACE)
		_esh_handle_space(self);

	// Backspace key entered -> do nothing, since cancellation is not implemented
	else if (*buf == ESH_KEY_BCKSP) {}

	// other characters -> add to command buffer and print real-time
	else
	{
		self->cmd_args[self->cmd_args_count][self->cmd_len_count] = *buf;
		self->cmd_len_count++;
		HAL_UART_Transmit_IT(self->huart, (uint8_t*)buf, 1);
	}
}

void esh_start(ESH_Shell* self)
{
	HAL_UART_Transmit_IT(self->huart, (uint8_t*)ESH_MSG_WELCOME, strlen(ESH_MSG_WELCOME));
}

void esh_wait_next(ESH_Shell* self)
{
	HAL_UART_Receive_IT(self->huart, (uint8_t*)self->buf, 1);
}
