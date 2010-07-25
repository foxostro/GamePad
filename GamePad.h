/* vim:ts=4:sw=4
 *
 */

#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_



struct _GamePad_State;
typedef struct _GamePad_State GamePad_State;


/* Scans the system for gamepads and rebuild the list of devices. */
void GamePad_ScanForDevices(GamePad_State * state);



/* Initializes the GamePad subsytem and returns the subsystem state. */
GamePad_State * GamePad_Init(void);



/* Releases resources and destroys the state for the GamePad subsystem */
void GamePad_Destroy(GamePad_State * state);



/* Dumps debugging information about each GamePad that is connected. */
void GamePad_DEBUG_DumpDevices(GamePad_State * state);



#endif /* _GAMEPAD_H_ */

