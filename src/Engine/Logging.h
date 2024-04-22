#ifndef _LOGGING_H
#define _LOGGING_H

#include <iostream>
#include <string>

#include <fstream>
#include <chrono>

class Logging
{
	std::chrono::time_point<std::chrono::system_clock> _start;
	std::ofstream _file;
public:
	std::vector<std::string> logs;

	Logging(std::string path)
	{
		_file.open(path);
		if (!_file.is_open())
		{
			std::cout << "Failed to open log file" << std::endl;
			return;
		}

		_start = std::chrono::system_clock::now();

		Write("Log started...");
	}

	~Logging()
	{
		if (_file.is_open())
			_file.close();
	}

	void Write(std::string message)
	{
		if (!_file.is_open())
		{
			std::cout << "[Failed to write to log] " << message << std::endl;
			return;
		}

		std::string time = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _start).count());

		std::string log = "[" + time + "s] " + message;

		std::cout << log << std::endl;
		_file << log << std::endl;

		_file.flush();

		logs.push_back(log);

		if (logs.size() > 30)
			logs.erase(logs.begin());
	}

};

#endif