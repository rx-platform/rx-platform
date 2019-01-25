#pragma once

#define RX_DIR_DELIMETER '/'
#define RX_OBJECT_DELIMETER '.'

// directories stuff
#define RX_NS_WORLD_NAME "world"
#define RX_NS_STORAGE_NAME "storage"
#define RX_NS_SYS_NAME "_sys"
#define RX_NS_TEST_NAME "_test"
#define RX_NS_UNASSIGNED_NAME "_unassigned"
#define RX_NS_BIN_NAME "bin"
#define RX_NS_OBJ_NAME "objects"
#define RX_NS_SYSTEM_OBJ_NAME "system"
#define RX_NS_PORT_OBJ_NAME "ports"
#define RX_NS_CLASSES_NAME "types"
#define RX_NS_BASE_CLASSES_NAME "base"
#define RX_NS_SYSTEM_CLASSES_NAME "system"
#define RX_NS_PORT_CLASSES_NAME "ports"
#define RX_NS_PLUGINS_NAME "plugins"
#define RX_NS_HOST_NAME "host"

#define RX_NULL_ITEM_NAME "<nullptr>"

// basic classes
#define RX_CLASS_OBJECT_BASE_NAME "ObjectBase"
#define RX_CLASS_OBJECT_BASE_ID 0x00000001
#define RX_CLASS_DOMAIN_BASE_NAME "DomainBase"
#define RX_CLASS_DOMAIN_BASE_ID 0x00000002
#define RX_CLASS_APPLICATION_BASE_NAME "ApplicationBase"
#define RX_CLASS_APPLICATION_BASE_ID 0x00000003
#define RX_CLASS_PORT_BASE_NAME "PortBase"
#define RX_CLASS_PORT_BASE_ID 0x00000004

#define RX_CLASS_STRUCT_BASE_NAME "StructBase"
#define RX_CLASS_STRUCT_BASE_ID 0x00000005
#define RX_CLASS_VARIABLE_BASE_NAME "VariableBase"
#define RX_CLASS_VARIABLE_BASE_ID 0x00000006
#define RX_CLASS_EVENT_BASE_NAME "EventBase"
#define RX_CLASS_EVENT_BASE_ID 0x00000007
#define RX_CLASS_SOURCE_BASE_NAME "SourceBase"
#define RX_CLASS_SOURCE_BASE_ID 0x00000008
#define RX_CLASS_MAPPER_BASE_NAME "MapperBase"
#define RX_CLASS_MAPPER_BASE_ID 0x00000009
#define RX_CLASS_FILTER_BASE_NAME "FilterBase"
#define RX_CLASS_FILTER_BASE_ID 0x0000000a


// objects stuff
#define RX_NS_SYSTEM_APP_NAME "SystemApp"
#define RX_NS_SYSTEM_APP_ID 0x0000000b
#define RX_NS_SYSTEM_DOM_NAME "SystemDomain"
#define RX_NS_SYSTEM_DOM_ID 0x0000000c

#define NS_RX_COMMANDS_MANAGER_NAME "CmdManager"
#define RX_COMMANDS_MANAGER_ID 0x8000001

#define RX_NS_SERVER_RT_NAME "Runtime"
#define RX_NS_SERVER_RT_ID 0x8000002
#define RX_IO_MANAGER_NAME "IoManager"
#define RX_IO_MANAGER_ID 0x8000003


#define IO_POOL_NAME "IoPool"
#define IO_POOL_ID 0x8000004
#define GENERAL_POOL_NAME "GeneralPool"
#define GENERAL_POOL_ID 0x8000005
#define WORKER_POOL_NAME "WorkerPool"
#define WORKER_POOL_ID 0x8000006
#define IO_LOG_NAME "Log"
#define IO_LOG_ID 0x8000007

#define RX_NS_SYSTEM_UNASS_NAME "UnassignedDomain"
#define RX_NS_SYSTEM_UNASS_ID 0x8000009
#define RX_NS_SYSTEM_UNASS_APP_NAME "UnassignedApp"
#define RX_NS_SYSTEM_UNASS_APP_ID 0x800000a


// port runtime objects
#define RX_PHYSICAL_PORT_NAME "PhysicalPort"
#define RX_PHYSICAL_PORT_ID 0x800000c
#define RX_TRANSPORT_PORT_NAME "TransportPort"
#define RX_TRANSPORT_PORT_ID 0x800000d
#define RX_PROTOCOL_PORT_NAME "ProtocolPort"
#define RX_PROTOCOL_PORT_ID 0x800000e

