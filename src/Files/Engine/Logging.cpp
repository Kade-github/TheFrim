/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

#include "Logging.h"

namespace Engine
{
	namespace Logging
	{

		std::ofstream Logger::file_handle = std::ofstream();

		std::vector<std::unordered_map<std::string, std::string>> Logger::log_queue = std::vector<std::unordered_map<std::string, std::string>>();

		/// <summary>
		/// Initialize the logging system
		/// </summary>
		void Init()
		{
			Logger::file_handle.open("log.txt", std::ios::out | std::ios::app);
		}

		/// <summary>
		/// Log a message to the log file
		/// </summary>
		/// <param name="message">- The message to log</param>
		/// <param name="type">- The type of the log (info, warn, error, etc)</param>
		void Log(std::string message, std::string type)
		{
			Logger::file_handle << ("[" + type + "] " + message) << std::endl;
			std::cout << ("[" + type + "] " + message) << std::endl;
		}

		/// <summary>
		/// Writes the log to a queue to be written to the log file later (on the next frame)
		/// </summary>
		/// <param name="message">- The message to log</param>
		/// <param name="type">- The type of the log (info, warn, error, etc)</param>
		void LogAsync(std::string message, std::string type)
		{
			Logger::log_queue.push_back({ {message,type} });
		}

		/// <summary>
		/// Frame update for logging
		/// </summary>
		void LogUpdate()
		{
			for (auto& log : Logger::log_queue)
			{
				Log(log["message"], log["type"]);
			}
			Logger::log_queue.clear();
		}

		/// <summary>
		/// Saves the log file and closes the file handle
		/// </summary>
		void Destroy()
		{
			Logger::file_handle.close();
		}
	}
}