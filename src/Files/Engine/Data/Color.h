/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _COLOR_H_
#define _COLOR_H_

namespace Engine
{
	namespace Data
	{
		struct Color
		{
		public:
			Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
			Color() {}
			float r;
			float g;
			float b;
			float a;
		};
	}
}

#endif