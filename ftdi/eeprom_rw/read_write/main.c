/*
	To build use the following gcc statement 
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o read main.c -L. -lftd2xx -Wl,-rpath,/usr/local/lib
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ftd2xx.h"

int printData(FT_DEVICE ftDevice, FT_PROGRAM_DATA * Data);

int main(int argc, char *argv[])
{
	FT_STATUS	ftStatus;
	FT_HANDLE	ftHandle0;
	int iport;
	static FT_PROGRAM_DATA Data;
	static FT_DEVICE ftDevice;
	DWORD libraryVersion = 0;
	int retCode = 0;

	ftStatus = FT_GetLibraryVersion(&libraryVersion);
	if (ftStatus == FT_OK)
		printf("Library version = 0x%x\n", (unsigned int)libraryVersion);
	else{
		printf("Error reading library version.\n"); return 1;
	}
	
	if(argc > 1) sscanf(argv[1], "%d", &iport);
	else iport = 0;
	printf("Opening port %d\n", iport);
	
	ftStatus = FT_Open(iport, &ftHandle0);
	if(ftStatus != FT_OK) {
		/* 
			This can fail if the ftdi_sio driver is loaded
		 	use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
		 */
		printf("FT_Open(%d) failed\n", iport);
		printf( "This can fail if the ftdi_sio driver is loaded "
		 	    "use lsmod to check this and rmmod ftdi_sio to remove "
		 	    "also rmmod usbserial");
		return 1;
	}
	
	printf("FT_Open succeeded.  Handle is %p\n", ftHandle0);

	ftStatus = FT_GetDeviceInfo(ftHandle0, &ftDevice, NULL, NULL, NULL, NULL);
	if (ftStatus != FT_OK){ 
		printf("FT_GetDeviceType FAILED!\n");
		retCode = 1;
		goto exit;
	}

	printf("FT_GetDeviceInfo succeeded.  Device is type %d.\n", 
	       (int)ftDevice);

	/* MUST set Signature1 and 2 before calling FT_EE_Read */
	Data.Signature1 = 0x00000000;
	Data.Signature2 = 0xffffffff;
	Data.Manufacturer = (char *)malloc(256); /* E.g "FTDI" */
	Data.ManufacturerId = (char *)malloc(256); /* E.g. "FT" */
	Data.Description = (char *)malloc(256); /* E.g. "USB HS Serial Converter" */
	Data.SerialNumber = (char *)malloc(256); /* E.g. "FT000001" if fixed, or NULL */
	if (Data.Manufacturer == NULL || Data.ManufacturerId == NULL || Data.Description == NULL || Data.SerialNumber == NULL){
		printf("Failed to allocate memory.\n");
		retCode = 1;
		goto exit;
	}

    // LECTURA DE LA EEPROM
	ftStatus = FT_EE_Read(ftHandle0, &Data);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Read failed\n");
		retCode = 1;
		goto exit;
	}
	printf("FT_EE_Read succeeded.\n\n");
	
	// IMPRIMO LO LEIDO EN PANTALLA
    printData(ftDevice, &Data);
    
    // CAMBIO LOS VALORES QUE ME PINTA
    Data.MaxPower = 100 + (Data.MaxPower - 50) % 400;//0xFA;
    Data.IFBIsFifo7 = 1;
    // Data.IFBIsFifoTar7 = 0;

    // ESCRIBO LA ESTRUCTURA MODIFICADA
	ftStatus = FT_EE_Program(ftHandle0, &Data);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Program failed (%d)\n", (int)ftStatus);
//		FT_Close(ftHandle0);
	}
	
	// VUELVO A LEER LA EEPROM PARA VERIFICAR QUE SE
	// ESCRIBIO CORRECTAMENTE.
	ftStatus = FT_EE_Read(ftHandle0, &Data);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Read failed\n");
		retCode = 1;
		goto exit;
	}
	printf("FT_EE_Read succeeded.\n\n");
    printData(ftDevice, &Data);
    
    
	
exit:
	free(Data.Manufacturer);
	free(Data.ManufacturerId);
	free(Data.Description);
	free(Data.SerialNumber);
	FT_Close(ftHandle0);
	printf("Returning %d\n", retCode);
	return retCode;
}

