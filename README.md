
![perica](https://rx-platform.github.io/images/favicon-128.png)


## The Mission

The main intention for building this platform is to connect different kind of programmers (Industrial Languages, C, C++, C#, Java, Python) on the same project and to enable them to collaborate and share ideas and solutions.


## RX Platform

Read or download RX Platform Documentation. From general documents for users to source code documentation. It is currentlly in a "wiki phase" so first take a look at [Wiki Pages](https://github.com/rx-platform/rx-docs/wiki) of this repository.

## The Code...

Start with the main function. This function is actually left out from the repository. It depends on your environment so no projects are uploaded. If you set your working environment correctly you just write the missing main function.

On GNU/Linux machines this is the main function example:

```cpp
 // this is a linux platform
 #include "os_itf/linux/rx_linux.h"

 // a platform itself
 #include "rx_platform.h"

 // the host...
#include "gnu_hosts/rx_gnu_console.h"
// and the plugins...
#include "local-plugins/modbus/include/rx_modbus_plugin.h"
#include "local-plugins/opc-ua/include/rx-opc-ua.h"

int main(int argc, char* argv[])
{
    // create the storages
    storage::files::file_system_storage_holder system;
    storage::files::file_system_storage_holder user;
    storage::files::file_system_storage_holder test;
    hosting::rx_host_storages storages;
    storages.system_storage = &system;
    storages.user_storage = &user;
    storages.test_storage = &test;
    
    // create the host
    gnu::gnu_console_host host(storages);
    // add plig-ins
    std::vector<library::rx_plugin_base*> plugins;

    protocols::modbus::rx_modbus_plugin modbus_plugin;
    plugins.emplace_back(&modbus_plugin);

    protocols::opc_ua::rx_opc_ua_plugin opc_ua_plugin;
    plugins.emplace_back(&opc_ua_plugin);
    // run console
    return host.console_main(argc, argv, plugins);
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

// and the plugins...
#include "local-plugins/modbus/include/rx_modbus_plugin.h"
#include "local-plugins/opc-ua/include/rx-opc-ua.h"

int main(int argc, char* argv[])
{
    // create the storages
    storage::files::file_system_storage_holder system;
    storage::files::file_system_storage_holder user;
    storage::files::file_system_storage_holder test;
    hosting::rx_host_storages storages;
    storages.system_storage = &system;
    storages.user_storage = &user;
    storages.test_storage = &test;
    
    // create the host
    gnu::win32_console_host host(storages);
    // add plig-ins
    std::vector<library::rx_plugin_base*> plugins;

    protocols::modbus::rx_modbus_plugin modbus_plugin;
    plugins.emplace_back(&modbus_plugin);

    protocols::opc_ua::rx_opc_ua_plugin opc_ua_plugin;
    plugins.emplace_back(&opc_ua_plugin);
    // run console
    return host.console_main(argc, argv, plugins);
}
```
For more information about the code see documentation repository wiki at [https://github.com/rx-platform/rx-docs/wiki/The-Code](https://github.com/rx-platform/rx-docs/wiki/The-Code)
