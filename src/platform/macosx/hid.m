/*
 Copyright 2011 Mattias Holm <mattias.holm(at)openorbit.org>

 This file is part of Open Orbit.

 Open Orbit is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Open Orbit is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with Open Orbit.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include "log.h"
#include "io-manager.h"

void hidValueChanged(void *ctxt, IOReturn result, void *sender,
                     IOHIDValueRef val);

static IOHIDManagerRef _hidManager;
static CFMutableDictionaryRef _deviceToIDDict;

void hidConnected(void *ctxt, IOReturn result, void *sender,
                  IOHIDDeviceRef dev);
void hidDisconnected(void *ctxt, IOReturn result, void *sender,
                     IOHIDDeviceRef dev);

void
hidValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val);
void
hidScValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val);
void
hidFsValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val);



CFMutableDictionaryRef
hidCreateMatchDict(int page, int key)
{
  CFMutableDictionaryRef query;
  query = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                    &kCFTypeDictionaryKeyCallBacks,
                                    &kCFTypeDictionaryValueCallBacks);

  if (!query) {
    ooLogError("could not create device matching dictionary");
  }
  CFNumberRef pageNum = CFNumberCreate(kCFAllocatorDefault,
                                       kCFNumberIntType,
                                       &page);

  CFNumberRef keyNum = CFNumberCreate(kCFAllocatorDefault,
                                      kCFNumberIntType,
                                      &key);

  CFDictionarySetValue(query, CFSTR( kIOHIDDeviceUsagePageKey ), pageNum);
  CFDictionarySetValue(query, CFSTR( kIOHIDDeviceUsageKey ), keyNum);
  CFRelease(pageNum);
  CFRelease(keyNum);
  return query;
}

bool
hidDevGetLongProp(IOHIDDeviceRef dev, CFStringRef key, long *res)
{
  bool stat = false;
  CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty(dev, key);
  if ( tCFTypeRef ) {
    // if this is a number
    if ( CFNumberGetTypeID( ) == CFGetTypeID( tCFTypeRef ) ) {
      // get its value
      stat = CFNumberGetValue((CFNumberRef)tCFTypeRef, kCFNumberLongType, res);
    }
  }
  return stat;
}

bool
hidDevGetStrProp(IOHIDDeviceRef dev, CFStringRef key, CFStringRef *res)
{
  bool stat = false;
  CFTypeRef tCFTypeRef = IOHIDDeviceGetProperty(dev, key);
  if ( tCFTypeRef ) {
    // if this is a number
    if ( CFStringGetTypeID( ) == CFGetTypeID( tCFTypeRef ) ) {
      // get its value
      *res = tCFTypeRef;
      stat = true;
    }
  }
  return stat;
}

void
hidElemSetDoubleProp(IOHIDElementRef elem, CFStringRef key, double val)
{
  CFNumberRef tCFNumberRef = CFNumberCreate( kCFAllocatorDefault, kCFNumberDoubleType, &val );
  if ( tCFNumberRef ) {
    IOHIDElementSetProperty( elem, key, tCFNumberRef );
    CFRelease( tCFNumberRef );
  }
}


void
hidElemCalbirateRange(IOHIDElementRef elem, double low, double high)
{
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationMinKey), low);
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationMaxKey), high);
}

void
hidElemCalibrateSat(IOHIDElementRef elem, double low, double high)
{
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationSaturationMinKey),
                       low);
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationSaturationMaxKey),
                       high);
}
void
hidElemCalibrateDeadzone(IOHIDElementRef elem, double low, double high)
{
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationDeadZoneMinKey), low);
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationDeadZoneMaxKey), high);
}

void
hidElemCalibrateGranularity(IOHIDElementRef elem, double val)
{
  hidElemSetDoubleProp(elem, CFSTR(kIOHIDElementCalibrationGranularityKey), val);
}


long
hidVendorId(IOHIDDeviceRef dev)
{
  long result = 0;
  if (hidDevGetLongProp(dev, CFSTR( kIOHIDVendorIDKey ), &result)) {
    return result;
  }

  ooLogError("vendor id is invalid");
  return -1;
}

long
hidProductId(IOHIDDeviceRef dev)
{
  long result = 0;
  if (hidDevGetLongProp(dev, CFSTR( kIOHIDProductIDKey ), &result)) {
    return result;
  }
  ooLogError("product id is invalid");
  return -1;
}

CFStringRef
hidProductName(IOHIDDeviceRef dev)
{
  CFStringRef ref = NULL;
  if (hidDevGetStrProp(dev, CFSTR( kIOHIDProductKey ), &ref)) {
    return ref;
  }

  ooLogError("product name is invalid");
  return NULL;
}

CFStringRef
hidVendorName(IOHIDDeviceRef dev)
{
  CFStringRef ref = NULL;
  if (hidDevGetStrProp(dev, CFSTR(kIOHIDManufacturerKey), &ref)) {
    return ref;
  }

  ooLogError("vendor name is invalid");
  return NULL;
}

CFStringRef
hidSerialNumber(IOHIDDeviceRef dev)
{
  CFStringRef ref = NULL;
  if (hidDevGetStrProp(dev, CFSTR(kIOHIDSerialNumberKey), &ref)) {
    return ref;
  }

  ooLogError("vendor name is invalid");
  return NULL;
}


void
hidInit(void)
{
//  hidInitSpaceshipSim();
//  hidInitFlightSim();
//  hidInitGenericDesktop();
  _hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

  _deviceToIDDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                              &kCFTypeDictionaryKeyCallBacks,
                                              NULL);

  CFMutableDictionaryRef scQuery;
  CFMutableDictionaryRef fsQuery;
  CFMutableDictionaryRef joyQuery;
  CFMutableDictionaryRef gpQuery;

  scQuery = hidCreateMatchDict(kHIDPage_Simulation,
                               kHIDUsage_Sim_SpaceshipSimulationDevice);
  fsQuery = hidCreateMatchDict(kHIDPage_Simulation,
                              kHIDUsage_Sim_FlightSimulationDevice);
  joyQuery = hidCreateMatchDict(kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
  gpQuery = hidCreateMatchDict(kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);
  const void *vals[4] = {scQuery, fsQuery, joyQuery, gpQuery};
  CFArrayRef query = CFArrayCreate(kCFAllocatorDefault, vals, 4,
                                   &kCFTypeArrayCallBacks);

  IOHIDManagerRegisterDeviceMatchingCallback(_hidManager,
                                             hidConnected,
                                             NULL);

  IOHIDManagerRegisterDeviceRemovalCallback(_hidManager,
                                            hidDisconnected,
                                            NULL);

  IOHIDManagerSetDeviceMatchingMultiple(_hidManager, query);

  IOReturn res = IOHIDManagerOpen(_hidManager, kIOHIDOptionsTypeNone);
  if (res != kIOReturnSuccess) {
    ooLogError("could not open hid-manager");
  }

  // Schedule hid manager to main run loop
  IOHIDManagerScheduleWithRunLoop(_hidManager, CFRunLoopGetCurrent(),
                                  kCFRunLoopDefaultMode);

  //IOHIDManagerRegisterInputValueCallback(_hidManager, hidValueChanged, NULL);

}

void
hidValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val);
void
hidScValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val)
{

}
void
hidFsValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val)
{
}

// Calibrates a device so that calibrated values will be in normalized ranges
// Essentially: sliders will be in the range 0.0 to 1.0
//              axises will be in the range -1.0 to 1.0
void
hidCalibrateDevice(IOHIDDeviceRef dev)
{
  CFArrayRef elements = IOHIDDeviceCopyMatchingElements(dev, NULL, 0);

  int elemCount = CFArrayGetCount(elements);

  for (int i = 0 ; i < elemCount ; i ++) {
    IOHIDElementRef elem = (IOHIDElementRef) CFArrayGetValueAtIndex(elements, i);
    IOHIDElementType etype = IOHIDElementGetType(elem);

    switch (etype) {
      case kIOHIDElementTypeInput_Misc: {
        uint32_t usage_page = IOHIDElementGetUsagePage(elem);
        uint32_t usage = IOHIDElementGetUsage(elem);
        if (usage_page == kHIDPage_GenericDesktop) {
          switch (usage) {
            case kHIDUsage_GD_X:
            case kHIDUsage_GD_Y:
            case kHIDUsage_GD_Z:
            case kHIDUsage_GD_Rx:
            case kHIDUsage_GD_Ry:
            case kHIDUsage_GD_Rz:
              hidElemCalbirateRange(elem, -1.0, 1.0);
              break;
            case kHIDUsage_GD_Slider:
              hidElemCalbirateRange(elem, 0.0, 1.0);
              break;
            case kHIDUsage_GD_Hatswitch: {
              int hat_min = IOHIDElementGetLogicalMin(elem);
              int hat_max = IOHIDElementGetLogicalMax(elem);
              ooLogInfo("hat switch min %d max %d", hat_min, hat_max);
              break;
            }
            default:
              break; // Ignore at the moment
          }
        }
        break;
      }
      default:
        break; // Ignore
    }
  }
}

void
hidConnected(void *ctxt, IOReturn result, void *sender, IOHIDDeviceRef dev)
{
  assert(sender == _hidManager);

  hidCalibrateDevice(dev);
  IOHIDDeviceRegisterInputValueCallback(dev, hidValueChanged, NULL);

  long vendorID = hidVendorId(dev);
  long productID = hidProductId(dev);
  CFStringRef vendorName = hidVendorName(dev);
  CFStringRef productName = hidProductName(dev);

  char vendorStr[CFStringGetLength(vendorName)*2+1];
  char productStr[CFStringGetLength(productName)*2+1];

  CFStringGetCString(vendorName, vendorStr, CFStringGetLength(vendorName)*2+1,
                     kCFStringEncodingUTF8);
  CFStringGetCString(productName, productStr, CFStringGetLength(productName)*2+1,
                     kCFStringEncodingUTF8);

  CFArrayRef arr = IOHIDDeviceCopyMatchingElements(dev, NULL, 0);
  int buttonCount = 0;
  int sliderCount = 0;
  int hatCount = 0;

  for (int i = 0 ; i < CFArrayGetCount(arr) ; i++) {
    IOHIDElementRef elem = (IOHIDElementRef)CFArrayGetValueAtIndex(arr, i);
    IOHIDElementType etype = IOHIDElementGetType(elem);
    if (etype == kIOHIDElementTypeInput_Button) {
      buttonCount ++;
    } else if (etype == kIOHIDElementTypeInput_Misc) {
      int page = IOHIDElementGetUsagePage(elem);
      int usage = IOHIDElementGetUsage(elem);
      if (page == kHIDPage_GenericDesktop) {
        if (usage == kHIDUsage_GD_Slider) {
          sliderCount ++;
        } else if (usage == kHIDUsage_GD_Hatswitch) {
          hatCount ++;
        }
      }
    }
  }
  CFRelease(arr);

  int deviceID = ioRegisterDevice(vendorID, vendorStr,
                                  productID, productStr,
                                  buttonCount, hatCount);
  CFDictionarySetValue(_deviceToIDDict, dev, (void*)deviceID);

  if (IOHIDDeviceConformsTo(dev, kHIDPage_Simulation,
                            kHIDUsage_Sim_SpaceshipSimulationDevice)) {
  } else if (IOHIDDeviceConformsTo(dev, kHIDPage_Simulation,
                                   kHIDUsage_Sim_FlightSimulationDevice)) {
  } else if (IOHIDDeviceConformsTo(dev, kHIDPage_GenericDesktop,
                                   kHIDUsage_GD_Joystick)) {
  } else if (IOHIDDeviceConformsTo(dev, kHIDPage_GenericDesktop,
                                   kHIDUsage_GD_GamePad)) {
  }

  IOHIDDeviceOpen(dev, 0);
}

void
hidDisconnected(void *ctxt, IOReturn result, void *sender, IOHIDDeviceRef dev)
{
  assert(sender == _hidManager);
  int devId = (int) CFDictionaryGetValue(_deviceToIDDict, dev);
  CFDictionaryRemoveValue(_deviceToIDDict, dev);
  ioRemoveDevice(devId);
}

void
hidValueChanged(void *ctxt, IOReturn result, void *sender, IOHIDValueRef val)
{
  IOHIDElementRef elem = IOHIDValueGetElement(val);
  IOHIDElementCookie cookie = IOHIDElementGetCookie(elem);
  IOHIDElementType etype = IOHIDElementGetType(elem);
  IOHIDElementCollectionType ctype = IOHIDElementGetCollectionType(elem);

  IOHIDDeviceRef dev = IOHIDElementGetDevice(elem);
  int dev_id = (int) CFDictionaryGetValue(_deviceToIDDict, dev);

  //CFStringRef name = hidProduct(dev);
  double_t dval = IOHIDValueGetScaledValue(val, kIOHIDValueScaleTypeCalibrated);

  //CFIndex maxVal = IOHIDElementGetPhysicalMax(elem);
  //CFIndex minVal = IOHIDElementGetPhysicalMin(elem);
  //NSLog(@"Name: %@", name);

//  ooLogInfo("%d: %d [%d, %d]", (int)ctype, (int)ival, (int)minVal, (int)maxVal);

  switch (etype){
    case kIOHIDElementTypeInput_Misc: {
      uint32_t usage = IOHIDElementGetUsage(elem);
      double_t dval = IOHIDValueGetScaledValue(val,
                                               kIOHIDValueScaleTypeCalibrated);

      switch (usage) {
        case kHIDUsage_GD_X: {
          ioPhysicalAxisChanged(dev_id, IO_AXIS_X, dval);
          break;
        }
        case kHIDUsage_GD_Y: {
          ioPhysicalAxisChanged(dev_id, IO_AXIS_Y, dval);
          break;
        }
        case kHIDUsage_GD_Z: {
         ioPhysicalAxisChanged(dev_id, IO_AXIS_Z, dval);
          break;
        }
        case kHIDUsage_GD_Rx: {
          ioPhysicalAxisChanged(dev_id, IO_AXIS_RX, dval);
          break;
        }
        case kHIDUsage_GD_Ry: {
          ioPhysicalAxisChanged(dev_id, IO_AXIS_RY, dval);
          break;
        }
        case kHIDUsage_GD_Rz: {
          ioPhysicalAxisChanged(dev_id, IO_AXIS_RZ, dval);
          break;
        }
        case kHIDUsage_GD_Slider: {
          ioPhysicalSliderChanged(dev_id, IO_SLIDER_THROT_0, dval);
          break;
        }
        case kHIDUsage_GD_Hatswitch: {
          int hat_min = IOHIDElementGetLogicalMin(elem);
          int hat_max = IOHIDElementGetLogicalMax(elem);
          int ival = IOHIDValueGetIntegerValue(val);

          float hat_pval
            = IOHIDValueGetScaledValue(val, kIOHIDValueScaleTypePhysical);


          // TODO: Think of this
          if (ival < hat_min || hat_max < ival) {
            ioDeviceHatSet(dev_id, 0, -1, -1);
          } else {
            ioDeviceHatSet(dev_id, 0, ival, hat_pval);
          }
          break;
        }
        default:
          break;// ignore
      }
    }
//      ooLogInfo("input misc");
      break;
    case kIOHIDElementTypeOutput:
//      ooLogInfo("output");
      break;
    case kIOHIDElementTypeFeature:
//      ooLogInfo("feature");

      break;
    case kIOHIDElementTypeCollection:
//      ooLogInfo("collection");
      break;
    case kIOHIDElementTypeInput_Axis:
//      ooLogInfo("axis value");
      break;
    case kIOHIDElementTypeInput_Button: {
      CFIndex state = IOHIDValueGetIntegerValue(val);
      uint32_t button_id = IOHIDElementGetUsage(elem);
      ooLogInfo("pushed button %u, state = %u", button_id, state);
      if (state) {
        ioDeviceButtonDown(dev_id, button_id);
      } else {
        ioDeviceButtonUp(dev_id, button_id);
      }
      //ioButtonStateChanged(dev, button_id, state);
      break;
    }
    case kIOHIDElementTypeInput_ScanCodes:
//      ooLogInfo("scan code");
      break;
    default:
      ooLogInfo("other value");
  }
}


#if 0
void
hidConnectedDesktop(void *ctxt, IOReturn result, void *sender, IOHIDDeviceRef dev)
{
  assert(sender == _hidManagerDesktop);

  long vendor = hidVendorId(dev);
  long product = hidProductId(dev);
  CFStringRef prodName = hidProduct(dev);

  ooLogInfo("usb device detected (v = %ld, p = %ld)", vendor, product);
  NSLog(@"%@", prodName);

  // Get all elements of the device
  CFArrayRef elements = IOHIDDeviceCopyMatchingElements(dev, NULL, 0);
  //kIOHIDElementNameKey
  int elemCount = CFArrayGetCount(elements);

  ooLogInfo("elements in device = %d", elemCount);
  for (int i = 0 ; i < elemCount ; i ++) {
    IOHIDElementRef elem = (IOHIDElementRef) CFArrayGetValueAtIndex(elements, i);
    IOHIDElementType etype = IOHIDElementGetType(elem);

    CFIndex maxPhysVal = IOHIDElementGetPhysicalMax(elem);
    CFIndex minPhysVal = IOHIDElementGetPhysicalMin(elem);

    CFIndex maxLogicalVal = IOHIDElementGetLogicalMax(elem);
    CFIndex minLogicalVal = IOHIDElementGetLogicalMin(elem);

    switch (etype){
      case kIOHIDElementTypeInput_Misc: {
        // In this case, usage defines the type of misc element
        uint32_t usage = IOHIDElementGetUsage(elem);
        switch (usage) {
          case kHIDUsage_GD_X:
            ooLogInfo("found x-axis");
            break;
          case kHIDUsage_GD_Y:
            ooLogInfo("found y-axis");
            break;
          case kHIDUsage_GD_Z:
            ooLogInfo("found z-axis");
            break;
          case kHIDUsage_GD_Rx:
            ooLogInfo("found rx-axis");
            break;
          case kHIDUsage_GD_Ry:
            ooLogInfo("found ry-axis");
            break;
          case kHIDUsage_GD_Rz:
            ooLogInfo("found rz-axis");
            break;

          // Vector stuff
          case kHIDUsage_GD_Vx: /* Dynamic Value */
            ooLogInfo("found vec x");
            break;
          case kHIDUsage_GD_Vy: /* Dynamic Value */
            ooLogInfo("found vec y");
            break;
          case kHIDUsage_GD_Vz:	/* Dynamic Value */
            ooLogInfo("found vec z");
            break;
          case kHIDUsage_GD_Vbrx:	/* Dynamic Value */
            ooLogInfo("found vec rx");
            break;
          case kHIDUsage_GD_Vbry:	/* Dynamic Value */
            ooLogInfo("found vec ry");
            break;
          case kHIDUsage_GD_Vbrz:	/* Dynamic Value */
            ooLogInfo("found vec rz");
            break;
          case kHIDUsage_GD_Vno:	/* Dynamic Value */
            ooLogInfo("found vec no");
            break;
          // Dpad
          case kHIDUsage_GD_DPadUp:	/* On/Off Control */
            ooLogInfo("found dpad up");
            break;
          case kHIDUsage_GD_DPadDown:	/* On/Off Control */
            ooLogInfo("found dpad down");
            break;
          case kHIDUsage_GD_DPadRight:	/* On/Off Control */
            ooLogInfo("found dpad right");
            break;
          case kHIDUsage_GD_DPadLeft:
            ooLogInfo("found dpad left");
            break;

          // Sliders and dials
          case kHIDUsage_GD_Slider:	/* Dynamic Value */
            ooLogInfo("found slider");
            break;
          case kHIDUsage_GD_Dial:	/* Dynamic Value */
            ooLogInfo("found dial");
            break;
          case kHIDUsage_GD_Wheel:	/* Dynamic Value */
            ooLogInfo("found wheel");
            break;
          case kHIDUsage_GD_Hatswitch:	/* Dynamic Value */
            ooLogInfo("found hat");
            break;
          case kHIDUsage_GD_CountedBuffer:	/* Logical Collection */
            ooLogInfo("found counted buffer");
            break;
          case kHIDUsage_GD_ByteCount:	/* Dynamic Value */
            ooLogInfo("found byte count");
            break;
          case kHIDUsage_GD_MotionWakeup:	/* One-Shot Control */
            ooLogInfo("found motion wakeup");
            break;
          case kHIDUsage_GD_Start:	/* On/Off Control */
            ooLogInfo("found start");

            break;
          case kHIDUsage_GD_Select:	/* On/Off Control */
            ooLogInfo("found select");
            break;
          default: {
            const char *strName = CFStringGetCStringPtr(prodName,
                                                        kCFStringEncodingUTF8);
            ooLogWarn("Unknown control for device '%s', usage = %u", strName,
                      usage);
            break;
          }
        }
        //ooLogInfo("input misc");

        break;
      }
      case kIOHIDElementTypeOutput:
        ooLogInfo("output");
        break;
      case kIOHIDElementTypeFeature:
        ooLogInfo("feature");
        break;
      case kIOHIDElementTypeCollection: {
        ooLogInfo("collection");
        IOHIDElementCollectionType ctyp = IOHIDElementGetCollectionType(elem);
        switch (ctyp) {
          case kIOHIDElementCollectionTypePhysical:
            ooLogInfo("physical");
            break;
          case kIOHIDElementCollectionTypeApplication:
            ooLogInfo("application");
            break;
          case kIOHIDElementCollectionTypeLogical:
            ooLogInfo("logical");
            break;
          case kIOHIDElementCollectionTypeReport:
            ooLogInfo("report");
            break;
          case kIOHIDElementCollectionTypeNamedArray:
            ooLogInfo("named array");
            break;
          case kIOHIDElementCollectionTypeUsageSwitch:
            ooLogInfo("usage switch");
            break;
          case kIOHIDElementCollectionTypeUsageModifier:
            ooLogInfo("usage modifier");
            break;
          default:
          break;
        }
        break;
      }
      case kIOHIDElementTypeInput_Axis:
        ooLogInfo("axis value");
        break;
      case kIOHIDElementTypeInput_Button: {
        // In this case, usage is button id
        uint32_t usage = IOHIDElementGetUsage(elem);
        ooLogInfo("found button %d", usage);
        break;
      }
      case kIOHIDElementTypeInput_ScanCodes:
        ooLogInfo("scan code");
        break;
      default:
        ooLogInfo("other value");
    }
  }
}
#endif


