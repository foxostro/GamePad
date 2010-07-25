/* vim:ts=4:sw=4
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>

#include <stdlib.h>
#include <stdio.h>

#include "GamePad.h"



/* state of the overall gamepad subsystem */
struct _GamePad_State
{
	IOHIDManagerRef hidManagerRef;
	CFMutableArrayRef devArrayRef;
};



/* get a long property from a HID device */
static Boolean
IOHIDDevice_GetLongProperty(IOHIDDeviceRef inIOHIDDeviceRef,
                            CFStringRef inKey,
							long * outValue )
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



static long
IOHIDDevice_GetLocationID(IOHIDDeviceRef inIOHIDDeviceRef)
{
	long result = 0;
	IOHIDDevice_GetLongProperty(inIOHIDDeviceRef,
	                            CFSTR(kIOHIDLocationIDKey),
								&result);
	return result;
}




/* CFSetApplierFunction to copy the CFSet to a CFArray */
static void
CFSetApplierFunctionCopyToCFArray(const void * v, void * ctx)
{
	CFArrayAppendValue((CFMutableArrayRef)ctx, v);
}



/* CFSetApplierFunction to copy the CFSet to a CFArray */
static CFComparisonResult
CFDeviceArrayComparatorFunction(const void *val1,
                                const void *val2,
								void *context)
{
	CFComparisonResult result = kCFCompareEqualTo;
	
	long loc1 = IOHIDDevice_GetLocationID((IOHIDDeviceRef)val1);
	long loc2 = IOHIDDevice_GetLocationID((IOHIDDeviceRef)val2);
	
	if(loc1 < loc2) {
		result = kCFCompareLessThan;
	} else if (loc1 > loc2) {
		result = kCFCompareGreaterThan;
	}
	return result;
}



/* Creates a HID matching dictionary for GamePad devices. */
static CFDictionaryRef
createGamePadMatchingDictionary(void)
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



/* Clears all fields in GamePad_State to their default values.
 * Performs no clean up for existing resources.
 */
static void
GamePad_State_Init(GamePad_State * state)
{
	assert(state);
	state->hidManagerRef = NULL;
	state->devArrayRef = NULL;
}



void
GamePad_ScanForDevices(GamePad_State * state)
{
	CFDictionaryRef matchingDict = NULL;
	CFSetRef devCFSetRef = NULL;
	CFIndex cnt = 0;

	assert(state);
	
	/* Enumerate HID devices that are gamepads */
	matchingDict = createGamePadMatchingDictionary();
	IOHIDManagerSetDeviceMatching(state->hidManagerRef, matchingDict);
	CFRelease(matchingDict);

	/* Copy all game pad device references into an array */
	devCFSetRef = IOHIDManagerCopyDevices(state->hidManagerRef);
	if(!devCFSetRef) {
		fprintf(stderr, "%s: Couldn't copy devices.\n", __PRETTY_FUNCTION__);
		exit(EXIT_FAILURE);
	}

	state->devArrayRef = CFArrayCreateMutable(kCFAllocatorDefault, 0,
									          &kCFTypeArrayCallBacks);
	CFSetApplyFunction(devCFSetRef,
	                   CFSetApplierFunctionCopyToCFArray,
					   state->devArrayRef);
	cnt = CFArrayGetCount(state->devArrayRef);
	CFArraySortValues(state->devArrayRef,
	                  CFRangeMake(0, cnt),
					  CFDeviceArrayComparatorFunction,
					  NULL);
	CFRelease(devCFSetRef);
}



GamePad_State *
GamePad_Init(void)
{
	GamePad_State * state = NULL;
	IOReturn r = 0;
	
	state = (GamePad_State *)malloc(sizeof(GamePad_State));
	GamePad_State_Init(state);

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



void
GamePad_Destroy(GamePad_State * state)
{
	IOReturn r = 0;

	assert(state);

	CFRelease(state->devArrayRef);

	r = IOHIDManagerClose(state->hidManagerRef, 0);
	if(kIOReturnSuccess != r) {
		fprintf(stderr, "%s: Couldn’t close IOHIDManager.\n",
		        __PRETTY_FUNCTION__);
		exit(EXIT_FAILURE);
	}

	free(state);
}



void
GamePad_DEBUG_DumpDevices(GamePad_State * state)
{
	CFIndex idx = 0, cnt = 0;

	assert(state);
	assert(state->devArrayRef);

	/* iterate across devices and print some information on each one */
	cnt = CFArrayGetCount(state->devArrayRef);
	for(idx = 0; idx < cnt; idx++)
	{
		IOHIDDeviceRef hidDeviceRef;

		hidDeviceRef = (IOHIDDeviceRef)CFArrayGetValueAtIndex(state->devArrayRef, idx);

		if(!hidDeviceRef) {
			continue;
		}

		printf("%s: dev[%ld]: %p\n",
				__PRETTY_FUNCTION__,
				idx,
				hidDeviceRef);
		fflush(stdout);
	}
}

