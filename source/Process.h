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
	mutex _logmutex;
	bool _debug;
public:
	Process(string command, string environment, bool debug);
	~Process();
	bool start();
	void clearOutput();
	bool waitForOutput();
	bool waitForOutput(int seconds);
	bool waitForOutput(const string &data);
	bool waitForOutput(const string &data, int seconds);
	int waitForAnyOutput(const vector<string> &datas, int seconds);
	bool writeInput(const string &data);
	string output();

	template<class...Args> void debug(const string &format, Args...args)
	{
		if(!_debug)
			return;

		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, sizeof(path));
		for (size_t i = strlen(path) - 1; i > 0; i--) {
			if (path[i] == '\\')
				break;
			path[i] = 0;
		}
		strcat_s(path, sizeof(path)-1, "debug.log");

		time_t timer;
    	char datetime[26];
    	struct tm tm_info;

    	timer = time(NULL);
    	localtime_s(&tm_info, &timer);

    	strftime(datetime, 26, "%Y-%m-%d %H:%M:%S", &tm_info);

    	_logmutex.lock();
		FILE *file = NULL;
		fopen_s(&file, path, "a");
		fprintf(file, "%s: ", datetime);
		fprintf(file, format.c_str(), args...);
		fputs("\n", file);
		fclose(file);
		_logmutex.unlock();
	}
private:
	void readThread();
};