int printData(FT_DEVICE ftDevice, FT_PROGRAM_DATA * Data){

/*	printf("Signature1 = %d\n", (int)Data->Signature1);			
	printf("Signature2 = %d\n", (int)Data->Signature2);			
	printf("Version = %d\n", (int)Data->Version);				
								
	printf("VendorId = 0x%04X\n", Data->VendorId);				
	printf("ProductId = 0x%04X\n", Data->ProductId);
	printf("Manufacturer = %s\n", Data->Manufacturer);			
	printf("ManufacturerId = %s\n", Data->ManufacturerId);		
	printf("Description = %s\n", Data->Description);			
	printf("SerialNumber = %s\n", Data->SerialNumber);			*/
	printf("MaxPower = %d\n", Data->MaxPower);				/*
	printf("PnP = %d\n", Data->PnP) ;					
	printf("SelfPowered = %d\n", Data->SelfPowered);			
	printf("RemoteWakeup = %d\n", Data->RemoteWakeup);			*/

	if (ftDevice == FT_DEVICE_2232H){
/*	    printf("2232H:\n");
	    printf("-----\n");
	    
	    printf("\tPullDownEnable7 = 0x%X\n", Data->PullDownEnable7 );		// non-zero if pull down enabled
	    printf("\tSerNumEnable7 = 0x%X\n", Data->SerNumEnable7 );	// non-zero if serial number to be used
	    printf("\tALSlowSlew = 0x%X\n", Data->ALSlowSlew );		// non-zero if AL pins have slow slew
	    printf("\tALSchmittInput = 0x%X\n", Data->ALSchmittInput );	// non-zero if AL pins are Schmitt input
	    printf("\tALDriveCurrent = 0x%X\n", Data->ALDriveCurrent );	// valid values are 4mA, 8mA, 12mA, 16mA
	    printf("\tAHSlowSlew = 0x%X\n", Data->AHSlowSlew );		// non-zero if AH pins have slow slew
	    printf("\tAHSchmittInput = 0x%X\n", Data->AHSchmittInput );	// non-zero if AH pins are Schmitt input
	    printf("\tAHDriveCurrent = 0x%X\n", Data->AHDriveCurrent );	// valid values are 4mA, 8mA, 12mA, 16mA
	    printf("\tBLSlowSlew = 0x%X\n", Data->BLSlowSlew );		// non-zero if BL pins have slow slew
	    printf("\tBLSchmittInput = 0x%X\n", Data->BLSchmittInput );	// non-zero if BL pins are Schmitt input
	    printf("\tBLDriveCurrent = 0x%X\n", Data->BLDriveCurrent );	// valid values are 4mA, 8mA, 12mA, 16mA
	    printf("\tBHSlowSlew = 0x%X\n", Data->BHSlowSlew );		// non-zero if BH pins have slow slew
	    printf("\tBHSchmittInput = 0x%X\n", Data->BHSchmittInput );	// non-zero if BH pins are Schmitt input
	    printf("\tBHDriveCurrent = 0x%X\n", Data->BHDriveCurrent );	// valid values are 4mA, 8mA, 12mA, 16mA
	    */
	    printf("\tIFAIsFifo7 = 0x%X\n", Data->IFAIsFifo7 );		// non-zero if interface is 245 FIFO
	    printf("\tIFAIsFifoTar7 = 0x%X\n", Data->IFAIsFifoTar7 );	// non-zero if interface is 245 FIFO CPU target
	    /*printf("\tIFAIsFastSer7 = 0x%X\n", Data->IFAIsFastSer7 );	// non-zero if interface is Fast serial
	    printf("\tAIsVCP7 = 0x%X\n", Data->AIsVCP7 );			// non-zero if interface is to use VCP drivers
	    */
	    printf("\tIFBIsFifo7 = 0x%X\n", Data->IFBIsFifo7 );		// non-zero if interface is 245 FIFO
	    printf("\tIFBIsFifoTar7 = 0x%X\n", Data->IFBIsFifoTar7 );	// non-zero if interface is 245 FIFO CPU target
	    /*
	    printf("\tIFBIsFastSer7 = 0x%X\n", Data->IFBIsFastSer7 );	// non-zero if interface is Fast serial
	    printf("\tBIsVCP7 = 0x%X\n", Data->BIsVCP7 );			        // non-zero if interface is to use VCP drivers
        printf("\tPowerSaveEnable = 0x%X\n", Data->PowerSaveEnable );   // non-zero if using BCBUS7 to save power for self-powered designs
	    */
	}
	
	return 0;

}
