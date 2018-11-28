#pragma once

// directories stuff
#define RX_NS_WORLD_NAME "world"
#define RX_NS_STORAGE_NAME "storage"
#define RX_NS_SYS_NAME "_sys"
#define RX_NS_TEST_NAME "_test"
#define RX_NS_UNASSIGNED_NAME "_unassigned"
#define RX_NS_BIN_NAME "bin"
#define RX_NS_OBJ_NAME "objects"
#define RX_NS_CLASS_NAME "classes"
#define RX_NS_PLUGINS_NAME "plugins"
#define RX_NS_HOST_NAME "host"

#define RX_NULL_ITEM_NAME "<nullptr>"


// objects stuff
#define RX_NS_SYSTEM_APP_NAME "system_app"
#define RX_NS_SYSTEM_APP_ID 0x00000001
#define RX_NS_SYSTEM_DOM_NAME "system"
#define RX_NS_SYSTEM_DOM_ID 0x00000002

#define NS_RX_COMMANDS_MANAGER_NAME "cmd_manager"
#define RX_COMMANDS_MANAGER_ID 0x80000001
#define NS_RX_INTERACTIVE_NAME "interactive"
#define RX_INTERACTIVE_ID 0x80000001


#define RX_NS_SERVER_RT_NAME "runtime"
#define RX_NS_SERVER_RT_ID 0x80000002
#define RX_IO_MANAGER_NAME "io_manager"
#define RX_IO_MANAGER_ID 0x80000003


#define IO_POOL_NAME "io_pool"
#define IO_POOL_ID 0x80000004
#define GENERAL_POOL_NAME "general_pool"
#define GENERAL_POOL_ID 0x80000005
#define WORKER_POOL_NAME "worker_pool"
#define WORKER_POOL_ID 0x80000006
#define IO_LOG_NAME "log"
#define IO_LOG_ID 0x80000007

#define RX_NS_SYSTEM_UNASS_NAME "unassigned"
#define RX_NS_SYSTEM_UNASS_ID 0x80000009
#define RX_NS_SYSTEM_UNASS_APP_NAME "unassigned_app"
#define RX_NS_SYSTEM_UNASS_APP_ID 0x8000000a


// system objects type stuff
#define RX_NS_SYSTEM_APP_TYPE_NAME "system_app_type"
#define RX_NS_SYSTEM_APP_TYPE_ID 0xc0000000
#define RX_NS_SYSTEM_DOM_TYPE_NAME "system_type"
#define RX_NS_SYSTEM_DOM_TYPE_ID 0xc0000001


#define NS_RX_COMMANDS_MANAGER_TYPE_NAME "cmd_manager_type"
#define RX_COMMANDS_MANAGER_TYPE_ID 0xc0000002
#define NS_RX_CONSOLE_TYPE_NAME "console_type"
#define RX_CONSOLE_TYPE_ID 0xc0000003


#define RX_NS_SERVER_RT_TYPE_NAME "runtime_type"
#define RX_NS_SERVER_RT_TYPE_ID 0xc0000004
#define RX_IO_MANAGER_TYPE_NAME "io_manager_type"
#define RX_IO_MANAGER_TYPE_ID 0xc0000005


#define IO_POOL_TYPE_NAME "io_pool_type"
#define IO_POOL_TYPE_ID 0xc0000006
#define GENERAL_POOL_TYPE_NAME "general_pool_type"
#define GENERAL_POOL_TYPE_ID 0xc0000007
#define WORKER_POOL_TYPE_NAME "worker_pool_type"
#define WORKER_POOL_TYPE_ID 0xc0000008
#define IO_LOG_TYPE_NAME "log_type"
#define IO_LOG_TYPE_ID 0xc0000009
#define RX_NS_SYSTEM_UNASS_TYPE_NAME "unassigned_type"
#define RX_NS_SYSTEM_UNASS_TYPE_ID 0xc000000a
#define RX_NS_SYSTEM_UNASS_APP_TYPE_NAME "unassigned_app_type"
#define RX_NS_SYSTEM_UNASS_APP_TYPE_ID 0xc000000b

// cpp classes stuff
#define RX_CPP_OBJECT_CLASS_TYPE_NAME "object_type"
#define RX_CPP_OBJECT_TYPE_NAME "object"
#define RX_CPP_DOMAIN_CLASS_TYPE_NAME "domain_type"
#define RX_CPP_DOMAIN_TYPE_NAME "domain"
#define RX_CPP_APPLICATION_CLASS_TYPE_NAME "application_type"
#define RX_CPP_APPLICATION_TYPE_NAME "application"
#define RX_CPP_PORT_CLASS_TYPE_NAME "port_type"
#define RX_CPP_PORT_TYPE_NAME "port"
#define RX_TEST_CASE_TYPE_NAME "test_case"

#define RX_CPP_COMMAND_TYPE_NAME "command"
#define RX_CPP_PROGRAM_TYPE_NAME "program"

#define RX_CONST_VALUE_TYPE_IDX 1
#define RX_CONST_VALUE_TYPE_NAME "const_value"
#define RX_VALUE_TYPE_IDX 1
#define RX_VALUE_TYPE_NAME "value"

#define RX_CPP_VARIABLE_CLASS_TYPE_NAME "variable_type"
#define RX_CPP_VARIABLE_TYPE_NAME "variable"
#define RX_CPP_STRUCT_CLASS_TYPE_NAME "struct_type"
#define RX_CPP_STRUCT_TYPE_NAME "struct"
#define RX_CPP_MAPPER_CLASS_TYPE_NAME "mapper_type"
#define RX_CPP_MAPPER_TYPE_NAME "mapper"
#define RX_CPP_SOURCE_CLASS_TYPE_NAME "source_type"
#define RX_CPP_SOURCE_TYPE_NAME "source"
#define RX_CPP_FILTER_CLASS_TYPE_NAME "filter_type"
#define RX_CPP_FILTER_TYPE_NAME "filter"
#define RX_CPP_EVENT_CLASS_TYPE_NAME "event_type"
#define RX_CPP_EVENT_TYPE_NAME "event"

//Main messages
#define RX_ACCESS_DENIED "*ACCESS DENIED*"

// Console related stuff
#define RX_CONSOLE_HEADER_LINE "============================================="


//Testing related stuff
#define RX_TEST_STATUS_UNKNOWN 0
#define RX_TEST_STATUS_OK 1
#define RX_TEST_STATUS_FAILED 2


#define RX_TEST_STATUS_UNKNOWN_NAME RX_NULL_ITEM_NAME
#define RX_TEST_STATUS_OK_NAME "Passed"
#define RX_TEST_STATUS_FAILED_NAME "*FAILED*"
#define RX_TESTING_CON_LINE "\r\n=================================="

//Storage related stuff
#define RX_FILE_STORAGE_FOLDER "storage"

