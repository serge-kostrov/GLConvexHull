#include "IOUtilities.h"

namespace io_util
{;

bool readAllLines(std::string& content, const char* fileName)
{
	using namespace std;
	content.clear();
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;
	string line = "";
	while (getline(ifs, line))
		content += line;
	ifs.close();

	return true;
}

}