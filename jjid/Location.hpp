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
			std::map<std::vector<int>, std::string> errPage;
			std::vector<std::string> AllowMethod;
			// bool autoIndex;
			int locationType;
			std::string extension;
			
		public:
			std::string getRoot();
			std::string getPath();
			std::vector<std::string> getIndex();
			std::map<std::vector<int>, std::string> getErrPage();
			std::vector<std::string> getAllowMethod();
			int getLocationType();
			std::string getExtension();
			// bool getAutoIndex();

			void setRoot(std::string root);
			void setPath(std::string path);
			void setIndex(std::vector<std::string> index);
			void setErrPage(std::map<std::vector<int>, std::string> errPage);
			void setAllowMethod(std::vector<std::string> AllowMethod);
			void setLocationType(int type);
			void setExtension(std::string extension);
			
	};

#endif