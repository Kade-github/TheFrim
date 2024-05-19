#include "CreditsReader.h"

void CreditsReader::ReadCredits(std::string filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cout << "Error: Could not open file " << filename << std::endl;
		return;
	}

	std::string line;

	credits.clear();

	float time = 0.0f;

	while (std::getline(file, line))
	{
		time += timeSpace;

		Credit c;
		if (line.empty())
			continue;

		c.credit = line;
		c.time = time;

		credits.push_back(c);
	}

	file.close();
}