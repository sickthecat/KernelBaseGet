# KernelBaseGet
Kernel Base Address with ASLR enabled? Hopefully this takes into account ASLR right. Still a learning kitty cat.


# Method 1: Using RtlPcToFileHeader
The first method involves using the `RtlPcToFileHeader` function to retrieve the base address of the kernel. This method requires that the application has the necessary permissions to access kernel memory, which may not be possible or safe in all circumstances.


# Method 2: Using SymGetModuleBase64
The second method involves using the `SymGetModuleBase64` function from the Debug Help Library (dbghelp.dll) to retrieve the base address of the kernel. This method is safer than the first method and does not require kernel access, but it may be affected by Address Space Layout Randomization (ASLR).
