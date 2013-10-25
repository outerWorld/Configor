#include "ini_file.h"

#include "configor.h"

std::string Configor::name_("Configor");
std::string Configor::desc_("a management module for configuration");
bool Configor::ready_ = false;
pthread_mutex_t Configor::ready_locker_ = PTHREAD_MUTEX_INITIALIZER;

Configor::Configor() {
	pthread_mutex_init(&reg_locker_, NULL);
	pthread_mutex_init(&ready_locker_, NULL);
	cur_config_index_ = 0;
}

Configor::~Configor() {
}

Configor& Configor::GetInstance(const char* conf_file) {
	static Configor configor;

	if (false == configor.ready()) {
		bool ret = configor.Init(conf_file); 
		if (true == ret) {
			configor.set_ready(true);
		} else {
			configor.set_ready(false);
		}
	}

	return configor;
}


bool Configor::Reg(ConfigRegInfo& reg_info) {
	if (0 != pthread_mutex_lock(&reg_locker_)) {
		return false;
	}

	// the usage of "do {}while(0);" is to make every invalid condition can go to the end and execute pthread_mutex_unlock
	do {
		regs_.push_back(reg_info);
	} while (0);

	pthread_mutex_unlock(&reg_locker_);
	return true;
}

bool Configor::Init(const char* conf_file) {

	std::string __conf_file(conf_file);
	ConfigRegInfo reg_info((void*)this, __conf_file, Reload);

	if (true != reg_info.IsValid()) {
		return false;
	}
	if (false == Reg(reg_info)) {
		return false;
	}

	if (false == Reload(this, __conf_file)) {
		return false;
	}

	if (0 != pthread_create(&check_tid_, NULL, CheckCb, this)) {
		return false;
	}

	return true;
}

bool Configor::IsFileUpdated(std::string& file, uint32_t last_time, uint32_t& new_time) {
	struct stat st;
		
	if (0 != stat(file.c_str(), &st)) {
		return false;
	}

	new_time = st.st_mtime;
	if (new_time == last_time) {
		return false;
	}

	return true;
}

void * Configor::CheckCb(void *arg) {
	Configor *configor = (Configor*)arg;

	// As the Configuration of Configor is managed by itself,
	// the data of Configor must be accessed by Configor object!!!
	bool b_ret = false;
	uint32_t file_new_time = 0;
	while (1) {
		std::vector<ConfigRegInfo>::iterator iter = configor->regs_.begin();
		for (; iter != configor->regs_.end(); ++iter) {
			b_ret = IsFileUpdated(iter->config_file_, iter->last_time_, file_new_time);
			if (false == b_ret) {
				continue;
			}
			for (int32_t retry_times = 0; retry_times < configor->configs(configor->cur_config_index()).retry_max_times_; ++retry_times) {
				if (true == iter->reload_func_(iter->param_, iter->config_file_)) {
					iter->last_time_ = file_new_time;
					break;
				}
				usleep(configor->configs(configor->cur_config_index()).retry_interval_us_);
			}
		}

		usleep(configor->configs(configor->cur_config_index()).check_interval_us_);
	}

	return NULL;
}

bool Configor::Reload(void* param, const std::string& conf_file) {
	Configor *configor = (Configor*)param;
	IniFile reader;

	if (true != reader.Init(conf_file)) {
		return false;
	}
	
	// !0 = 1 , !1 = 0, so the index of current configuration is range of [0, 1].
	int32_t next_index = !configor->cur_config_index();
	uint32_t value = 0;
	Config& config = configor->configs(next_index);

	config.check_interval_us_ = reader.Get("Configor", "check_interval", 10000000);

	config.retry_max_times_ = reader.Get("Configor", "retry_max_times", 3);

	config.retry_interval_us_ = reader.Get("Configor", "retry_interval", 10000);


	configor->set_cur_config_index(next_index);

	return true;
}
