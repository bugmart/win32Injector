// Injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include "Windows.h"

bool InjectDll2Process(int targetProcessId, char* dllName);
int main(int argc, char* argv[])
{    
    std::cout << "Have " << argc << " arguments:\n";
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << "\n";
    }

    if (argc != 3)
    {
        std::cout << "The number of argments does not match\n";
        return 0;
    }

    int processId = atoi(argv[1]);
    char* dllName = argv[2];

    InjectDll2Process(processId, dllName);
}

bool InjectDll2Process(int targetProcessId, char* dllName)
{
    //open process by processID;
    HANDLE targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
    if (targetProcessHandle == NULL)
    {
        std::cout << "open process failed id=" << targetProcessId << "\n";
        return false;
    }

    int pathLength = strlen(dllName);

    //allocate path memory in targetProcess
    LPVOID remoteProcessBuffer = VirtualAllocEx(targetProcessHandle, NULL, pathLength + 1, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (remoteProcessBuffer == NULL)
    {
        std::cout << "alloc memory failed size=" << pathLength + 1 << "\n";
        return false;
    }
    //copy dllName to target process's buffer
    bool res = WriteProcessMemory(targetProcessHandle, remoteProcessBuffer, dllName, pathLength + 1, NULL);
    if (res == false)
    {
        std::cout << "write to process memory failed\n";
        return false;
    }
        
    //get target process's "LoadLibraryA" function address;
    HMODULE kernel32Module = LoadLibraryA("kernel32.dll");
    LPTHREAD_START_ROUTINE loadLibraryFunc = (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32Module, "LoadLibraryA");

    //remote call target process's LoadLibraryA function
    DWORD remoteThreadId = 0;
    HANDLE threadHandle = CreateRemoteThread(targetProcessHandle, NULL, 0, loadLibraryFunc, remoteProcessBuffer, 0, &remoteThreadId);
    if (threadHandle == NULL)
    {
        std::cout << "create remote thread failed\n";
        return false;
    }

    std::cout << "create remote thread succeed threadid=" << remoteThreadId <<"\n";

    //close targetProcess Handle
    CloseHandle(targetProcessHandle);

    return true;
}

