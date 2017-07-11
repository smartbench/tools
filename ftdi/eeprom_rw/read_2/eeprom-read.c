/*
 * FT_EEPROM_Read demonstration.
 * Dumps the EEPROM fields of any connected FTDI device.
 *
 * Copy libftd2xx.a, ftd2xx.h, WinTypes.h into current directory.
 *
 * Compile:
 *     cc eeprom-read.c -L. -lftd2xx -lpthread
 *
 * On Mac, the above line needs these extra dependencies:
 *     -lobjc -framework IOKit -framework CoreFoundation
 *
 * Run:
 *     sudo ./a.out
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ftd2xx.h"
#include "eeprom-read.h"

// Lecutra de:
// Eeprom_Generic
//  '-- FT_EEPROM_HEADER
//  '-- FT_EEPROM_2232H

int main (int argc, char *argv[]){
    FT_STATUS                 ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo = NULL;
    DWORD                     numDevs = 0;
    int                       i;
    int                       retCode = 0;

    /* Unused parameters */
    (void)argc;
    (void)argv;

    /* Discover how many FTDI devices are connected */
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus != FT_OK)
    {
        printf("FT_CreateDeviceInfoList failed (error code %d)\n",
               (int)ftStatus);
        goto exit;
    }

    if (numDevs == 0)
    {
        printf("No devices connected.\n");
        goto exit;
    }

    /* Allocate storage */
    devInfo = calloc((size_t)numDevs,
                     sizeof(FT_DEVICE_LIST_INFO_NODE));
    if (devInfo == NULL)
    {
        printf("Allocation failure.\n");
        goto exit;
    }

    /* Populate the list of info nodes */
    ftStatus = FT_GetDeviceInfoList(devInfo, &numDevs);
    if (ftStatus != FT_OK)
    {
        printf("FT_GetDeviceInfoList failed (error code %d)\n",
               (int)ftStatus);
        goto exit;
    }

    /* Display info (including EEPROM fields) for each connected FTDI device */
    for (i = 0; i < (int)numDevs; i++)
    {
        printf("Device %d:\n",i);
        printf("  Flags = 0x%x\n",devInfo[i].Flags);
        printf("  Type = 0x%x\n",devInfo[i].Type);
        printf("  ID = 0x%04x\n",devInfo[i].ID);
        printf("  LocId = 0x%x\n",devInfo[i].LocId);
        printf("  SerialNumber = %s\n",devInfo[i].SerialNumber);
        printf("  Description = %s\n",devInfo[i].Description);
        printf("  ftHandle = %p\n",devInfo[i].ftHandle);
        
        if (!readEeprom(devInfo[i].LocId, devInfo[i].Type))
        {
            goto exit;
        }
    }

exit:
    free(devInfo);
    return retCode;
}

// ############################################## //
// ############################################## //