#define RX_TTY_PORT_NAME "TTYPort"
#define RX_TTY_PORT_ID 0x8000010
#define RX_UDP_PORT_NAME "UDPPort"
#define RX_UDP_PORT_ID 0x8000011
#define RX_TCP_PORT_NAME "TCPPort"
#define RX_TCP_PORT_ID 0x8000012
#define RX_TCP_LISTEN_PORT_NAME "TCPListenPort"
#define RX_TCP_LISTEN_PORT_ID 0x8000013
#define RX_TCP_SERVER_PORT_NAME "TCPServerPort"
#define RX_TCP_SERVER_PORT_ID 0x8000014
#define RX_TCP_CLIENT_PORT_NAME "TCPClientPort"
#define RX_TCP_CLIENT_PORT_ID 0x8000015
#define RX_ETHERNET_PORT_NAME "EthernetPort"
#define RX_ETHENERT_PORT_ID 0x8000016

#define RX_VT00_NAME "VT100Port"
#define RX_VT00_ID 0x8000017
#define RX_INTERACTIVE_NAME "Interactive"
#define RX_INTERACTIVE_ID 0x8000018


// system objects type stuff
#define RX_NS_SYSTEM_APP_TYPE_NAME "SystemAppType"
#define RX_NS_SYSTEM_APP_TYPE_ID 0xc000000
#define RX_NS_SYSTEM_DOM_TYPE_NAME "SystemDomainType"
#define RX_NS_SYSTEM_DOM_TYPE_ID 0xc000001


#define RX_COMMANDS_MANAGER_TYPE_NAME "CmdManagerType"
#define RX_COMMANDS_MANAGER_TYPE_ID 0xc000002
#define RX_CONSOLE_TYPE_NAME "ConsolePort"
#define RX_CONSOLE_TYPE_ID 0xc000003


#define RX_NS_SERVER_RT_TYPE_NAME "RuntimeType"
#define RX_NS_SERVER_RT_TYPE_ID 0xc000004
#define RX_IO_MANAGER_TYPE_NAME "IoManagerType"
#define RX_IO_MANAGER_TYPE_ID 0xc000005

#define RX_POOL_TYPE_NAME "PoolType"
#define RX_POOL_TYPE_ID 0xc000007
#define RX_LOG_TYPE_NAME "LogType"
#define RX_LOG_TYPE_ID 0xc000009
#define RX_NS_SYSTEM_UNASS_TYPE_NAME "UnassignedDomainType"
#define RX_NS_SYSTEM_UNASS_TYPE_ID 0xc00000a
#define RX_NS_SYSTEM_UNASS_APP_TYPE_NAME "UnassignedAppType"
#define RX_NS_SYSTEM_UNASS_APP_TYPE_ID 0xc00000b



// port classes and releated structs
#define RX_PHYSICAL_PORT_TYPE_NAME "PhysicalPort"
#define RX_PHYSICAL_PORT_TYPE_ID 0xc00000c
#define RX_TRANSPORT_PORT_TYPE_NAME "TransportPort"
#define RX_TRANSPORT_PORT_TYPE_ID 0xc00000d
#define RX_PROTOCOL_PORT_TYPE_NAME "ProtocolPort"
#define RX_PROTOCOL_PORT_TYPE_ID 0xc00000e

#define RX_TTY_PORT_TYPE_NAME "TTYPort"
#define RX_TTY_PORT_TYPE_ID 0xc000010
#define RX_UDP_PORT_TYPE_NAME "UDPPort"
#define RX_UDP_PORT_TYPE_ID 0xc000011
#define RX_TCP_PORT_TYPE_NAME "TCPPort"
#define RX_TCP_PORT_TYPE_ID 0xc000012
#define RX_TCP_LISTEN_PORT_TYPE_NAME "TCPListenPort"
#define RX_TCP_LISTEN_PORT_TYPE_ID 0xc000013
#define RX_TCP_SERVER_PORT_TYPE_NAME "TCPServerPort"
#define RX_TCP_SERVER_PORT_TYPE_ID 0xc000014
#define RX_TCP_CLIENT_PORT_TYPE_NAME "TCPClientPort"
#define RX_TCP_CLIENT_PORT_TYPE_ID 0xc000015
#define RX_ETHERNET_PORT_TYPE_NAME "EthernetPort"
#define RX_ETHENERT_PORT_TYPE_ID 0xc000016

#define RX_VT00_TYPE_NAME "VT100Port"
#define RX_VT00_TYPE_ID 0xc000017
#define RX_INTERACTIVE_TYPE_NAME "InteractivePort"
#define RX_INTERACTIVE_TYPE_ID 0xc000018


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

#define RX_CPP_DIRECORY_TYPE_NAME "directory"
#define RX_CPP_COMMAND_TYPE_NAME "command"
#define RX_CPP_PROGRAM_TYPE_NAME "program"

#define RX_CONST_VALUE_TYPE_NAME "const_value"
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

