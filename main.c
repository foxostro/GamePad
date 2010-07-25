/* vim:ts=4:sw=4
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "GamePad.h"



int
main(int argc, const char * argv[])
{
	GamePad_State * gamepad_state = NULL;

	gamepad_state = GamePad_Init();

	printf("Initially, these gamepads were connected:\n");
	GamePad_DEBUG_DumpDevices(gamepad_state);
	
	printf("You have 10 seconds to disconnect or connect a device.\n");
	sleep(10);

	printf("Scanning...\n");
	GamePad_ScanForDevices(gamepad_state);
	
	printf("Now, these gamepads are connected:\n");
	GamePad_DEBUG_DumpDevices(gamepad_state);

	printf("Cleaning up.\n");
	GamePad_Destroy(gamepad_state);
	
	return EXIT_SUCCESS;
}

