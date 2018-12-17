
![perica](https://rx-platform.github.io/images/favicon-64.png)


## The Mission

The main intention for building this platform is to connect different kind of programmers (Industrial Languages, C, C++, C#, Java, Python) on the same project and to enable them to collaborate and share ideas and solutions.


## RX Platform

Read or download RX Platform Documentation. From general documents for users to source code documentation. It is currentlly in a "wiki phase" so first take a look at [Wiki Pages](https://github.com/rx-platform/rx-docs/wiki) of this repository.

## The Code...

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
	// create the storage
	auto storage = rx_create_reference<gnu::gnu_file_system_storage>();
  	// create the host
 	host::gnu::gnu_console_host host(storage);
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
	// create the storage
	auto storage = rx_create_reference<win32::win32_file_system_storage>();
  	// create the host
	win32::win32_console_host host(storage);
 	// run console
 	return host.console_main(argc, argv);
 }
```
For more information about the code see documentation repository wiki at [https://github.com/rx-platform/rx-docs/wiki/The-Code](https://github.com/rx-platform/rx-docs/wiki/The-Code)
