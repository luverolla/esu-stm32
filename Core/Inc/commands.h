#ifndef INC_COMMANDS_H
#define INC_COMMANDS_H

#include <esh.h>
#include <stdlib.h>
#include <string.h>

/*
 * add commands here as
 * void <command_function_name>(ESH_Shell shell, int argc, char** argv);
 */

void turn_on_led(ESH_Shell* shell, int argc, char** argv);
void turn_off_led(ESH_Shell* shell, int argc, char** argv);
void toggle_led(ESH_Shell* shell, int argc, char** argv);
void blink_led(ESH_Shell* shell, int argc, char** argv);
void pwm_led(ESH_Shell* shell, int argc, char** argv);

#endif
