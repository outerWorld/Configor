//
// It will work in the multithread environment.
//

#ifndef __CONFIGOR_H
#define __CONFIGOR_H

#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pthread.h>

#include <iostream>
#include <string>
#include <vector>

class ConfigRegInfo {
 public:	
	// it can solve the problem that a class use static function
	// as the callback function, so it works with single model.
	// for double buffer, the caller must change index of running configuration itself.
	typedef bool (*ReloadFunc)(void *param, const std::string& conf_file);

 public:
	ConfigRegInfo(void *param, std::string& config_file, ReloadFunc func) : param_(param), config_file_(config_file), reload_func_(func) {
	}

	~ConfigRegInfo() { }

	bool IsValid() {
		struct stat st;
		
		if (0 != stat(config_file_.c_str(), &st)) {
			return false;
		}

		if (!S_ISREG(st.st_mode)) {
			return false;
		}
		last_time_ = st.st_mtime;

		return true;
	}

	void *		param_;
	std::string config_file_;
	ReloadFunc	reload_func_;
	uint32_t	last_time_;
};

class Configor {
 private:
	class Config {
	 public:
		int32_t				check_interval_us_;
		int32_t				retry_interval_us_;
		int32_t				retry_max_times_;
	};

	bool Init(const char* conf_file);

	static bool IsFileUpdated(std::string& file, uint32_t last_time, uint32_t& new_time);

	static void * CheckCb(void *);

	static bool Reload(void*, const std::string&);

 public:
	static Configor& GetInstance(const char* conf_file);

	bool IsReady() {
		return ready_;
	}

	bool Reg(ConfigRegInfo& reg_info);

	int32_t cur_config_index() { return cur_config_index_; }

	void set_cur_config_index(int32_t index) { cur_config_index_ = index; }

	void NextConfigIndex() {
		cur_config_index_ = !cur_config_index_;
	}

	Config& configs(int32_t index) {
		return configs_[index];
	}

	static std::string& name() { return Configor::name_; }

	void Status() {
		std::cout << "Configor name is " << Configor::name_ << std::endl;
		std::cout << "current configure index is " << cur_config_index_ << std::endl;
		std::cout << "\tCheck interval = " << configs_[cur_config_index_].check_interval_us_ << " (us)" << std::endl;
		std::cout << "\tretry max times = " << configs_[cur_config_index_].retry_max_times_ << std::endl;
		std::cout << "\tretry interval = " << configs_[cur_config_index_].retry_interval_us_ << " (us)" << std::endl;
		std::vector<ConfigRegInfo>::iterator iter = regs_.begin();
		for (; iter != regs_.end(); ++iter) {
			std::cout << "Registed: " << iter->config_file_ << std::endl;
		}
		std::cout << std::endl;
	}

 private:
	Configor();
	virtual ~Configor();

	bool ready() {
		if (false == ready_) {
			pthread_mutex_lock(&ready_locker_);
		}

		return ready_;
	}

	void set_ready(bool value) {
		if (false == ready_) {
			pthread_mutex_unlock(&ready_locker_);
		}

		ready_ = value;
	}

 private:
	pthread_mutex_t				reg_locker_;
	pthread_t					check_tid_;
	std::vector<ConfigRegInfo> 	regs_;

	// check interval for files registed to Configor
	Config 				configs_[2];

	int32_t				cur_config_index_;

	static bool				ready_;
	static pthread_mutex_t	ready_locker_;

	static std::string	name_;
	static std::string	desc_;
};

#endif // __CONFIGOR_H
