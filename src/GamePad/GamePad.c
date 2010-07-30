/* vim:ts=4:sw=4
 *
 * Enumerates, and provides access to, game pads on a Mac OS X system.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>

#include <stdlib.h>
#include <stdio.h>

#include "GamePad_Private.h"




#if 0 /* Leaving this in because I'll want it soon.*/



/* get a long property from a HID device */
static Boolean
IOHIDDevice_GetLongProperty(IOHIDDeviceRef inIOHIDDeviceRef,
                            CFStringRef inKey,
							long * outValue)
{
	Boolean result = FALSE;

	if(inIOHIDDeviceRef) {
		CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty(inIOHIDDeviceRef, inKey);
		if(tCFTypeRef && CFNumberGetTypeID() == CFGetTypeID(tCFTypeRef)) {
			result = CFNumberGetValue((CFNumberRef)tCFTypeRef,
			                          kCFNumberSInt32Type,
									  outValue);
		}
	}

	return result;
}



/* get the LocationID from a device */
static long IOHIDDevice_GetLocationID(IOHIDDeviceRef inIOHIDDeviceRef)
{
	long result = 0;
	IOHIDDevice_GetLongProperty(inIOHIDDeviceRef,
	                            CFSTR(kIOHIDLocationIDKey),
								&result);
	return result;
}



#endif



/* get the device serial number */
static CFStringRef IOHIDDevice_GetSerialNumber(IOHIDDeviceRef inIOHIDDeviceRef)
{
	return IOHIDDeviceGetProperty(inIOHIDDeviceRef,
	                              CFSTR(kIOHIDSerialNumberKey));
}



/* Creates a HID matching dictionary for GamePad devices. */
static CFDictionaryRef CreateGamePadMatchingDictionary(void)
{
	CFMutableDictionaryRef theDict = NULL;
	int usage = kHIDUsage_GD_GamePad;
	int usagePage = kHIDPage_GenericDesktop;
	
	theDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
								        &kCFTypeDictionaryKeyCallBacks,
								        &kCFTypeDictionaryValueCallBacks);
	
	CFDictionaryAddValue(theDict,
						 CFSTR(kIOHIDPrimaryUsageKey),
						 CFNumberCreate(kCFAllocatorDefault,
						                kCFNumberIntType,
										&usage));
	
	CFDictionaryAddValue(theDict,
						 CFSTR(kIOHIDPrimaryUsagePageKey),
	                     CFNumberCreate(kCFAllocatorDefault,
						                kCFNumberIntType,
										&usagePage));
	
	return (CFDictionaryRef)theDict;
}



void GamePad_Device_Clear(GamePad_Device * gamepad)
{
	assert(gamepad);
	gamepad->dev = NULL;
	gamepad->uid = 0;
	bzero(gamepad->sn, sizeof(gamepad->sn));
}



GamePad_Device * GamePad_Device_Init(IOHIDDeviceRef dev)
{
	GamePad_Device * gamepad = NULL;
	CFStringRef snRef = NULL;
	
	assert(dev);
	
	gamepad = (GamePad_Device *)malloc(sizeof(GamePad_Device));
	GamePad_Device_Clear(gamepad);
	
	gamepad->dev = dev;

	/* Grab some information about the gamepad from IOKit */
	snRef = IOHIDDevice_GetSerialNumber(dev);
	gamepad->uid = (uint32_t)CFHash(snRef);
	CFStringGetCString(snRef, gamepad->sn, sizeof(gamepad->sn),
	                   kCFStringEncodingMacRoman);

	return gamepad;
}



void GamePad_Device_Destroy(GamePad_Device * gamepad)
{
	assert(gamepad);
	assert(gamepad->dev);
	CFRelease(gamepad->dev);
	free(gamepad);
}



/* Clears all fields in GamePad_State to their default values.
 * Performs no clean up for existing resources.
 */
void GamePad_State_Clear(GamePad_State * state)
{
	assert(state);
	state->hidManagerRef = NULL;
	state->gamepads = NULL;
}



