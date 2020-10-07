#include "pch.h"
#include "Process.h"

Process::Process(string command, string environment, bool debug) {
	_command = command;
	_environment = environment;
	_running = false;
	_debug = debug;
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

	debug("create process: %s", _command.c_str());
	debug("environment: %s", _environment.c_str());
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
		debug("could not create process");
		return false;
	}
	debug("process created");

	_hProcess = piProcInfo.hProcess;
	CloseHandle(piProcInfo.hThread);

	CloseHandle(hChildStd_OUT_Wr);
	CloseHandle(hChildStd_IN_Rd);

	_stdout = hChildStd_OUT_Rd;
	_stdin = hChildStd_IN_Wr;
	_running = true;
	debug("start thread");
	_thread = thread([this]() {readThread(); });

	return true;
}

void Process::clearOutput() {
	unique_lock<mutex> lock(_mutex);
	debug("clear output");
	_stdoutbuffer.clear();
}

bool Process::waitForOutput() {
	unique_lock<mutex> lock(_mutex);
	debug("waitForOutput()");
	_cv.wait(lock, [this]() {
		return !_stdoutbuffer.empty() || !_running;
	});
	bool res = !_stdoutbuffer.empty();
	debug("waitForOutput(): %d", res);
	return res;
}

bool Process::waitForOutput(int seconds) {
	unique_lock<mutex> lock(_mutex);
	debug("waitForOutput(%d)", seconds);
	_cv.wait_for(lock, chrono::seconds(seconds), [this]() {
		return !_stdoutbuffer.empty() || !_running;
	});
	bool res = !_stdoutbuffer.empty();
	debug("waitForOutput(%d): %d", seconds, res);
	return res;
}

bool Process::waitForOutput(const string& data)
{
	unique_lock<mutex> lock(_mutex);
	debug("waitForOutput(\"%s\")", data.c_str());
	_cv.wait(lock, [this, data]() {
		return _stdoutbuffer.find(data) != _stdoutbuffer.npos || !_running;
	});
	bool res = _stdoutbuffer.find(data) != _stdoutbuffer.npos;
	debug("waitForOutput(\"%s\"): %d", data.c_str(), res);
	return res;
}

bool Process::waitForOutput(const string& data, int seconds)
{
	unique_lock<mutex> lock(_mutex);
	debug("waitForOutput(\"%s\", %d)", data.c_str(), seconds);
	_cv.wait_for(lock, chrono::seconds(seconds), [this, data]() {
		return _stdoutbuffer.find(data) != _stdoutbuffer.npos || !_running;
	});
	bool res = _stdoutbuffer.find(data) != _stdoutbuffer.npos;
	debug("waitForOutput(\"%s\", %d): %d", data.c_str(), seconds, res);
	return res;
}

int Process::waitForAnyOutput(const vector<string> &datas, int seconds)
{
	unique_lock<mutex> lock(_mutex);
	debug("waitForAnyOutput(%ld, %d)", datas.size(), seconds);
	for(size_t i = 0; i < datas.size(); i++) {
		debug("%ld: %s", i, datas[i].c_str());
	}

	_cv.wait_for(lock, chrono::seconds(seconds), [this, datas]() {
		for(auto &data: datas) {
			if(_stdoutbuffer.find(data) != _stdoutbuffer.npos)
				return true;
		}
		if(!_running)
			return true;

		return false;
	});
	for(size_t i = 0; i < datas.size(); i++) {
		if(_stdoutbuffer.find(datas[i]) != _stdoutbuffer.npos) {
			debug("waitForAnyOutput(%ld, %d): %ld", datas.size(), seconds, i);
			return static_cast<int>(i);
		}
	}
	debug("waitForAnyOutput(%ld, %d): %d", datas.size(), seconds, -1);
	return -1;
}

bool Process::writeInput(const string &data)
{
	debug("input: \"%s\"", data.c_str());
	if (!WriteFile(_stdin, data.c_str(), static_cast<DWORD>(data.size()), NULL, NULL)) {
		return false;
	}
	return true;
}

string Process::output() {
	debug("return output: \"%s\"", _stdoutbuffer.c_str());
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
		debug("add output: \"%s\"", string(buffer, read).c_str());
		_stdoutbuffer += string(buffer, read);
		_cv.notify_one();
	}
	lock.lock();
	_running = false;
	_cv.notify_one();
	debug("read thread ended");
}
