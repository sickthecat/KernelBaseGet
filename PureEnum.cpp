#include <iostream>
#include <Windows.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

using namespace std;

DWORD64 GetKernelBase()
{
    DWORD cbNeeded;
    LPVOID drivers[1024];

    if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded))
    {
        return (DWORD64)drivers[0];
    }

    return 0;
}

int main()
{
    DWORD64 kernelBase = GetKernelBase();

    if (kernelBase == 0)
    {
        cout << "Failed to get kernel base address." << endl;
        return 1;
    }

    cout << "Obtained kernel base address: 0x" << hex << kernelBase << endl;

    return 0;
}
