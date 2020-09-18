#include "pch.h"
#include "Process.h"

Process::Process(string command, string environment) {
	_command = command;
	_environment = environment;
}

Process::~Process() {
	TerminateProcess(_hProcess, -1);
	try {
		if (_thread.joinable())
			_thread.join();
	}
	catch (...) {

	}
	CloseHandle(_hProcess);
	CloseHandle(_stdin);
	CloseHandle(_stdout);
}

bool Process::start() {
	HANDLE hChildStd_IN_Rd = NULL;
	HANDLE hChildStd_IN_Wr = NULL;
	HANDLE hChildStd_OUT_Rd = NULL;
	HANDLE hChildStd_OUT_Wr = NULL;
	
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0))
		return false;
	if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		return false;

	if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
		return false;
	if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		return false;

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOA siStartInfo;
	BOOL bSuccess = FALSE;

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = hChildStd_OUT_Wr;
	siStartInfo.hStdInput = hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	siStartInfo.wShowWindow = SW_HIDE;

	// environment needs two \0 at the end
	_environment.resize(_environment.size() + 2);

	bSuccess = CreateProcessA(NULL,
		(LPSTR)_command.c_str(),     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		true,          // handles are inherited 
		NULL,             // creation flags 
		(LPVOID)_environment.c_str(),          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	 // If an error occurs, exit the application. 
	if (!bSuccess) {
		printf("could not create process\n");
		return false;
	}

	_hProcess = piProcInfo.hProcess;
	CloseHandle(piProcInfo.hThread);

	CloseHandle(hChildStd_OUT_Wr);
	CloseHandle(hChildStd_IN_Rd);

	_stdout = hChildStd_OUT_Rd;
	_stdin = hChildStd_IN_Wr;
	_thread = thread([this]() {readThread(); });

	return true;
}

void Process::clearOutput() {
	unique_lock<mutex> lock(_mutex);
	_stdoutbuffer.clear();
}

void Process::waitForOutput() {
	unique_lock<mutex> lock(_mutex);
	_cv.wait(lock, [this]() {
		return !_stdoutbuffer.empty();
	});
}

void Process::waitForOutput(const string& data)
{
	unique_lock<mutex> lock(_mutex);
	_cv.wait(lock, [this, data]() {
		return _stdoutbuffer.find(data) != _stdoutbuffer.npos;
	});
}

bool Process::writeInput(const string &data)
{
	if (!WriteFile(_stdin, data.c_str(), static_cast<DWORD>(data.size()), NULL, NULL)) {
		return false;
	}
	return true;
}

string Process::output() {
	return _stdoutbuffer;
}

void Process::readThread() {
	char buffer[256];
	DWORD read = 0;
	unique_lock<mutex> lock(_mutex);
	_stdoutbuffer = string();
	while (true) {
		lock.unlock();

		if (!ReadFile(_stdout, buffer, sizeof(buffer), &read, NULL))
			break;
		if (read == 0) break;

		lock.lock();
		//printf("read %d bytes: %.*s\n", read, read, buffer);
		_stdoutbuffer += string(buffer, read);
		_cv.notify_one();
	}
}
