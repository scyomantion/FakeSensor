#pragma once

extern "C" {
	__declspec(dllexport) bool DC_Startup(int* pVendor, unsigned short usb);
	__declspec(dllexport) bool DC_Authorize(int* key, int includeDatacolor);
	__declspec(dllexport) bool DC_UseCalibration(unsigned long type);
	__declspec(dllexport) bool DC_GetXYZ(unsigned short frames, int* pX, int* pY, int* pZ);
	__declspec(dllexport) void DC_Shutdown();
	__declspec(dllexport) unsigned int DC_GetDetailedError(char* error);
	__declspec(dllexport) bool DC_ReadAmbientLightLevel(unsigned short* pLux);
	__declspec(dllexport) bool DC_WriteAmbientLightControlRegVal(char val);
	__declspec(dllexport) bool DC_SetLED(char mode, unsigned short rate);
}