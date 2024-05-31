#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <vector>

int main() {
	SetConsoleTitleA("I love you mom");

	printf("Ready?\n");
	system("pause");

	DWORD pid = 0;
	GetWindowThreadProcessId(FindWindowA(0, "Roblox"), &pid);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

	if (!hProcess) {
		printf("Couldn't open Roblox.\n");
		
		system("pause");
		return 1;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	std::vector<DWORD> tids{}; // Thread IDs
	
	while (Thread32Next(hSnapshot, &threadEntry)) {
		if (threadEntry.th32OwnerProcessID == pid) {
			tids.push_back(threadEntry.th32ThreadID);
		}
	} CloseHandle(hSnapshot);

	// This could've been shorter but I like things organized.
	for (int i = 0; i < tids.size(); i++) {
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, 0, tids.at(i));
		
		SuspendThread(hThread);
	}

	HANDLE hModSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	DWORD64 modBase = 0;
	DWORD modSize = 0;
	while (Module32Next(hModSnapshot, &modEntry)) {
		if (!_strcmpi(modEntry.szModule, "RobloxPlayerBeta.exe")) {
			modBase = (DWORD64)modEntry.modBaseAddr;
			modSize = modEntry.modBaseSize;

			break;
		}
	} CloseHandle(hModSnapshot);

	// printf("Base: 0x%llx\nSize: 0x%x\n", modBase, modSize);

	DWORD oldProtection = 0;
	if (!VirtualProtectEx(hProcess, (void*)modBase, modSize, PAGE_READONLY, &oldProtection)) {
		printf("Something went wrong. 0x%x\n", GetLastError());
		
		system("pause");
		return 1;
	}

	printf("Done. For security reasons, Roblox will be terminated after closing this console.\n");
	system("pause");
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
	return 0;
}