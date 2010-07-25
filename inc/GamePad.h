/* vim:ts=4:sw=4
 *
 * Public interface for the GamePad subsystem.
 */

#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_



struct _GamePad_Device;
typedef struct _GamePad_Device GamePad_Device;

struct _GamePad_State;
typedef struct _GamePad_State GamePad_State;



/* Scans the system for gamepads and rebuild the list of devices. */
void GamePad_ScanForDevices(GamePad_State * state);



/* Initializes the GamePad subsytem and returns the subsystem state. */
GamePad_State * GamePad_Init(void);



/* Releases resources and destroys the state for the GamePad subsystem */
void GamePad_Destroy(GamePad_State * state);



/* Dump debugging information for a single device */
void GamePad_DEBUG_DumpDevice(GamePad_Device * gamepad);



/* Dumps debugging information about each GamePad that is connected. */
void GamePad_DEBUG_DumpDevices(GamePad_State * state);



#endif /* _GAMEPAD_H_ */

