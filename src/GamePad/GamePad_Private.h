/* vim:ts=4:sw=4
 *
 * Private interface and declarations for the GamePad subsystem.
 */

#ifndef _GAMEPAD_PRIVATE_H_
#define _GAMEPAD_PRIVATE_H



#include "GamePad.h"

#define SERIAL_NUMBER_LEN (256)



/* state of the overall gamepad subsystem */
struct _GamePad_State
{
	IOHIDManagerRef hidManagerRef;
	CFMutableArrayRef gamepads;
};



/* state of a single GamePad device */
struct _GamePad_Device
{
	IOHIDDeviceRef dev;
	uint32_t uid;
	char sn[SERIAL_NUMBER_LEN];
};



/* Resets all fields in a GamePad_Device object. */
void GamePad_Device_Clear(GamePad_Device * gamepad);



/* Creates a gamepad device object from a HID device reference */
GamePad_Device * GamePad_Device_Init(IOHIDDeviceRef dev);



/* Cleanly release resources used by, and destroy, a GamePad_Device object. */
void GamePad_Device_Destroy(GamePad_Device * gamepad);



#endif /* _GAMEPAD_PRIVATE_H */

