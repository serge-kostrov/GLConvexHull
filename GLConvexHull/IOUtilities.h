#pragma once
#include "base.h"
#include "stdheaders.h"


namespace io_util
{;

vector<string>&& split(const string& str);
bool readAllLines(string& content, const char* fileName);
bool readXYZFile(vector<double3>& aPt, const char* fileName);

}