#include "parseConf.hpp"

int         checkArg(int ac, char **av, std::string& confPath)
{
    if (ac == 2)
		confPath = av[1];
	else if (ac == 1)
		confPath = DEFAULT_PATH;
	else
        return (-1);
    return (0);
}

std::string openConfigfile(std::string confPath)
{
    std::ifstream fin(confPath);
	std::string contents;
    int size; 

	if (fin.is_open())
    {
        fin.seekg(0, std::ios::end);
        size = fin.tellg();
        contents.resize(size);
        fin.seekg(0, std::ios::beg);
        fin.read(&contents[0], size);
    }
	else
    {
        contents.clear();
        return (contents);
    }
	fin.close();
    return (contents);
}

void makeServers(std::vector<Server>& servers, std::string config)
{
    ;
}