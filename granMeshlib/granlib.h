// 김치연구소 kimchi Sensor Lib for ESP32

#ifndef granlib_h
#define granlib_h

#include <Arduino.h>
#include <String.h>
#include <painlessMesh.h>
#include <Arduino_JSON.h>
#include "granDB.h"
#include "granEEPROM_esp32.h"
#include "granConvert.h"

class granlib
{
public:
    granEEPROM_esp32 _EEPROM;
    granDB _DB;
    granConvert _CONVERT;

    granlib();

private:
};

#endif
