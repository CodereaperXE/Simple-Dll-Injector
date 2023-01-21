#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

const char *procName = "notepad.exe";

DWORD GetProcId(const char *procName) {

	DWORD procId=0;

	HANDLE hSnap= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {
			do {
				if (!_stricmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procId;
}



void Inject(DWORD procId) {

	char libPath[] = "C:\\Users\\vortex\\source\\repos\\monitordll\\x64\\Debug";

	HANDLE hProc;

	void* procAddr;

	

	hProc=OpenProcess(PROCESS_ALL_ACCESS, 1, procId);
	
	if (hProc == INVALID_HANDLE_VALUE) {

		std::cout << "Error in opening handle to program" << std::endl;

		exit(0);

	}

	procAddr = VirtualAllocEx(hProc, NULL, sizeof(libPath), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (procAddr == NULL) {

		std::cout << "Memory allocation failed" << std::endl;

		exit(0);

	}

	WriteProcessMemory(hProc, (LPVOID)procAddr, (LPCVOID)libPath, sizeof(libPath), NULL);

	HANDLE hThread=CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, procAddr, 0, 0);

	if (hThread)
		CloseHandle(hThread);

	if (hProc)
		CloseHandle(hProc);

}

int main() {
	Inject(GetProcId(procName));	
	return 0;
}
