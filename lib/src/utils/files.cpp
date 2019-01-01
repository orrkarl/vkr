#include <utils/files.h>

#include <sstream>
#include <fstream>

namespace nr
{

namespace utils
{

string loadFile(const string& fileName, Error& err)
{
    std::ifstream file(fileName);

    if (!file) 
    {
        err = Error::INVALID_OPERATION;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

string loadResourceFile(const string& resourceName, Error& err)
{
    return loadFile("resource/" + resourceName, err);
}

}

}
