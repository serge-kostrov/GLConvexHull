#include "IOUtilities.h"

namespace io_util
{;

vector<string> split(const string& str)
{
    using namespace std;
    vector<string> tokens;
    istringstream iss(str);
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));

    return tokens;
}

bool readAllLines(string& content, const char* fileName)
{
	using namespace std;
	content.clear();
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;
	string line = "";
	while (getline(ifs, line))
		content += line + "\n";
	ifs.close();

	return true;
}

bool readXYZFile(vector<double3>& aPt, const char* fileName)
{
    using namespace std;
    string content = "";
    if(!readAllLines(content, fileName))
        return false;

    vector<string> aToken = split(content);
    const int nToken = aToken.size();
    if(nToken % double3::dim != 0)
        return false;

    for(int i = 0; i < nToken; i += 3)
        aPt.push_back(double3{stod(aToken[i]), stod(aToken[i + 1]), stod(aToken[i + 2])});

    return true;
}

}