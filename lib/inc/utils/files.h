#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace utils
{

string loadFile(const string& fileName, Error& err);

string loadResourceFile(const string& resourceName, Error& err);

}

}
