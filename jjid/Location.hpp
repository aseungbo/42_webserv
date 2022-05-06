#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "uniHeader.hpp"
// # include "Server.hpp"

class Location
	{
		private:
			std::string root;
			// std::string index;
			std::vector<std::string> index;
			// bool autoIndex;
			// std::map<std::vector<int>, std::string> errPage;
			std::vector<std::string> methodsAllowed;
			
			std::string path;
			
		public:
			std::string getRoot();
			std::vector<std::string> getIndex();
			// bool getAutoIndex();
			// std::map<std::vector<int>, std::string> getErrPage();
			std::vector<std::string> getMethodsAllowed();
			
			std::string getPath();
			
	};

#endif