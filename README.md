# win32Injector
In a Win32 application, we inject our DLL into the target process and hijack the code flow to our function. 
For example, this allows us to log arguments, registry keys, and the values at specific addresses.

build
- injector
  
gcc injector.cpp -o injector.exe

- libhook
  
gcc --shared -o libhook.dll dllmain.cpp hook.cpp 

in visual studio, use win32Injector.sln
