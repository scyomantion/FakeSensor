#include "pch.h"
#include "dccmtr.h"
#include "Process.h"

Process *p = NULL;
int delay = 0;

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
	bool debug;
	GetPrivateProfileStringA("autocal", "command", NULL, command, sizeof(command) - 1, path);
	GetPrivateProfileStringA("autocal", "environment", NULL, environment, sizeof(environment) - 2, path);
	delay = GetPrivateProfileIntA("autocal", "delay", 0, path);
	debug = static_cast<bool>(GetPrivateProfileIntA("autocal", "debug", 0, path));

	p = new Process(command, environment, debug);
	if (!p->start()) {
		delete p;
		p = NULL;
		return false;
	}

	int res = p->waitForAnyOutput({"Hit ESC or Q to exit", "Spot read needs a calibration"}, 30);
	if(res == -1) {
		delete p;
		p = NULL;
		MessageBoxA(0, "Error 1", "Error", MB_OK);
		return false;
	}

	if(res == 1) {
		if(!p->waitForOutput("or hit Esc or Q to abort", 30)) {
			delete p;
			p = NULL;
			MessageBoxA(0, "Error 2", "Error", MB_OK);
			return false;
		}

		string message = p->output();
		size_t pos1 = message.find("before continuing");
		if(pos1 == message.npos) {
			delete p;
			p = NULL;
			MessageBoxA(0, "Error 3", "Error", MB_OK);
			MessageBoxA(0, message.c_str(), "Output was", MB_OK);
			return false;
		}
		pos1 += 18;
		while(message[pos1] == '\n')
			pos1++;
		size_t pos2 = message.find("and then hit any key");
		if(pos2 == message.npos) {
			delete p;
			p = NULL;
			return false;
		}
		while(message[pos2] != ',' && pos2 > pos1)
			pos2--;

		message = message.substr(pos1, pos2-pos1);
		MessageBoxA(0, message.c_str(), "Message from ArgyllCMS", MB_OK | MB_ICONWARNING);
		p->clearOutput();
		p->writeInput(" ");
		if(!p->waitForOutput("Hit ESC or Q to exit", 30)) {
			delete p;
			p = NULL;
			MessageBoxA(0, "Error 4", "Error", MB_OK);
			MessageBoxA(0, message.c_str(), "Output was", MB_OK);
			return false;
		}
	}

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

	if(delay) {
		this_thread::sleep_for(chrono::seconds(delay));
	}

	p->clearOutput();
	p->writeInput(" ");
	if(!p->waitForOutput("Hit ESC or Q to exit", 60)) {
		return false;
	}

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
