#ifndef PARSECONF_HPP
# define PARSECONF_HPP

# include "parseUtil.hpp"
# include "Location.hpp"
# include "Server.hpp"
// # include "WebServer.hpp"

class Parser
{
		private :
            std::string config;
			std::map<std::string, std::vector<std::string> > keyValueMap;
            std::map<std::string, std::vector<std::string> > locMap;
            std::vector<Location> locations;
            int locFlag;

		public : 
			// Parser();
            void openConfigfile(std::string confPath);
            std::vector<std::string> parseServerBlock(std::string config);
            void parseCurrLine(std::string currLine);
            void parseLocPath(std::string currLine);
            void parseAllowMethod(std::string currLine);
            void parseKeyValue(std::string content);
            std::map<std::vector<int>, std::string> initErrPage();
            Location initLocation();
            void initServer(std::vector<Server>& servers, std::string content);
            std::vector<Server> makeServers();

            std::string getConfig();
            int getLocFlag();
            std::map<std::string, std::vector<std::string> > getKeyValueMap();

            void setConfig(std::string config);
            void setLocFlag(int flag);
            void insertExistKey(std::string key, std::vector<std::string> val);
            void insertNewKey(std::string key, std::vector<std::string> val);
            void insertKeyValue(std::string key, std::vector<std::string> val);
};

#endif