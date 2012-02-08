#include <string>

#include <gl3w/gl3w.h>
#include <time.h>

#include "Singleton.h"
#include "SFMLCore.h"
#include "GlobalValues.h"

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));
	std::string executablePath = std::string(argv[0]);
	std::string dataPath = executablePath + "/../../data/";
	Singleton<GlobalValues>::Instance()->setDataPath(dataPath);
	Singleton<SFMLCore>::Instance();
	return 0;
}

/* 
Things to fix:
-Combine mesh loader into one method
-Test removing event listeners
-Same mesh with different shaders
*/