#include "dccmtrx.h"
#include "dccmtr.h"

bool DCX_Startup(int* pVendor, unsigned short usb)
{
    return DC_Startup(pVendor, usb);
}
bool DCX_Authorize(int* key, int includeDatacolor)
{
    return DC_Authorize(key, includeDatacolor);
}
bool DCX_UseCalibration(unsigned long type)
{
    return DC_UseCalibration(type);
}
bool DCX_GetXYZ(int* pX, int* pY, int* pZ)
{
    return DC_GetXYZ(1, pX, pY, pZ);
}
void DCX_Shutdown()
{
    DC_Shutdown();
}
unsigned int DCX_GetDetailedError(char* error)
{
    return DC_GetDetailedError(error);
}
bool DCX_ReadAmbientLightLevel(unsigned short* pLux)
{
    return DC_ReadAmbientLightLevel(pLux);
}