static int readEeprom (DWORD locationId, FT_DEVICE deviceType){
    int                  success = 0;
    FT_STATUS            ftStatus;
    FT_HANDLE            ftHandle = (FT_HANDLE)NULL;
    char                 manufacturer[64];
    char                 manufacturerId[64];
    char                 description[64];
    char                 serialNumber[64];
    Eeprom_Generic      *eeprom = NULL;
    FT_EEPROM_HEADER    *header;

    ftStatus = FT_OpenEx((PVOID)(uintptr_t)locationId,
                         FT_OPEN_BY_LOCATION,
                         &ftHandle);
    if (ftStatus != FT_OK)
    {
        printf("FT_OpenEx failed (error code %d)\n", (int)ftStatus);
        goto exit;
    }

    /* Allocate enough to hold biggest EEPROM structure */
    eeprom = calloc(1, sizeof(*eeprom));
    if (eeprom == NULL)
    {
        printf("Allocation failure.\n");
        goto exit;
    }

    /* EEPROM_HEADER is first member of every type of eeprom */
    header = (FT_EEPROM_HEADER *)eeprom;
    header->deviceType = deviceType;

    manufacturer[0] = '\0';
    manufacturerId[0] = '\0';
    description[0] = '\0';
    serialNumber[0] = '\0';

    ftStatus = FT_EEPROM_Read(ftHandle,
                              eeprom,
                              sizeof(*eeprom),
                              manufacturer,
                              manufacturerId,
                              description,
                              serialNumber);
    if (ftStatus != FT_OK)
    {
        printf("FT_EEPROM_Read failed (error code %d)\n", (int)ftStatus);
        goto exit;
    }

    printf("Manufacturer = %s\n", manufacturer);
    printf("ManufacturerId = %s\n", manufacturerId);
    printf("Description = %s\n", description);
    printf("Serial number = %s\n", serialNumber);

    printf("deviceType: %s\n", deviceName(header->deviceType));

    printf("VendorId: %04x\n", header->VendorId);
    printf("ProductId: %04x\n", header->ProductId);
    printf("SerNumEnable: %02x\n", header->SerNumEnable);

    printf("MaxPower: %04x\n", header->MaxPower);
    printf("SelfPowered: %02x\n", header->SelfPowered);
    printf("RemoteWakeup: %02x\n", header->RemoteWakeup);

    printf("PullDownEnable: %02x\n", header->PullDownEnable);
    
    FT_EEPROM_2232H *dualH;
    if(deviceType == FT_DEVICE_2232H){
        dualH = &eeprom->dualH;
        printf("ALSlowSlew: %02x\n", dualH->ALSlowSlew);
        printf("ALSchmittInput: %02x\n", dualH->ALSchmittInput);
        printf("ALDriveCurrent: %02x\n", dualH->ALDriveCurrent);
        printf("AHSlowSlew: %02x\n", dualH->AHSlowSlew);
        printf("AHSchmittInput: %02x\n", dualH->AHSchmittInput);
        printf("AHDriveCurrent: %02x\n", dualH->AHDriveCurrent);
        printf("BLSlowSlew: %02x\n", dualH->BLSlowSlew);
        printf("BLSchmittInput: %02x\n", dualH->BLSchmittInput);
        printf("BLDriveCurrent: %02x\n", dualH->BLDriveCurrent);
        printf("BHSlowSlew: %02x\n", dualH->BHSlowSlew);
        printf("BHSchmittInput: %02x\n", dualH->BHSchmittInput);
        printf("BHDriveCurrent: %02x\n", dualH->BHDriveCurrent);

        printf("AIsFifo: %02x\n", dualH->AIsFifo);
        printf("AIsFifoTar: %02x\n", dualH->AIsFifoTar);
        printf("AIsFastSer: %02x\n", dualH->AIsFastSer);
        printf("BIsFifo: %02x\n", dualH->BIsFifo);
        printf("BIsFifoTar: %02x\n", dualH->BIsFifoTar);
        printf("BIsFastSer: %02x\n", dualH->BIsFastSer);
        printf("PowerSaveEnable: %02x\n", dualH->PowerSaveEnable);

        printf("ADriverType: %02x\n", dualH->ADriverType);
        printf("BDriverType: %02x\n", dualH->BDriverType);
    }

    printf("\n");

    // Success
    success = 1;

exit:
    (void)FT_Close(ftHandle);
    free(eeprom);
    return success;
}

// ############################################## //
// ############################################## //

static const char *deviceName(FT_DEVICE deviceType)
{
    switch(deviceType)
    {
        default:
            return "Unknown";

        case FT_DEVICE_BM:
            return "232 B";

        case FT_DEVICE_2232C:
            return "Dual 232";

        case FT_DEVICE_232R:
            return "232 R";

        case FT_DEVICE_2232H:
            return "Dual Hi-Speed 232";

        case FT_DEVICE_4232H:
            return "Quad Hi-Speed 232";

        case FT_DEVICE_232H:
            return "Hi-Speed 232";

        case FT_DEVICE_X_SERIES:
            return "X Series";
    }
}

