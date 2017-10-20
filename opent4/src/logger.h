#pragma once
#include <vector>
#include <string>
#include <mutex>
using namespace std;

#include <stdarg.h>
class global_logger {
	public:
		static global_logger* get();

		void log(const char* fmt, ...);

		mutex log_mutex;
		vector<string> logs;

	private:
		char m_buf[512];
		static global_logger* m_instance;
		global_logger();
};



#define printf global_logger::get()->log
