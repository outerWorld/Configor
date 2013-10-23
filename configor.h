//
// It will work in the multithread environment.
//

#ifndef __CONFIGOR_H
#define __CONFIGOR_H

#include <string>
#include <vector>

class ConfigRegInfo {
 public:
	ConfigRegInfo();
	~ConfigRegInfo();
	
	typedef bool (*ReloadFunc)(std::string& conf_file);

 private:
	std::string config_file_;
	ReloadFunc	reload_func_;
};

class Configor {
 public:
	static Configor* GetInstance(std::string& conf_file);

	bool Reg(ConfigRegInfo& reg_info);

 private:
	Configor(std::string& conf_file);
	~Configor();

 private:
	std::string			conf_file_;
	pthread_mutex_t		reg;			
	std::vector<ConfigRegInfo> regs_;
};

#endif // __CONFIGOR_H
