#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "uniHeader.hpp"
// # include "Server.hpp"

class Location
	{
		private:
			std::string root;
			std::string path;
			std::vector<std::string> index;
			// std::vector<std::string> methodsAllowed;
			// bool autoIndex;
			// std::map<std::vector<int>, std::string> errPage;
			
			
		public:
			std::string getRoot();
			std::vector<std::string> getIndex();
			// bool getAutoIndex();
			// std::map<std::vector<int>, std::string> getErrPage();
			std::vector<std::string> getMethodsAllowed();
			std::string getPath();

			void setRoot(std::string root);
			void setPath(std::string path);
			void setIndex(std::vector<std::string> index);
			// void setMethodsAllowed(std::vector<std::string> methodsAllowed);
			
			
	};

#endif