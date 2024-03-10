#ifndef _MATHTOOLS_H
#define _MATHTOOLS_H

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

class MathTools
{
public:
	static double fround(float val, int dp) {
        int charsNeeded = 1 + snprintf(NULL, 0, "%.*f", dp, val);
        char* buffer = (char*)malloc(charsNeeded);
        snprintf(buffer, charsNeeded, "%.*f", dp, val);
        float result = (float)atof(buffer);
        free(buffer);
        return result;
    }
};

#endif