#include <utils/files.h>

#include <sstream>
#include <fstream>

namespace nr
{

namespace utils
{

string loadFile(const string& fileName)
{
    std::ifstream file(fileName);

    if (!file) 
    {
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

string loadResourceFile(const string& resourceName)
{
    return loadFile("resource/" + resourceName);
}

}

}