/* CFSetApplierFunction to create gamepads and copy into a CFArray */
static void CreateGamePadDeviceCopyToCFArray(const void * v, void * ctx)
{
	GamePad_Device * gamepad = NULL;
	assert(v);
	assert(ctx);
	gamepad = GamePad_Device_Init((IOHIDDeviceRef)v);
	CFArrayAppendValue((CFMutableArrayRef)ctx, gamepad);
}



void GamePad_ScanForDevices(GamePad_State * state)
{
	CFDictionaryRef matchingDict = NULL;
	CFSetRef devCFSetRef = NULL;

	assert(state);
	
	/* Create an array of gamepad objects */
	state->gamepads = CFArrayCreateMutable(kCFAllocatorDefault, 0, NULL);
	
	/* Enumerate HID devices that are gamepads */
	matchingDict = CreateGamePadMatchingDictionary();
	IOHIDManagerSetDeviceMatching(state->hidManagerRef, matchingDict);
	CFRelease(matchingDict);

	/* Copy all game pad device references into an array */
	devCFSetRef = IOHIDManagerCopyDevices(state->hidManagerRef);
	if(!devCFSetRef) {
		return; /* There are no gamepads available. */
	}

	/* Populate that array with the gamepads on the system. */
	CFSetApplyFunction(devCFSetRef,
	                   CreateGamePadDeviceCopyToCFArray,
					   state->gamepads);
	
	CFRelease(devCFSetRef);
}



GamePad_State * GamePad_Init(void)
{
	GamePad_State * state = NULL;
	IOReturn r = 0;
	
	state = (GamePad_State *)malloc(sizeof(GamePad_State));
	GamePad_State_Clear(state);

	/* initialize the HID manager */
	state->hidManagerRef = IOHIDManagerCreate(kCFAllocatorDefault, 0);
	if(!state->hidManagerRef) {
		fprintf(stderr, "%s: Couldn’t create a IOHIDManager.\n",
		        __PRETTY_FUNCTION__ );
		exit(EXIT_FAILURE);
	}

	r = IOHIDManagerOpen(state->hidManagerRef, 0);
	if(kIOReturnSuccess != r) {
		fprintf(stderr, "%s: Couldn’t open IOHIDManager.\n",
		        __PRETTY_FUNCTION__);
		exit(EXIT_FAILURE);
	}

	/* Rebuild the device list */
	GamePad_ScanForDevices(state);

	return state;
}



void GamePad_Destroy(GamePad_State * state)
{
	CFIndex i = 0, cnt = 0;
	IOReturn r = 0;

	assert(state);
	assert(state->gamepads);

	/* free all gamepads */
	cnt = CFArrayGetCount(state->gamepads);
	for(i = 0; i < cnt; i++)
	{
		GamePad_Device * gamepad = NULL;
		gamepad = (GamePad_Device *)CFArrayGetValueAtIndex(state->gamepads,i);
		GamePad_Device_Destroy(gamepad);
	}
	CFRelease(state->gamepads);

	/* Close the HID manager */
	r = IOHIDManagerClose(state->hidManagerRef, 0);
	if(kIOReturnSuccess != r) {
		fprintf(stderr, "%s: Couldn’t close IOHIDManager.\n",
		        __PRETTY_FUNCTION__);
		exit(EXIT_FAILURE);
	}

	/* Release storage used for subsystem state */
	free(state);
}



void GamePad_DEBUG_DumpDevice(GamePad_Device * gamepad)
{
	assert(gamepad);
	
	printf("Device %p:\n{\n", gamepad->dev);
	printf("\tUID: 0x%X\n", gamepad->uid);
	printf("\tSerial Number: %s\n", gamepad->sn);
	printf("}\n\n");
	fflush(stdout);
}



void GamePad_DEBUG_DumpDevices(GamePad_State * state)
{
	CFIndex i = 0, cnt = 0;

	assert(state);
	assert(state->gamepads);

	/* iterate across devices and print some information on each one */
	cnt = CFArrayGetCount(state->gamepads);
	for(i = 0; i < cnt; i++)
	{
		GamePad_Device * gamepad = NULL;
		gamepad = (GamePad_Device *)CFArrayGetValueAtIndex(state->gamepads, i);
		GamePad_DEBUG_DumpDevice(gamepad);
	}
}

