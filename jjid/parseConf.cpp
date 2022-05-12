#include "parseConf.hpp"

std::string Parser::getConfig()
{
    return (config);
}

int Parser::getLocFlag()
{
    return (locFlag);
}

void Parser::setConfig(std::string config)
{
    this->config = config;
}           

void Parser::setLocFlag(int flag)
{
    this->locFlag = flag;
}

void Parser::insertExistKey(std::string key, std::vector<std::string> val)
{
    if (locFlag == 0)
        keyValueMap.find(key)->second.insert(keyValueMap.find(key)->second.end(), val.begin(), val.end());
    else if (locFlag == 1)
        locMap.find(key)->second.insert(locMap.find(key)->second.end(), val.begin(), val.end());
}

void Parser::insertNewKey(std::string key, std::vector<std::string> val)
{
    if (locFlag == 0)
        keyValueMap.insert(std::pair<std::string, std::vector<std::string> >(key, val));
    else if (locFlag == 1)
        locMap.insert(std::pair<std::string, std::vector<std::string> >(key, val));
}

void Parser::openConfigfile(std::string confPath)
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
        setConfig(contents);
    }
	else
    {
        contents.clear();
        setConfig(contents);
    }
	fin.close();
}

std::vector<std::string> Parser::parseServerBlock(std::string config)
{
    std::vector<std::string> blockVec;
    unsigned long idx = config.find("server {");
    unsigned long idxNext = 0;
    try
	{
        if (idx == std::string::npos)
        {
            blockVec.clear();
            // throw 서버 블록 존재하지 않음;
        }
        else
        {
            std::string temp;
            while (idx != std::string::npos)
            {   
                idxNext = config.find("server {", idx + 1);
                temp = config.substr(idx, idxNext - idx);
                idx = idxNext;
                blockVec.push_back(temp);
            }
        }
    }
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
    return (blockVec);
}

void    Parser::parseLocPath(std::string currLine)
{
    std::string temp = currLine;
    std::string key = "path";
    std::vector<std::string> path;
    int start = temp.find("/");
    int end = temp.find(" ", start);

    path.push_back(temp.substr(start, end - start));
    if (locMap.find(key) == locMap.end())
        locMap.insert(std::pair<std::string, std::vector<std::string> >(key, path));
    else
        locMap.find(key)->second.insert(locMap.find(key)->second.end(), path.begin(), path.end());   
}

void    Parser::parseCurrLine(std::string currLine)
{
    std::vector<std::string> lineSplit = splitCurrLine(currLine);
    std::string key = lineSplit[0];
    lineSplit.erase(lineSplit.begin());
    if (getLocFlag() == 0)
    {
        if (keyValueMap.find(key) != keyValueMap.end())
            insertExistKey(key, lineSplit);
        else
            insertNewKey(key, lineSplit);
    }
    else if (getLocFlag() == 1)
    {
        if (locMap.find(key) != locMap.end())
            insertExistKey(key, lineSplit);
        else
            insertNewKey(key, lineSplit);
    }
}

Location Parser::initLocation()
{
    Location loc;

    if (locMap.find("path") != locMap.end())
        loc.setPath(locMap.find("path")->second[0]);
    if (locMap.find("root") != locMap.end())
        loc.setRoot(locMap.find("root")->second[0]);
    if (locMap.find("index") != locMap.end())
        loc.setIndex(locMap.find("index")->second);
    if (locMap.find("error_page") != locMap.end())
        loc.setErrPage(initErrPage());
    locMap.clear();
    return (loc);
}

std::map<std::vector<int>, std::string> Parser::initErrPage()
{
    std::map<std::vector<int>, std::string> temp;
    std::vector<int> statusCodes;
    int size;

    if (locFlag == 0)
    {
        if (keyValueMap.find("error_page") != keyValueMap.end())
        {
            size = keyValueMap.find("error_page")->second.size();
            std::string path = keyValueMap.find("error_page")->second[size - 1];
            for (int i = 0; i < size - 1; i++)
                statusCodes.push_back(atoi((keyValueMap.find("error_page")->second[i].c_str())));
            temp.insert(std::pair<std::vector<int>, std::string>(statusCodes, path));
        }
    }
    else if (locFlag == 1)
    {
        if (locMap.find("error_page") != locMap.end())
            size = locMap.find("error_page")->second.size();
            std::string path = locMap.find("error_page")->second[size - 1];
            for (int i = 0; i < size - 1; i++)
                statusCodes.push_back(atoi((locMap.find("error_page")->second[i].c_str())));
            temp.insert(std::pair<std::vector<int>, std::string>(statusCodes, path));
    }
    return (temp);
}

void    Parser::parseKeyValue(std::string content)
{
    std::vector<std::string> contSplit = charSplit(content, '\n');
    setLocFlag(0);

    for (unsigned long i = 0; i < contSplit.size(); i++)
    {   
        try
        {
            if (contSplit[i].find("location") != std::string::npos)
            {
                parseLocPath(contSplit[i]);
                setLocFlag(1);
            }
            else if (locFlag == 1 && contSplit[i].find("}") != std::string::npos)
            {
                locations.push_back(initLocation());
                setLocFlag(0);
            }
            else if (contSplit[i].find(";") != std::string::npos)
                parseCurrLine(contSplit[i]);
            // else
                // throw 올바르지 않은 config line form
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}

void    Parser::initServer(std::vector<Server>& servers, std::string content)
{
    parseKeyValue(content);
    for (unsigned long i = 0; i < keyValueMap.find("listen")->second.size(); i++)
    {
        Server serv;
        
        if (keyValueMap.find("root") != keyValueMap.end())
            serv.setRoot(keyValueMap.find("root")->second[0]);
        if (keyValueMap.find("server_name") != keyValueMap.end())
            serv.setHosts(keyValueMap.find("server_name")->second);
        if (keyValueMap.find("listen") != keyValueMap.end())
            serv.setPort(atoi((keyValueMap.find("listen")->second[i]).c_str()));
        if (keyValueMap.find("client_max_body_size") != keyValueMap.end())
            serv.setClientBodySize(atoi((keyValueMap.find("client_max_body_size")->second[0].c_str())));        
        if (keyValueMap.find("index") != keyValueMap.end())
            serv.setIndex(keyValueMap.find("index")->second);
        if (keyValueMap.find("error_page") != keyValueMap.end())
            serv.setErrPage(initErrPage());
        if (locations.size() != 0)
            serv.setLocation(locations);
        // if (keyValueMap.find("limit_except") != keyValueMap.end())
        //     serv.setAllowMethod(keyValueMap.find("limit_except")->second);
        std::cout << serv.getRoot() << std::endl;
        servers.push_back(serv);
    }
    locations.clear();
    keyValueMap.clear();
}

std::vector<Server> Parser::makeServers()
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