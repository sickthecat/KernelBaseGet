/*
Reading: https://github.com/FULLSHADE/Win10InfoLeaks
Reading: https://twitter.com/33y0re/status/1278104088090116097?lang=en
Reading: https://connormcgarr.github.io/swimming-in-the-kernel-pool-part-1/
It says medium integrity, but even with an admin account it was pissing mew off.. 
so I reused some code from a previous project.
I don't actually know if this is gonna work right... 
I've done the best meow I could muster.
ASLR is hard.

BTC for wtfe: bc1qywm3pcgtwv2wx42ue9zelepdgukp4t94krh0va


Random Taort card:

Temperance (XIV):
Is the fourteenth trump or Major Arcana card in most traditional Tarot decks.
Depiction: 
Temperance is almost invariably depicted as a person pouring liquid from one receptacle into another. 
In many decks, the person is a winged person/angel, usually female or androgynous, and stands with one foot on water and one foot on land.
Meaning: This card indicates that you should learn to bring about balance, patience and moderation in your life. 
You should take the middle road, avoiding extremes and maintain a sense of calm.
Upright card (keywords): Alchemy, Balance, Connecting with your guides, Harmony, Looking for divine intervention, Moderation.
Reversed card (keywords): Disharmony, Imbalance, Lack of patience, Onset of illness.
*/

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib") // link against Psapi.lib
#pragma comment(lib, "Advapi32.lib") // link against Advapi32.lib

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "  /\\_/\\   " << std::endl;
        std::cout << " ( o.o )  " << std::endl;
        std::cout << "  > ^ < \n  " << std::endl;
        std::cout << " M E O W    " << std::endl;
        std::cout << " _SiCk   " << std::endl;
        std::cout << "Website: https://afflicted.sh " << std::endl;
        std::cout << "Usage: " << argv[0] << " <PID>" << std::endl;
        return 0;
    }

    DWORD pid = atoi(argv[1]);

    // Enable debug privilege
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        std::cout << "OpenProcessToken failed: " << GetLastError() << std::endl;
        return 0;
    }

    TOKEN_PRIVILEGES tk;
    tk.PrivilegeCount = 1;
    tk.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tk.Privileges[0].Luid)) {
        std::cout << "LookupPrivilegeValue failed: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return 0;
    }

    if (!AdjustTokenPrivileges(hToken, FALSE, &tk, 0, nullptr, nullptr)) {
        std::cout << "AdjustTokenPrivileges failed: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return 0;
    }

    CloseHandle(hToken);

    // Open the process
    HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!process_handle) {
        std::cout << "Failed to open process: " << GetLastError() << std::endl;
        return 0;
    }

    // Get the process token
    HANDLE process_token = nullptr;
    if (!OpenProcessToken(process_handle, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &process_token)) {
        std::cout << "Failed to open process token: " << GetLastError() << std::endl;
        CloseHandle(process_handle);
        return 0;
    }

    // Duplicate the process token
    HANDLE new_token = nullptr;
    if (!DuplicateTokenEx(process_token, MAXIMUM_ALLOWED, nullptr, SecurityImpersonation, TokenPrimary, &new_token)) {
        std::cout << "Failed to duplicate token: " << GetLastError() << std::endl;
        CloseHandle(process_token);
        CloseHandle(process_handle);
        return 0;
    }

    CloseHandle(process_token);
    CloseHandle(process_handle);

    // Use EnumDeviceDrivers to get the base address of ntoskrnl.exe
    LPVOID drivers[1024];
    DWORD cbNeeded;
    if (!EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)) {
        std::cout << "Error enumerating device drivers" << std::endl;
        CloseHandle(new_token);
        return -1;
    }

    int nDrivers = cbNeeded / sizeof(drivers[0]);
    for (int i = 0; i < nDrivers; i++) {
        char driverName[1024];
        if (GetDeviceDriverBaseNameA(drivers[i], driverName, sizeof(driverName))) {
            if (strcmp(driverName, "ntoskrnl.exe") == 0) {
                std::cout << "ntoskrnl.exe base address: 0x" << std::hex << drivers[i] << std::endl;

                // Get the process handle for the given PID
                HANDLE processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
                if (!processHandle) {
                    std::cout << "Failed to open process: " << GetLastError() << std::endl;
                    CloseHandle(new_token);
                    return -1;
                }

                // Get the base address and size of the process's virtual memory
                HMODULE hMods[1024];
                DWORD cbNeeded;
                if (EnumProcessModules(processHandle, hMods, sizeof(hMods), &cbNeeded)) {
                    for (int j = 0; j < (cbNeeded / sizeof(HMODULE)); j++) {
                        TCHAR szModName[MAX_PATH];

                        // Get the module name
                        if (GetModuleFileNameEx(processHandle, hMods[j], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                            if (_tcsicmp(szModName + _tcslen(szModName) - _tcslen(TEXT("\\ntoskrnl.exe")), TEXT("\\ntoskrnl.exe")) == 0) {
                                MODULEINFO moduleInfo;
                                if (GetModuleInformation(processHandle, hMods[j], &moduleInfo, sizeof(moduleInfo))) {
                                    std::cout << "ntoskrnl.exe module base address: 0x" << std::hex << moduleInfo.lpBaseOfDll << std::endl;
                                }
                                else {
                                    std::cout << "Error getting module information: " << GetLastError() << std::endl;
                                }

                                break;
                            }
                        }
                        else {
                            std::cout << "Error getting module file name: " << GetLastError() << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "Error enumerating process modules: " << GetLastError() << std::endl;
                }

                CloseHandle(processHandle);

                break;
            }
        }
    }

    CloseHandle(new_token);

    return 0;
}
