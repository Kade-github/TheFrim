/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _VERSION_H_
#define _VERSION_H_

namespace Engine
{
    namespace Data
    {
        struct Version
        {
        public:
            Version(int minor, int major, int patch) : minor(minor), major(major), patch(patch) { }
            Version() {}
			int major;
			int minor;
			int patch;

            std::string ToString()
            {
                return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
            }
		};
    }
}

#endif