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

std::vector<std::string> parseServerBlock(std::string config)
{
    std::vector<std::string> blockVec;
    std::string temp;
    int idx = config.find("server {");
    int idxNext;

    if (idx == std::string::npos)
    {
        blockVec.clear();
        printErr("Not exist server block.");
        return (blockVec);
    }
    else
    {
        while (idx != std::string::npos)
        {   
            idxNext = config.find("server {", idx + 1);
            temp = config.substr(idx, idxNext - idx);
            idx = idxNext;
            blockVec.push_back(temp);
            temp.clear();
        }
    }
    return (blockVec);
}

std::vector<Server> makeServers(std::string config)
{
    std::vector<Server> servers;
    std::vector<std::string> serverBlocks = parseServerBlock(config);
    std::vector<std::string>::iterator serverBlock = serverBlocks.begin();
    while (serverBlock != serverBlocks.end())
    {
        std::cout << *serverBlock << std::endl;
        serverBlock++;
    }
    return (servers);
}