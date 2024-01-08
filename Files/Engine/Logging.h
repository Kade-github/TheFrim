/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "../../Includes.h"

namespace Engine
{
    namespace Logging
    {
        class Logger
        {
        public:
            static std::ofstream file_handle;

            // queue of log messages, each log message is a map of type and message
            static std::vector<std::unordered_map<std::string, std::string>> log_queue;
        };


        void Init();

        void Log(std::string message, std::string type);
        void LogAsync(std::string message, std::string type);

        void LogUpdate();

        void Destroy();
    }
}

#endif