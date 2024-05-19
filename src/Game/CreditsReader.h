#ifndef _CREDITSREADER_H_
#define _CREDITSREADER_H_

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct Credit {
	float time = 0;
	std::string credit = "";
};

class CreditsReader
{
public:
	float timeSpace = 0.5f;

	std::vector<Credit> credits;

	void ReadCredits(std::string filename);
};

#endif