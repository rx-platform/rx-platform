#pragma once

#include "version/rx_version.h"

#define RX_SERVER_NAME "Molecule"
#define RX_SERVER_MAJOR_VERSION 2
#define RX_SERVER_MINOR_VERSION 3
#define RX_SERVER_BUILD_NUMBER 3

// version 0.14 introduced runtime_data<>
// version 0.14.9 introduced rx_result<>
// version 0.15.0 rx platform API
// version 0.15.2 introduced storage
// version 0.15.11 introduced model queries
// version 0.16.0 introduced rx_item_type
// version 0.16.1 introduced types_resolver
// version 0.17.0 introduced runtime internals and runtime manager
// version 0.18.0 introduced runtime subscription and "tags browsing"
// version 0.19.0 introduced item_reference type
// version 0.19.3 introduced resolve_reference api-s
// version 0.20.0 introduced storage_holder
// version 0.21.0 introduced relations basics
// version 0.22.0 runtime_holder is now template that is root to all runtime items
// version 0.23.0 introduced rx_internal namespace, async functions to internal space
// version 0.24.0 first process runtime algorithm
// version 0.25.0 port relations done
// version 1.0.0 first release version
// version 1.0.1 tcp/ip connection, housekeeping and rx-architect
// version 1.0.2 master port and more rx-architect
// version 1.1.0 domain, application and parent types are now item reference instead of node id
// version 1.1.1 code creation process done with plugins implementing it
// version 1.2.0 data_type, display_type and method_type added to platform
// version 1.2.1 block_data implemented in runtime
// version 1.2.8 relations stuff and http protocol
// version 1.3.0 added constraints for inheritance
// version 1.3.1 added constraints for relation targets
// version 1.4.0 moved a lot of runtime to internal files
// version 1.4.1 support for rx-common shared library
// version 1.5.0 added callback support rx_any_callback<> and rx_remote_function<>
// version 1.6.0 source input and mapper mapped value as tag
// version 1.7.0 exporting functions for dynamic plugins
// version 1.8.0 added basic opc ua server
// version 1.8.3 finished basic opc ua server
// version 1.8.3 added basic opc ua client
// version 1.9.0 added ethernet port
// version 1.10.0 CHANGED ABI!!!! (without versioning!!!)
// version 1.10.1 added signal level support
// version 1.10.2 added some discovery features, ARM32 dynamic linking
// version 1.11.0 ABI and Common versioning, Data Structs, added Web Socket support
// version 2.0.0 added security and meta moved to rx-common
// version 2.1.0 added simple mqtt implementation
// version 2.2.0 added const, value and variable blocks
// version 2.3.0 resolved execute for both methods

