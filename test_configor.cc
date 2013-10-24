
#include <time.h>

#include <string>
#include <iostream>

#include "configor.h"

int main(int argc, char *argv[]) {
	Configor* config = NULL;

	if (argc < 2) {
		return 1;
	}

	std::string conf_file(argv[1]);

	config = Configor::GetInstance(conf_file);

	while (1) {
		config->Status();
		sleep(5);
	}

	return 0;
}
