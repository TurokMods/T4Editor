#include <logger.h>
#include <memory.h>

global_logger* global_logger::m_instance = 0;

global_logger* global_logger::get() {
	if(!m_instance) m_instance = new global_logger();
	return m_instance;
}

void global_logger::log(const char* fmt, ...) {
	va_list a;
	va_start(a, fmt);
	memset(m_buf, 0, 512);
	vsprintf(m_buf, fmt, a);
	logs.push_back(string(m_buf));
	vprintf(m_buf, a);
	va_end(a);
	log_mutex.lock();
	if(logs.size() == 100) {
		logs.erase(logs.begin());
	}
	log_mutex.unlock();
}
global_logger::global_logger() {
	logs.reserve(101);
}
