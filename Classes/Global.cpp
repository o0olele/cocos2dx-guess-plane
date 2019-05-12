#include "Global.h"

std::string GLOBAL_TRIM(std::string s)
{
	std::string temp = s;
	int index = 0;
	if (!temp.empty())
	{
		while ((index = temp.find(char(0), index)) != std::string::npos)
		{
			temp.erase(index, 1);
		}
	}

	return temp;
}
