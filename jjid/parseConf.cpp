#include "parseConf.hpp"

// trim
std::string& leftTrim(std::string& str, std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rightTrim(std::string& str, std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trimString(std::string& str, std::string& chars)
{
    
    return leftTrim(rightTrim(str, chars), chars);
}

std::string transferOneSpace(std::string str)
{
    std::string answer;
    int spaceFlag = 0;

    for (int idx = 0; idx < str.size(); idx++)
    {
        if (str[idx] == ' ')
        {
            if (!spaceFlag)
            {
                answer += str[idx];
                spaceFlag = 1;
            }
        }
        else
        {
            if (spaceFlag)
                spaceFlag = 0;
            answer += str[idx];
        }
    }
    return (answer);
}

// split
std::vector<std::string> charSplit(std::string input, char delimiter) {
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
    return answer;
}

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

void    parseOneline(std::string contSplit, std::map<std::string, std::vector<std::string> > *map)
{
    std::string charsToTrim = " ;";

    trimString(contSplit, charsToTrim);
    transferOneSpace(contSplit);
    contSplit = transferOneSpace(contSplit);
    std::vector<std::string> lineSplit = charSplit(contSplit, ' ');
    std::string Key = lineSplit[0];
    lineSplit.erase(lineSplit.begin());
    map->insert(std::pair<std::string, std::vector<std::string> >(Key, lineSplit));
}

void    initServer(std::vector<Server>& servers, std::string content)
{
    // parse server block line
    std::vector<std::string> contSplit = charSplit(content, '\n');
    std::map<std::string, std::vector<std::string> > keyValueMap;

    for (int i = 0; i < contSplit.size(); i++)
    {   

        // location block line
        if (contSplit[i].find("location") != std::string::npos)
        {   
            while (*(contSplit[++i].end() - 1) == ';')
            {
                // trimString(contSplit[i], charsToTrim);
                // std::cout << "location line: " << contSplit[i] << std::endl;
            }
        }
        // normal line
        else if (*(contSplit[i].end() - 1) == ';')
            parseOneline(contSplit[i], &keyValueMap);
    }
    
    // init server
    int portNum = keyValueMap.find("listen")->second.size();
    for (int i = 0; i < portNum; i++)
    {
        Server serv;

        serv.setHost(keyValueMap.find("server_name")->second);
        serv.setPort(stoi(keyValueMap.find("listen")->second[i]));
        serv.setClientBodySize(1000);
        servers.push_back(serv);
    }
}

std::vector<Server> makeServers(std::string config)
{
    std::vector<Server> servers;
    std::cout << "server size: " << servers.size() << std::endl;
    std::vector<std::string> serverBlocks = parseServerBlock(config);
    std::vector<std::string>::iterator serverBlock = serverBlocks.begin();
    while (serverBlock != serverBlocks.end())
    {
        initServer(servers, *serverBlock);
        serverBlock++;
    }
    std::cout << "after server size: " << servers.size() << std::endl;
    return (servers);
}