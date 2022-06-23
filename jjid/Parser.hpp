#ifndef PARSER_HPP
# define PARSER_HPP

# include "uniHeader.hpp"
# include "Location.hpp"
# include "Server.hpp"

class Parser
{
		private :
            std::string config;
			std::map<std::string, std::vector<std::string> > keyValueMap;
            std::map<std::string, std::vector<std::string> > locMap;
            std::vector<Location> locations;
            int locFlag;

		public : 
			Parser(void);
            int checkCommentAndBlank(std::string currLine);
            void openConfigfile(std::string confPath);
            std::vector<std::string> parseServerBlock(std::string config);
            void parseCurrLine(std::string currLine);
            void parseLocPath(std::string currLine);
            void parseAllowMethod(std::string currLine);
            void parseKeyValue(std::string content);
            std::map<int, std::string> initErrPage();
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

            std::string& leftTrim(std::string& str, std::string& chars);
            std::string& rightTrim(std::string& str, std::string& chars);
            std::string& trimString(std::string& str, std::string& chars);
            std::string transferOneSpace(std::string str);
            std::vector<std::string> charSplit(std::string input, char delimiter);
            std::vector<std::string> splitCurrLine(std::string currLine);
};

#endif