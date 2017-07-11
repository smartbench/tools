#ifndef EEPROM_READ_H_
#define EEPROM_READ_H_

#include "ftd2xx.h"

static const char *deviceName(FT_DEVICE deviceType);
static int readEeprom (DWORD locationId, FT_DEVICE deviceType);

typedef union Eeprom_Generic {
    FT_EEPROM_232B     b;
    FT_EEPROM_232R     r;
    FT_EEPROM_232H     singleH;
    FT_EEPROM_2232     dual;
    FT_EEPROM_2232H    dualH;
    FT_EEPROM_4232H    quadH;
    FT_EEPROM_X_SERIES x;
} Eeprom_Generic;

#endif
