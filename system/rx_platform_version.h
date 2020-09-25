#pragma once

#include "version/rx_version.h"

#define RX_SERVER_NAME "Atom" 
#define RX_SERVER_MAJOR_VERSION 0
#define RX_SERVER_MINOR_VERSION 25
#define RX_SERVER_BUILD_NUMBER 2

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

