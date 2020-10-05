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
	bool _running;
public:
	Process(string command, string environment);
	~Process();
	bool start();
	void clearOutput();
	bool waitForOutput();
	bool waitForOutput(int seconds);
	bool waitForOutput(const string &data);
	bool waitForOutput(const string &data, int seconds);
	bool writeInput(const string &data);
	string output();
private:
	void readThread();
};

