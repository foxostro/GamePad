/* vim:ts=4:sw=4
 *
 * Demo program for the GamePad library.
 */

#include <stdlib.h>
#include <stdio.h>

#include "GamePad.h"



int main(int argc, const char * argv[])
{
	GamePad_State * gamepad_state = NULL;

	gamepad_state = GamePad_Init();
	GamePad_DEBUG_DumpDevices(gamepad_state);
	GamePad_Destroy(gamepad_state);
	
	return EXIT_SUCCESS;
}

