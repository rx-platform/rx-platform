
<div align="left">
<img src="https://rx-platform.github.io/images/favicon-128.png"  width="128" />
<img src="https://rx-platform.github.io/images/ensaco.png"  width="52" />
</div>

## The Mission

The main intention for building this platform is to connect different kind of programmers 
(Industrial Languages, C, C++, C#, Go, Java, Python) on the same project
and to enable them to collaborate and share ideas and solutions.


<h2 align="left">
<b>{rx-platform}</b>
</h2>

`{rx-platform}` is distributed platform for building applications that exchange Real-Time data. 
It enables seamless integration of various OT (Operational Technology) systems,
providing a unified framework for data acquisition, processing, and visualization.

- You can download the latest version of platform binaries from [this link](http://rx-platform.org).
- Commercial support is available at [ENSACO Solutions doo.](https://ensaco.rs/)
- Documentation is available at [this link](https://ensaco.rs/)

## The Code...

This repository is the main repository of the platform itself
and provides source code for the platform runtime and the plugin interfaces.
The section of the main function for one of the _"hosts"_
, please notice double includes without pre-processor definitions regarding operating system, is shown below:

```cpp
// Windows Win32/WinRT executable - Console App
#include "os_itf/windows/rx_win.h"

// GNU Linux executable
#include "os_itf/linux/rx_linux.h"

// the rx-platform
#include "rx_platform.h"

// the Win32 host
#include "win32_hosts/rx_win32_interactive.h"

// the GNU host
#include "gnu_hosts/rx_gnu_console.h"

// storages
#include "storage/rx_file_storage.h"

int main(int argc, char* argv[])
{    
	// register storages
	std::vector<storage_base::rx_platform_storage_type*> storages;
	storage::files::file_system_storage_type file;
	storages.emplace_back(&file);

	
	// create the Win32 host
	win32::win32_interactive_host host(storages);

	// create the GNU host
	gnu::gnu_console_host host(storages);

	// add staticly linked plug-ins
	// really not recomended, better to use dynamic plug-ins!
	std::vector<library::rx_plugin_base*> plugins;

	// run console
	auto ret = host.console_main(argc, argv, plugins);

	return ret;
}
```
So far so much, for more details please check the documentation ;). 