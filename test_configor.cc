
#include <time.h>

#include <string>
#include <iostream>

#include "configor.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		return 1;
	}

	std::string conf_file(argv[1]);

	Configor& config = Configor::GetInstance(conf_file.c_str());
	if (false == config.IsReady()) {
		return 1;
	}

	std::cout << "after GetInstance of Configor" << std::endl;
	while (1) {
		config.Status();
		sleep(5);
	}

	return 0;
}
