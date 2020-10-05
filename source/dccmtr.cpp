#include "pch.h"
#include "dccmtr.h"
#include "Process.h"

Process *p = NULL;

bool DC_Startup(int* pVendor, unsigned short usb)
{

	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, sizeof(path));
	for (size_t i = strlen(path) - 1; i > 0; i--) {
		if (path[i] == '\\')
			break;
		path[i] = 0;
	}
	strcat_s(path, sizeof(path)-1, "autocal.ini");

	char command[MAX_PATH], environment[256];
	GetPrivateProfileStringA("autocal", "command", NULL, command, sizeof(command) - 1, path);
	GetPrivateProfileStringA("autocal", "environment", NULL, environment, sizeof(environment) - 2, path);

	p = new Process(command, environment);
	if (!p->start()) {
		delete p;
		p = NULL;
		return false;
	}
	p->waitForOutput();

	return true;
}

bool DC_Authorize(int* key, int includeDatacolor)
{
	return true;
}

bool DC_UseCalibration(unsigned long type)
{
	return true;
}

bool DC_GetXYZ(unsigned short frames, int* pX, int* pY, int* pZ)
{
	if (!p) {
		return false;
	}

	p->clearOutput();
	p->writeInput(" ");
	p->waitForOutput("Hit ESC or Q to exit");

	string processdata = p->output();
	size_t pos = processdata.find("Result is XYZ: ");
	if (pos == processdata.npos) {
		printf("could not find string\n");
		return false;
	}

	double x, y, z;
	sscanf_s(processdata.c_str() + pos + 15, "%lf %lf %lf", &x, &y, &z);
	*pX = static_cast<int>(x * 1000.0);
	*pY = static_cast<int>(y * 1000.0);
	*pZ = static_cast<int>(z * 1000.0);
	return true;
}

void DC_Shutdown()
{
	if (!p)
		return;

	for (int i = 0; i < 3; i++) {
		p->writeInput("q");
	}

	delete p;
	p = NULL;
}

unsigned int DC_GetDetailedError(char* error)
{
	return false;
}

bool DC_ReadAmbientLightLevel(unsigned short* pLux)
{
	int x, y, z;
	if (DC_GetXYZ(1, &x, &y, &z)) {
		*pLux = y;
		return true;
	}
	return false;
}

bool DC_WriteAmbientLightControlRegVal(char val)
{
	return true;
}

bool DC_SetLED(char mode, unsigned short rate)
{
	return true;
}
