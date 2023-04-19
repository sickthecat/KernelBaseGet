# KernelBaseGet
Kernel Base Address with ASLR enabled? Hopefully this takes into account ASLR right. Still a learning kitty cat.


The program then obtains the necessary privileges to perform certain operations using the `OpenProcessToken` and `AdjustTokenPrivileges` functions. It then opens the target process using the `OpenProcess` function, retrieves its token using `OpenProcessToken`, and duplicates it using `DuplicateTokenEx`.

Next, the program uses `EnumDeviceDrivers` to obtain the base addresses of all loaded device drivers, and searches for the `ntoskrnl.exe` driver by comparing its name with the base name of each driver using `GetDeviceDriverBaseNameA`.

Once the `ntoskrnl.exe` driver is found, the program opens the target process again and uses `EnumProcessModules` to obtain a list of all loaded modules in the process. It then searches for the `ntoskrnl.exe` module by comparing the module name with the full path of each module using `GetModuleFileNameEx`. Once found, it obtains information about the module using `GetModuleInformation` and displays its base address.

The program then terminates by closing all handles and returning 0.


Some code reused.
Derp.

