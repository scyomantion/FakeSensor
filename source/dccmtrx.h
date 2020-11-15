#pragma once

extern "C" {
    __declspec(dllexport) bool DCX_Startup(int* pVendor, unsigned short usb);
    __declspec(dllexport) bool DCX_Authorize(int* key, int includeDatacolor);
    __declspec(dllexport) bool DCX_UseCalibration(unsigned long type);
    __declspec(dllexport) bool DCX_GetXYZ(int* pX, int* pY, int* pZ);
    __declspec(dllexport) void DCX_Shutdown();
    __declspec(dllexport) unsigned int DCX_GetDetailedError(char* error);
    __declspec(dllexport) bool DCX_ReadAmbientLightLevel(unsigned short* pLux);
}