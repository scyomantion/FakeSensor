#pragma once
class Process
{
private:
	string _command;
	string _environment;
	HANDLE _stdin;
	HANDLE _stdout;
	thread _thread;
	HANDLE _hProcess;
	mutex _mutex;
	condition_variable _cv;
	string _stdoutbuffer;
public:
	Process(string command, string environment);
	~Process();
	bool start();
	void clearOutput();
	void waitForOutput();
	void waitForOutput(const string &data);
	bool writeInput(const string &data);
	string output();
private:
	void readThread();
};

