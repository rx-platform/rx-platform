# RX Platform 

[![N|Solid](https://rx-platform.github.io/images/processor-64-dis.png)](https://rx-platform.github.io/images/processor-64-dis.png)


## The Mission

The main intention for building this platform is to connect different kind of programmers (Industrial Languages, C, C++, C#, Java, Python) on the same project and to enable them to collaborate and share ideas and solutions. 


### The Code...

Start with the main function. This function is actually left out from the repository. It depends on your environment so no projects are uploaded. If you set your working environment correctly you just write the missing main function.

On GNU/Linux machines this is the main function example: 

```cpp
 // this is a gnu/linux platform
 #include "os_itf/linux/rx_linux.h"
 
 // a platform itself
 #include "rx_platform.h"

 // the host
 #include "host/gnu_hosts/rx_gnu_console.h"

 int main(int argc, char* argv[])
 {
 	// create the host
 	host::gnu::gnu_console_host host;
 	// run console
 	return host.console_main(argc,argv);
 }
```

On Windows machines this is the main function example: 
```cpp
 // this is a windows platform 
 #include "os_itf/windows/rx_win.h"

 // a platform itself
 #include "rx_platform.h"

 // the host
 #include "host/win32_hosts/rx_win32_console.h"

 int main(int argc, char* argv[])
 {
 	// create the host
 	host::win32::win32_console_host host;
 	// run console
 	return host.console_main(argc, argv);
 }
```
For more information about hte code see documentation repository wiki at [https://github.com/rx-platform/rx-docs/wiki](https://github.com/rx-platform/rx-docs/wiki)
