#ifndef CONFIGFILE_HEADER_GUARD
#define CONFIGFILE_HEADER_GUARD

#include <string>
#include <fstream>
#include <cstdio>

namespace config
{
	class ConfigFile
	{
	public:
		///Constructor
		ConfigFile(const char *fpath)
		{
			std::string line;
			std::ifstream File( fpath );

			while( std::getline(File, line) ){
				_filedata += line + '\n';//.substr(0,line.length()-1);
			}
			File.close();

			printf("%s",_filedata.c_str());
		}
		///Destructor
		~ConfigFile(void){}

	public:
		///Acess
		std::string getVariablestr(std::string pvar)
		{
			int loc = _filedata.find(pvar); //find variable
			int end = _filedata.find('\n', loc);
			int len = end - loc - 4;

			printf("%i %i %i\n",loc, end, len);
			return _filedata.substr(loc+pvar.length()+1, len);


		}
		int		getVariablei(std::string pvar){}
		float 	getVariablef(std::string pvar){}
		bool	getVariableb(std::string pvar){}

	private:
		std::string _filedata;
	};
}



#endif //CONFIGFILE_HEADER_GUARD
