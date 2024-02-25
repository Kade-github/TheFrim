#ifndef _STRINGTOOLS_H
#define _STRINGTOOLS_H

#include <string>
#include <vector>

class StringTools
{
public:
    static std::vector<std::string> Split(std::string s, std::string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }


	static std::string ToTheDecimial(double value, int decimalPlaces)
	{
		std::string str = std::to_string(value);
		int decimalIndex = str.find('.');

		if (decimalIndex == std::string::npos)
			return str;

        if (decimalPlaces == 0)
            return str.substr(0, decimalIndex);

		return str.substr(0, decimalIndex + decimalPlaces + 1);
	}

    static std::string gen_random(const int len) {
        static const char alpha[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::string tmp_s;
        tmp_s.reserve(len);

        for (int i = 0; i < len; ++i) {
            tmp_s += alpha[rand() % (sizeof(alpha) - 1)];
        }

        return tmp_s;
    }
};

#endif