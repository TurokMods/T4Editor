#include <app.h>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#include <Winbase.h>
#include <Tlhelp32.h>
DWORD FindProcessId(char* processName)
{
	// strip path

	char* p = strrchr(processName, '\\');
	if(p)
		processName = p+1;

	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if ( processesSnapshot == INVALID_HANDLE_VALUE )
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if ( !strcmp(processName, processInfo.szExeFile) )
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while ( Process32Next(processesSnapshot, &processInfo) )
	{
		if ( !strcmp(processName, processInfo.szExeFile) )
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}
VOID start_process(LPCTSTR lpApplicationName)
{
   // additional information
   STARTUPINFO si;     
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

  // start the program up
  CreateProcess( lpApplicationName,   // the path
    "",				// Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
#endif

namespace t4editor {
	void monitor_game_process(application* app) {
		#ifdef _WIN32
		bool foundProcess = false;
		bool stillRunning = false;

		while(foundProcess == stillRunning) {
			Sleep(100);
			DWORD handle = FindProcessId("Turok4.exe");
			if(handle != 0) {
				foundProcess = true;
				stillRunning = true;
			}
			if(handle == 0 && foundProcess) stillRunning = false;
		}

		app->level_test_done();
		return;

		#else
			return;
		#endif
	}

	void update_actor_func(application* app, const string& path) {
		app->getTurokData()->update_actor_cache(path);
	}

	void restore_backup_func(application* app) {
		app->getTurokData()->restore_backup();
	}

	void application::update_actor_cache() {
		string path = m_editorDataPath + "/actor_cache.bin";
		thread t = thread(update_actor_func, this, path);
		t.detach();
	}

	void application::restore_backups() {
		thread t = thread(restore_backup_func, this);
		t.detach();
	}

	void application::monitor_testing(const string& path) {
		#ifdef _WIN32
		start_process(path.c_str());
		#endif
		thread t = thread(monitor_game_process, this);
		t.detach();
	}
}
