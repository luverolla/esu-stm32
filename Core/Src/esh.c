#include "esh.h"
#include "usart.h"

#include <stdlib.h>

// == PRIVATE MACROS ==


// == PRIVATE FUNCTIONS ==
void CANCEL_AT(char* str, int pos) {
	for(int i = pos; i < (strlen(str) - 1); i++) {
		str[i] = str[i + 1];
	}
	str[strlen(str) - 1] = '\0';
}

void INSERT_AT(char* str, int pos, char val) {
	for(int i = strlen(str) + 1; i > pos; i--) {
		str[i] = str[i - 1];
	}
	str[pos] = val;
}

ESH_Result _esh_exec_command(ESH_Shell* self)
{
	int argc = 0;
	char argv[ESH_MAX_ARGNUM][ESH_MAX_ARGLEN];

	char* tok = strtok(self->cmd_string, " ");
	while (tok != NULL)
	{
		if (argc == ESH_MAX_ARGNUM || strlen(tok) > ESH_MAX_ARGLEN)
			return ESH_OVERFLOW;

		strcpy(argv[argc], tok);
		argc++;
		tok = strtok(NULL, " ");
	}

	// first argument is command name
	ESH_Command cmd_fn = map_get(self->commands, argv[0]);
	if (cmd_fn == NULL)
		return ESH_BADCMD;

	cmd_fn(argc, (char**)argv);
	return ESH_OK;
}

void _esh_handle_enter(ESH_Shell* self)
{
	ESH_Result res = _esh_exec_command(self);
	if (res != ESH_OK)
		ESH_SEND_MSG(self, ESH_ERRMSGS[res]);

	// resetting
	memset(self->cmd_string, 0, sizeof(self->cmd_string));
	ESH_Start(self);
}

void _esh_handle_bcksp(ESH_Shell* self)
{
	if (self->curspos == 0) {
		ESH_WaitNext(self);
		return;
	}

	CANCEL_AT(self->cmd_string, self->curspos - 1);

	self->cmd_len -= 1;
	self->curspos -= 1;

	*self->buf = ESH_KEY_BCKSP;
	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 1);
}

void _esh_handle_arrsx(ESH_Shell* self)
{
	if (self->curspos == 0) {
		ESH_WaitNext(self);
		return;
	}

	self->curspos -= 1;

	self->buf[0] = 0x1B;
	self->buf[1] = '[';
	self->buf[2] = 'D';

	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 3);
}

void _esh_handle_arrdx(ESH_Shell* self)
{
	if (self->curspos == self->cmd_len) {
		ESH_WaitNext(self);
		return;
	}

	self->curspos += 1;

	self->buf[0] = 0x1B;
	self->buf[1] = '[';
	self->buf[2] = 'C';

	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->buf, 3);
}

void _esh_handle_arrws(ESH_Shell* self)
{
	HAL_UART_Receive(self->huart, (uint8_t*)self->buf, 1, HAL_MAX_DELAY);

	// char 0 has already been checked, check that char 1 is [
	if (*self->buf == '[')
	{
		// now char 2
		HAL_UART_Receive(self->huart, (uint8_t*)self->buf, 1, HAL_MAX_DELAY);
		if (*self->buf == 'D')
			_esh_handle_arrsx(self);
		else if (*self->buf == 'C')
			_esh_handle_arrdx(self);
	}
}

// == PUBLIC FUNCTIONS ==
ESH_Shell* ESH_Init(UART_HandleTypeDef* huart, char* name)
{
	ESH_Shell* obj = (ESH_Shell*)malloc(sizeof(ESH_Shell));

	obj->huart = huart;
	obj->cmd_len = 0;
	obj->curspos = 0;
	obj->cmd_string[0] = '\0';
	obj->commands = map_create(ESH_MAX_CMDNUM);

	strcpy(obj->name, name);

	return obj;
}

void ESH_Deinit(ESH_Shell* self)
{
	map_destroy(self->commands);
	free(self);
}

void ESH_AddCommand(ESH_Shell* self, char* name, ESH_Command fun)
{
	map_insert(self->commands, name, fun);
}

ESH_Command ESH_GetCommand(ESH_Shell* self, char* name)
{
	return map_get(self->commands, name);
}

void ESH_RemoveCommand(ESH_Shell* self, char* name)
{
	map_delete(self->commands, name);
}

void ESH_Read(ESH_Shell* self)
{
	char* buf = self->buf;

	// check for arrows
	if (*buf == 0x1B)
		_esh_handle_arrws(self);

	// Enter key pressed -> command string is complete, now process it
	else if (*buf == ESH_KEY_ENTER)
		_esh_handle_enter(self);

	// Backspace key pressed -> cancel char at current cursor position
	else if (*buf == ESH_KEY_BCKSP)
		_esh_handle_bcksp(self);

	// other characters key pressed -> add to command buffer and print real-time
	else
	{
		INSERT_AT(self->cmd_string, self->curspos, *buf);
		self->cmd_len += 1;
		self->curspos += 1;
		HAL_UART_Transmit_IT(self->huart, (uint8_t*)buf, 1);
	}
}

void ESH_Start(ESH_Shell* self)
{
	self->cmd_len = 0;
	self->curspos = 0;

	sprintf(self->msg, ESH_PROMPTSTR, self->name);
	HAL_UART_Transmit_IT(self->huart, (uint8_t*)self->msg, strlen(self->msg));
}

void ESH_WaitNext(ESH_Shell* self)
{
	HAL_UART_Receive_IT(self->huart, (uint8_t*)self->buf, 1);
}
