#pragma once


#define RX_LINE_END "\n"
#define RX_LINE_END_CH '\n'

// directories stuff
#define RX_NS_WORLD_NAME "world"
#define RX_NS_STORAGE_NAME "storage"
#define RX_NS_SYS_NAME "sys"
#define RX_NS_TEST_NAME "test"
#define RX_NS_UNASSIGNED_NAME "unassigned"
#define RX_NS_BIN_NAME "bin"
#define RX_NS_OBJ_NAME "runtime"
#define RX_NS_SYSTEM_OBJ_NAME "system"
#define RX_NS_PORT_OBJ_NAME "ports"
#define RX_NS_PEER_OBJ_NAME "peers"
#define RX_NS_RELATIONS_NAME "relations"
#define RX_NS_CLASSES_NAME "types"
#define RX_NS_BASE_CLASSES_NAME "base"
#define RX_NS_SYSTEM_CLASSES_NAME "system"
#define RX_NS_PORT_CLASSES_NAME "ports"
#define RX_NS_OPC_CLASSES_NAME "opc"
#define RX_NS_MQTT_CLASSES_NAME "mqtt"
#define RX_NS_SUPPORT_CLASSES_NAME "support"
#define RX_NS_SIMULATION_CLASSES_NAME "simulation"
#define RX_NS_TERMINAL_NAME "term"
#define RX_NS_PLUGINS_NAME "plugins"
#define RX_NS_HOST_NAME "host"
#define RX_NS_HTTP_CLASSES_NAME "http"
#define RX_NS_XML_CLASSES_NAME "xml"
#define RX_NS_JSON_CLASSES_NAME "json"

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
#define RX_PARENT_MAPPER_TYPE_NAME "StructuralMapper"
#define RX_PARENT_MAPPER_TYPE_ID 0xc000045
#define RX_VARIABLE_MAPPER_TYPE_NAME "VariableMapper"
#define RX_VARIABLE_MAPPER_TYPE_ID 0xc0000d3
#define RX_METHOD_MAPPER_TYPE_NAME "MethodMapper"
#define RX_METHOD_MAPPER_TYPE_ID 0xc0000d4
#define RX_EVENT_MAPPER_TYPE_NAME "EventMapper"
#define RX_EVENT_MAPPER_TYPE_ID 0xc0000da

#define RX_CLASS_FILTER_BASE_NAME "FilterBase"
#define RX_CLASS_FILTER_BASE_ID 0x0000000a


#define RX_CLASS_METHOD_BASE_NAME "AbstractMethod"
#define RX_CLASS_METHOD_BASE_ID 0x00000013
#define RX_CLASS_PROGRAM_BASE_NAME "ProgramBase"
#define RX_CLASS_PROGRAM_BASE_ID 0x00000014
#define RX_CLASS_DATA_BASE_NAME "DataTypeBase"
#define RX_CLASS_DATA_BASE_ID 0x00000015
#define RX_CLASS_DISPLAY_BASE_NAME "DisplayBase"
#define RX_CLASS_DISPLAY_BASE_ID 0x00000016

// console stuff
#define RX_CLASS_CONSOLE_IN_NAME "ConsoleInData"
#define RX_CLASS_CONSOLE_IN_ID 0x00000019
#define RX_CLASS_CONSOLE_OUT_NAME "ConsoleOUTData"
#define RX_CLASS_CONSOLE_OUT_ID 0x0000001a
#define RX_CLASS_CONSOLE_COMMAND_BASE_NAME "ConsoleCommandBase"
#define RX_CLASS_CONSOLE_COMMAND_BASE_ID 0x0000001b


// objects stuff
#define RX_NS_SYSTEM_APP_NAME "SystemApp"
#define RX_NS_SYSTEM_APP_ID 0x0000000b
#define RX_NS_SYSTEM_DOM_NAME "SystemDomain"
#define RX_NS_SYSTEM_DOM_ID 0x0000000c
#define RX_NS_OBJECT_DATA_NAME "ObjectData"
#define RX_NS_OBJECT_DATA_ID 0x0000000d
#define RX_NS_OBJECT_CHANGED_NAME "ObjectChanged"
#define RX_NS_OBJECT_CHANGED_ID 0x0000001c


#define RX_HOST_OBJ_ID 0xc00006f
#define RX_HOST_APP_ID 0xc000070
#define RX_HOST_DOMAIN_ID 0xc000071

#define RX_HOST_APP_NAME "HostApp"
#define RX_HOST_DOMAIN_NAME "HostDomain"
#define RX_HOST_OBJECT_NAME "Host"


#define RX_NS_WORLD_APP_NAME "WorldApp"
#define RX_NS_WORLD_APP_ID 0xc0000bb


#define RX_NS_RELATION_BASE_NAME "RelationBase"
#define RX_NS_RELATION_BASE_ID 0x0000000e

#define RX_NS_PORT_STACK_NAME "PortStack"
#define RX_NS_PORT_STACK_ID 0x0000000f

#define RX_NS_PORT_REF_NAME "PortReference"
#define RX_NS_PORT_REF_ID 0x00000017

#define RX_NS_APPLICATION_RELATION_NAME "Application"
#define RX_NS_APPLICATION_RELATION_ID 0x00000010

#define RX_NS_PORT_APPLICATION_RELATION_NAME "PortApp"
#define RX_NS_PORT_APPLICATION_RELATION_ID 0x00000011

#define RX_NS_DOMAIN_RELATION_NAME "Domain"
#define RX_NS_DOMAIN_RELATION_ID 0x00000012

#define NS_RX_COMMANDS_MANAGER_NAME "CmdManager"
#define RX_COMMANDS_MANAGER_ID 0x8000001

#define RX_NS_SERVER_RT_NAME "Runtime"
#define RX_NS_SERVER_RT_ID 0x8000002
#define RX_IO_MANAGER_NAME "IoManager"
#define RX_IO_MANAGER_ID 0x8000003


#define IO_POOL_NAME "IoPool"
#define IO_POOL_ID 0x8000004
#define UNASSIGNED_POOL_NAME "UnassignedPool"
#define UNASSIGNED_POOL_ID 0x8000005
#define WORKER_POOL_NAME "WorkerPool"
#define WORKER_POOL_ID 0x8000006
#define IO_LOG_NAME "Log"
#define IO_LOG_ID 0x8000007

#define RX_NS_SYSTEM_UNASS_NAME "UnassignedDomain"
#define RX_NS_SYSTEM_UNASS_ID 0x8000009
#define RX_NS_SYSTEM_UNASS_APP_NAME "UnassignedApp"
#define RX_NS_SYSTEM_UNASS_APP_ID 0x800000a
#define RX_NS_SYSTEM_UNASS_APP_PATH RX_DIR_DELIMETER_STR RX_NS_UNASSIGNED_NAME RX_DIR_DELIMETER_STR RX_NS_SYSTEM_UNASS_APP_NAME

#define META_POOL_NAME "MetaPool"
#define META_POOL_ID 0x8000019
#define RX_NS_SYSTEM_NAME "System"
#define RX_NS_SYSTEM_ID 0x800001a


#define RX_NS_MEMORY_NAME "Memory"
#define RX_NS_MEMORY_ID 0xc0000ed

// system objects type stuff
#define RX_NS_SYSTEM_APP_TYPE_NAME "SysAppType"
#define RX_NS_SYSTEM_APP_TYPE_ID 0xc000000

#define RX_NS_WORLD_APP_TYPE_NAME "WorldAppType"
#define RX_NS_WORLD_APP_TYPE_ID 0xc0000ba

#define RX_NS_SYSTEM_TYPE_NAME "SystemType"
#define RX_NS_SYSTEM_TYPE_ID 0xc000054
#define RX_NS_MEMORY_TYPE_NAME "MemoryType"
#define RX_NS_MEMORY_TYPE_ID 0xc0000ee
#define RX_NS_SYSTEM_INFO_TYPE_NAME "SystemInfo"
#define RX_NS_SYSTEM_INFO_TYPE_ID 0xc000055
#define RX_NS_SYSTEM_DOM_TYPE_NAME "SysDomainType"
#define RX_NS_SYSTEM_DOM_TYPE_ID 0xc000001

#define RX_NS_BUCKET_STATUS_TYPE_NAME "BucketStatus"
#define RX_NS_BUCKET_STATUS_TYPE_ID 0xc0000ef
#define RX_NS_MEMORY_STATUS_TYPE_NAME "MemoryStatus"
#define RX_NS_MEMORY_STATUS_TYPE_ID 0xc0000f0


#define RX_USER_APP_TYPE_NAME "UserApplication"
#define RX_USER_APP_TYPE_ID 0xc00003a
#define RX_USER_DOMAIN_TYPE_NAME "UserDomain"
#define RX_USER_DOMAIN_TYPE_ID 0xc00003b
#define RX_USER_OBJECT_TYPE_NAME "UserObject"
#define RX_USER_OBJECT_TYPE_ID 0xc00003c

#define RX_HOST_APP_TYPE_NAME "HostApplicationType"
#define RX_HOST_APP_TYPE_ID 0xc00004b
#define RX_HOST_DOMAIN_TYPE_NAME "HostDomainType"
#define RX_HOST_DOMAIN_TYPE_ID 0xc00004c
#define RX_NS_HOST_INFO_NAME "HostInfo"
#define RX_NS_HOST_INFO_ID 0xc000056
#define RX_NS_HOST_TYPE_NAME "HostType"
#define RX_NS_HOST_TYPE_ID 0xc000057


#define RX_INTERNAL_APP_TYPE_NAME "InternalAppType"
#define RX_INTERNAL_APP_TYPE_ID 0xc00003d
#define RX_INTERNAL_DOMAIN_TYPE_NAME "InternalDomainType"
#define RX_INTERNAL_DOMAIN_TYPE_ID 0xc00003e
#define RX_INTERNAL_OBJECT_TYPE_NAME "InternalObjectType"
#define RX_INTERNAL_OBJECT_TYPE_ID 0xc00003f

#define RX_COMMANDS_MANAGER_TYPE_NAME "CmdManagerType"
#define RX_COMMANDS_MANAGER_TYPE_ID 0xc000002
#define RX_CONSOLE_TYPE_NAME "ConsolePort"
#define RX_CONSOLE_TYPE_ID 0xc000003


#define RX_NS_SERVER_RT_TYPE_NAME "RuntimeType"
#define RX_NS_SERVER_RT_TYPE_ID 0xc000004
#define RX_IO_MANAGER_TYPE_NAME "IoManagerType"
#define RX_IO_MANAGER_TYPE_ID 0xc000005

#define RX_POOL_TYPE_NAME "IoPoolType"
#define RX_POOL_TYPE_ID 0xc000007
#define RX_LOG_TYPE_NAME "LogType"
#define RX_LOG_TYPE_ID 0xc000009
#define RX_NS_SYSTEM_UNASS_TYPE_NAME "UnassignedDomainType"
#define RX_NS_SYSTEM_UNASS_TYPE_ID 0xc00000a
#define RX_NS_SYSTEM_UNASS_APP_TYPE_NAME "UnassignedAppType"
#define RX_NS_SYSTEM_UNASS_APP_TYPE_ID 0xc00000b



// port classes and related structs
#define RX_EXTERNAL_PORT_TYPE_NAME "ExternalPort"
#define RX_EXTERNAL_PORT_TYPE_ID 0xc00000c
//#define RX_PHYSICAL_SINGLE_PORT_TYPE_NAME "PhysicalSingle"
//#define RX_PHYSICAL_SINGLE_PORT_TYPE_ID 0xc000036
//#define RX_PHYSICAL_MULTI_PORT_TYPE_NAME "PhysicalMultiple"
//#define RX_PHYSICAL_MULTI_PORT_TYPE_ID 0xc000037

#define RX_TRANSPORT_PORT_TYPE_NAME "TransportPort"
#define RX_TRANSPORT_PORT_TYPE_ID 0xc00000d
#define RX_ROUTED_TRANSPORT_PORT_TYPE_NAME "RoutedTransportPort"
#define RX_ROUTED_TRANSPORT_PORT_TYPE_ID 0xc000038
#define RX_CONN_TRANSPORT_PORT_TYPE_NAME "ConnectionTransportPort"
#define RX_CONN_TRANSPORT_PORT_TYPE_ID 0xc0000a4

#define RX_APPLICATION_PORT_TYPE_NAME "ApplicationPort"
#define RX_APPLICATION_PORT_TYPE_ID 0xc00000e

#define RX_SERIAL_PORT_TYPE_NAME "SerialPort"
#define RX_SERIAL_PORT_TYPE_ID 0xc000010
#define RX_UDP_PORT_TYPE_NAME "UDPPort"
#define RX_UDP_PORT_TYPE_ID 0xc000011
#define RX_TCP_SERVER_PORT_TYPE_NAME "TCPServerPort"
#define RX_TCP_SERVER_PORT_TYPE_ID 0xc000014
#define RX_TCP_CLIENT_PORT_TYPE_NAME "TCPClientPort"
#define RX_TCP_CLIENT_PORT_TYPE_ID 0xc000015
#define RX_ETHERNET_PORT_TYPE_NAME "EthernetPort"
#define RX_ETHERNET_PORT_TYPE_ID 0xc000016
#define RX_FILE_PORT_TYPE_NAME "FilePort"
#define RX_FILE_PORT_TYPE_ID 0xc0000fc
#define RX_JSON_PORT_TYPE_NAME "JSONPort"
#define RX_JSON_PORT_TYPE_ID 0xc0000fe
#define RX_XML_PORT_TYPE_NAME "XMLPort"
#define RX_XML_PORT_TYPE_ID 0xc0000ff


#define RX_JSON_SOURCE_TYPE_NAME "JSONSource"
#define RX_JSON_SOURCE_TYPE_ID 0xc000102
#define RX_XML_SOURCE_TYPE_NAME "XMLSource"
#define RX_XML_SOURCE_TYPE_ID 0xc000103


#define RX_IP4_ROUTER_TYPE_NAME "IP4RouterPort"
#define RX_IP4_ROUTER_TYPE_ID 0xc00005a
#define RX_BYTE_ROUTER_TYPE_NAME "ByteRouterPort"
#define RX_BYTE_ROUTER_TYPE_ID 0xc00005b
#define RX_WORD_ROUTER_TYPE_NAME "WordRouterPort"
#define RX_WORD_ROUTER_TYPE_ID 0xc0000ab
#define RX_MAC_ROUTER_TYPE_NAME "MACRouterPort"
#define RX_MAC_ROUTER_TYPE_ID 0xc0000ac
#define RX_STRING_ROUTER_TYPE_NAME "StringRouterPort"
#define RX_STRING_ROUTER_TYPE_ID 0xc0000c9

#define RX_TRANS_LIMITER_TYPE_NAME "TransactionLimiterPort"
#define RX_TRANS_LIMITER_TYPE_ID 0xc00005c

#define RX_SIZE_LIMITER_TYPE_NAME "SizeLimiterPort"
#define RX_SIZE_LIMITER_TYPE_ID 0xc0000e0

#define RX_INITIATOR_TO_LISTENER_PORT_TYPE_NAME "Initiator2ListenerPort"
#define RX_INITIATOR_TO_LISTENER_PORT_TYPE_ID 0xc00006c
#define RX_LISTENER_TO_INITIATOR_PORT_TYPE_NAME "Listener2InitiatorPort"
#define RX_LISTENER_TO_INITIATOR_PORT_TYPE_ID 0xc00006d

#define RX_STXETX_TYPE_NAME "STXETXPort"
#define RX_STXETX_TYPE_ID 0xc00006a

#define RX_VT00_TYPE_NAME "VT100Port"
#define RX_VT00_TYPE_ID 0xc000017

#define RX_TELNET_TYPE_NAME "TelnetPort"
#define RX_TELNET_TYPE_ID 0xc000018


#define RX_PORT_STATUS_TYPE_NAME "PortStatusData"
#define RX_PORT_STATUS_TYPE_ID 0xc000019
#define RX_PHY_PORT_STATUS_TYPE_NAME "ExternalPortStatusData"
#define RX_PHY_PORT_STATUS_TYPE_ID 0xc000020
#define RX_CLIENT_PORT_STATUS_TYPE_NAME "ClientPortStatusData"
#define RX_CLIENT_PORT_STATUS_TYPE_ID 0xc00005e
#define RX_MASTER_PORT_STATUS_TYPE_NAME "MasterPortStatusData"
#define RX_MASTER_PORT_STATUS_TYPE_ID 0xc00005f

#define RX_IP_BIND_TYPE_NAME "IPBindData"
#define RX_IP_BIND_TYPE_ID 0xc000021

#define RX_MAC_BIND_TYPE_NAME "MACBindData"
#define RX_MAC_BIND_TYPE_ID 0xc0000ad


#define RX_RX_OPC_ADAPT_TYPE_NAME "RxOPCProtocolAdapter"
#define RX_RX_OPC_ADAPT_TYPE_ID 0xc000110

#define RX_RX_JSON_TYPE_NAME "RxJsonProtocol"
#define RX_RX_JSON_TYPE_ID 0xc000022

#define RX_RX_JSON_CLIENT_TYPE_NAME "RxJsonClientProtocol"
#define RX_RX_JSON_CLIENT_TYPE_ID 0xc000089
#define RX_RX_JSON_RELATION_TYPE_NAME "RxJsonProtocolClient"
#define RX_RX_JSON_RELATION_TYPE_ID 0xc0000b1

#define RX_TIMEOUTS_TYPE_NAME "TimeoutsData"
#define RX_TIMEOUTS_TYPE_ID 0xc000024
#define RX_CLIENT_TIMEOUTS_TYPE_NAME "ClientTimeoutsData"
#define RX_CLIENT_TIMEOUTS_TYPE_ID 0xc000025
#define RX_SERVER_TIMEOUTS_TYPE_NAME "ServerTimeoutsData"
#define RX_SERVER_TIMEOUTS_TYPE_ID 0xc000060
#define RX_MASTER_TIMEOUTS_TYPE_NAME "MasterTimeoutsData"
#define RX_MASTER_TIMEOUTS_TYPE_ID 0xc00005d


#define RX_PORT_OPTIONS_TYPE_NAME "PortOptions"
#define RX_PORT_OPTIONS_TYPE_ID 0xc000061
#define RX_TCP_PORT_OPTIONS_TYPE_NAME "TcpPortOptions"
#define RX_TCP_PORT_OPTIONS_TYPE_ID 0xc000104
#define RX_ROUTER_PORT_OPTIONS_TYPE_NAME "RouterPortOptions"
#define RX_ROUTER_PORT_OPTIONS_TYPE_ID 0xc000062
#define RX_BRIDGE_PORT_OPTIONS_TYPE_NAME "BridgePortOptions"
#define RX_BRIDGE_PORT_OPTIONS_TYPE_ID 0xc000063
#define RX_LIMITER_PORT_OPTIONS_TYPE_NAME "LimiterPortOptions"
#define RX_LIMITER_PORT_OPTIONS_TYPE_ID 0xc000064
#define RX_SIZE_LIMITER_PORT_OPTIONS_TYPE_NAME "SizeLimiterPortOptions"
#define RX_SIZE_LIMITER_PORT_OPTIONS_TYPE_ID 0xc0000e1

#define RX_STXETX_PORT_OPTIONS_TYPE_NAME "STXETXPortOptions"
#define RX_STXETX_PORT_OPTIONS_TYPE_ID 0xc00006e


#define RX_SERIAL_PORT_OPTIONS_TYPE_NAME "SerialPortOptions"
#define RX_SERIAL_PORT_OPTIONS_TYPE_ID 0xc00006b


#define RX_ETHERNET_PORT_OPTIONS_TYPE_NAME "EthernetPortOptions"
#define RX_ETHERNET_PORT_OPTIONS_TYPE_ID 0xc0000aa

#define RX_FILE_PORT_OPTIONS_TYPE_NAME "FilePortOptions"
#define RX_FILE_PORT_OPTIONS_TYPE_ID 0xc0000fd

#define RX_JSON_PORT_OPTIONS_TYPE_NAME "JSONPortOptions"
#define RX_JSON_PORT_OPTIONS_TYPE_ID 0xc000100

#define RX_XML_PORT_OPTIONS_TYPE_NAME "XMLPortOptions"
#define RX_XML_PORT_OPTIONS_TYPE_ID 0xc000101

// runtime support structures
#define RX_POOL_DATA_TYPE_NAME "PoolData"
#define RX_POOL_DATA_TYPE_ID 0xc000026
#define RX_RT_DATA_TYPE_NAME "RuntimeData"
#define RX_RT_DATA_TYPE_ID 0xc000027


#define RX_UNASSIGNED_POOL_TYPE_NAME "UnassignedThreadType"
#define RX_UNASSIGNED_POOL_TYPE_ID 0xc000028
#define RX_META_POOL_TYPE_NAME "MetaThreadType"
#define RX_META_POOL_TYPE_ID 0xc000029

#define RX_THREAD_DATA_TYPE_NAME "ThreadData"
#define RX_THREAD_DATA_TYPE_ID 0xc000030
#define RX_PHYSICAL_THREAD_TYPE_NAME "PhysicalThread"
#define RX_PHYSICAL_THREAD_TYPE_ID 0xc000031



#define RX_OPCUA_TRANSPORT_PORT_TYPE_NAME "OpcBinTransport"
#define RX_OPCUA_TRANSPORT_PORT_TYPE_ID 0xc000032
#define RX_OPCUA_TRANSPORT_OPTIONS_TYPE_NAME "OpcBinTransportOptions"
#define RX_OPCUA_TRANSPORT_OPTIONS_TYPE_ID 0xc00009a

#define RX_OPCUA_CLIENT_TRANSPORT_PORT_TYPE_NAME "OpcClientBinTransport"
#define RX_OPCUA_CLIENT_TRANSPORT_PORT_TYPE_ID 0xc000099
#define RX_OPCUA_CLIENT_TRANSPORT_OPTIONS_TYPE_NAME "OpcClientBinTransportOptions"
#define RX_OPCUA_CLIENT_TRANSPORT_OPTIONS_TYPE_ID 0xc00009b


#define RX_OPCUA_ENDPOINT_DATA_NAME "OpcEndpointBindData"
#define RX_OPCUA_ENDPOINT_DATA_ID 0xc0000a3

#define RX_OPCUA_SEC_BASE_PORT_TYPE_NAME "OpcSecBase"
#define RX_OPCUA_SEC_BASE_PORT_TYPE_ID 0xc000080
#define RX_OPCUA_SEC_CHANNEL_OPTIONS_TYPE_NAME "OpcSecChannelOptions"
#define RX_OPCUA_SEC_CHANNEL_OPTIONS_TYPE_ID 0xc000098
#define RX_OPCUA_SEC_NONE_PORT_TYPE_NAME "OpcBinSecNone"
#define RX_OPCUA_SEC_NONE_PORT_TYPE_ID 0xc00007b
#define RX_OPCUA_SEC_SIGN_PORT_TYPE_NAME "OpcBinSecSign"
#define RX_OPCUA_SEC_SIGN_PORT_TYPE_ID 0xc00007d
#define RX_OPCUA_SEC_SIGNENCR_PORT_TYPE_NAME "OpcBinSecSignEncr"
#define RX_OPCUA_SEC_SIGNENCR_PORT_TYPE_ID 0xc00007e
#define RX_OPCUA_SEC_SIGNSIGNENCR_PORT_TYPE_NAME "OpcBinSecSignSignEncr"
#define RX_OPCUA_SEC_SIGNSIGNENCR_PORT_TYPE_ID 0xc00007f


#define RX_OPCUA_SEC_BASE_CLIENT_PORT_TYPE_NAME "OpcClientSecBase"
#define RX_OPCUA_SEC_BASE_CLIENT_PORT_TYPE_ID 0xc00008e
#define RX_OPCUA_SEC_CHANNEL_CLIENT_OPTIONS_TYPE_NAME "OpcClientSecChannelOptions"
#define RX_OPCUA_SEC_CHANNEL_CLIENT_OPTIONS_TYPE_ID 0xc000094
#define RX_OPCUA_SEC_NONE_CLIENT_PORT_TYPE_NAME "OpcBinClientSecNone"
#define RX_OPCUA_SEC_NONE_CLIENT_PORT_TYPE_ID 0xc00008f
#define RX_OPCUA_SEC_SIGN_CLIENT_PORT_TYPE_NAME "OpcBinClientSecSign"
#define RX_OPCUA_SEC_SIGN_CLIENT_PORT_TYPE_ID 0xc000090
#define RX_OPCUA_SEC_SIGNENCR_CLIENT_PORT_TYPE_NAME "OpcBinClientSecSignEncr"
#define RX_OPCUA_SEC_SIGNENCR_CLIENT_PORT_TYPE_ID 0xc000091
#define RX_OPCUA_SEC_SIGNSIGNENCR_CLIENT_PORT_TYPE_NAME "OpcBinClientSecSignSignEncr"
#define RX_OPCUA_SEC_SIGNSIGNENCR_CLIENT_PORT_TYPE_ID 0xc000092


#define RX_OPCUA_SERVER_BASE_PORT_TYPE_NAME "OpcServerBase"
#define RX_OPCUA_SERVER_BASE_PORT_TYPE_ID 0xc000083
#define RX_OPCUA_SERVER_PORT_OPTIONS_TYPE_NAME "OpcServerOptions"
#define RX_OPCUA_SERVER_PORT_OPTIONS_TYPE_ID 0xc000093

#define RX_OPCUA_SIMPLE_SERVER_PORT_TYPE_NAME "OpcSimpleServer"
#define RX_OPCUA_SIMPLE_SERVER_PORT_TYPE_ID 0xc00009c
#define RX_OPCUA_SIMPLE_SERVER_RELATION_TYPE_NAME "OpcSimpleServerConnection"
#define RX_OPCUA_SIMPLE_SERVER_RELATION_TYPE_ID 0xc00009f

#define RX_OPCUA_SIMPLE_BINARY_SERVER_PORT_TYPE_NAME "OpcSimpleBinServer"
#define RX_OPCUA_SIMPLE_BINARY_SERVER_PORT_TYPE_ID 0xc000082



#define RX_OPCUA_CLIENT_BASE_PORT_TYPE_NAME "OpcClientBase"
#define RX_OPCUA_CLIENT_BASE_PORT_TYPE_ID 0xc000095
#define RX_OPCUA_CLIENT_PORT_OPTIONS_TYPE_NAME "OpcClientOptions"
#define RX_OPCUA_CLIENT_PORT_OPTIONS_TYPE_ID 0xc000096

#define RX_OPCUA_SIMPLE_CLIENT_PORT_TYPE_NAME "OpcSimpleClient"
#define RX_OPCUA_SIMPLE_CLIENT_PORT_TYPE_ID 0xc00009d
#define RX_OPCUA_SIMPLE_CLIENT_RELATION_TYPE_NAME "OpcSimpleClientConnection"
#define RX_OPCUA_SIMPLE_CLIENT_RELATION_TYPE_ID 0xc00009e

#define RX_OPCUA_SIMPLE_BINARY_CLIENT_PORT_TYPE_NAME "OpcSimpleBinClient"
#define RX_OPCUA_SIMPLE_BINARY_CLIENT_PORT_TYPE_ID 0xc000097


#define RX_MQTT_ENDPOINT_DATA_NAME "MqttEndpointBindData"
#define RX_MQTT_ENDPOINT_DATA_ID 0xc0000c4



#define RX_MQTT_CLIENT_BASE_PORT_TYPE_NAME "MqttClientBase"
#define RX_MQTT_CLIENT_BASE_PORT_TYPE_ID 0xc0000bc
#define RX_MQTT_CLIENT_PORT_OPTIONS_TYPE_NAME "MqttClientOptions"
#define RX_MQTT_CLIENT_PORT_OPTIONS_TYPE_ID 0xc0000bd
#define RX_MQTT_CLIENT_PORT_STATUS_TYPE_NAME "MqttClientStatus"
#define RX_MQTT_CLIENT_PORT_STATUS_TYPE_ID 0xc0000d0

#define RX_MQTT_SIMPLE_CLIENT_PORT_TYPE_NAME "MqttSimpleClient"
#define RX_MQTT_SIMPLE_CLIENT_PORT_TYPE_ID 0xc0000be
#define RX_MQTT_SIMPLE_CLIENT_RELATION_TYPE_NAME "MqttSimpleClientConnection"
#define RX_MQTT_SIMPLE_CLIENT_RELATION_TYPE_ID 0xc0000bf


#define RX_MQTT_SERVER_BASE_PORT_TYPE_NAME "MqttServerBase"
#define RX_MQTT_SERVER_BASE_PORT_TYPE_ID 0xc0000c0
#define RX_MQTT_SERVER_PORT_OPTIONS_TYPE_NAME "MqttServerOptions"
#define RX_MQTT_SERVER_PORT_OPTIONS_TYPE_ID 0xc0000c1
#define RX_MQTT_SERVER_PORT_STATUS_TYPE_NAME "MqttServerStatus"
#define RX_MQTT_SERVER_PORT_STATUS_TYPE_ID 0xc0000d1

#define RX_MQTT_SIMPLE_SERVER_PORT_TYPE_NAME "MqttSimpleServer"
#define RX_MQTT_SIMPLE_SERVER_PORT_TYPE_ID 0xc0000c2
#define RX_MQTT_SIMPLE_SERVER_RELATION_TYPE_NAME "MqttSimpleServerConnection"
#define RX_MQTT_SIMPLE_SERVER_RELATION_TYPE_ID 0xc0000c3


#define RX_MQTT_MAPPER_BASE_TYPE_NAME "MqttMapperBase"
#define RX_MQTT_MAPPER_BASE_TYPE_ID 0xc0000ca

#define RX_MQTT_SIMPLE_MAPPER_TYPE_NAME "MqttSimpleMapper"
#define RX_MQTT_SIMPLE_MAPPER_TYPE_ID 0xc0000cb
#define RX_MQTT_SIMPLE_BROKER_MAPPER_TYPE_NAME "MqttSimpleBrokerMapper"
#define RX_MQTT_SIMPLE_BROKER_MAPPER_TYPE_ID 0xc0000ce


#define RX_MQTT_EVENT_MAPPER_BASE_TYPE_NAME "MqttEventMapperBase"
#define RX_MQTT_EVENT_MAPPER_BASE_TYPE_ID 0xc0000f4

#define RX_MQTT_SIMPLE_EVENT_MAPPER_TYPE_NAME "MqttSimpleEventMapper"
#define RX_MQTT_SIMPLE_EVENT_MAPPER_TYPE_ID 0xc0000f5
#define RX_MQTT_SIMPLE_BROKER_EVENT_MAPPER_TYPE_NAME "MqttSimpleBrokerEventMapper"
#define RX_MQTT_SIMPLE_BROKER_EVENT_MAPPER_TYPE_ID 0xc0000f6


#define RX_MQTT_SOURCE_BASE_TYPE_NAME "MqttSourceBase"
#define RX_MQTT_SOURCE_BASE_TYPE_ID 0xc0000cc

#define RX_MQTT_SIMPLE_SOURCE_TYPE_NAME "MqttSimpleSource"
#define RX_MQTT_SIMPLE_SOURCE_TYPE_ID 0xc0000cd
#define RX_MQTT_SIMPLE_BROKER_SOURCE_TYPE_NAME "MqttSimpleBrokerSource"
#define RX_MQTT_SIMPLE_BROKER_SOURCE_TYPE_ID 0xc0000cf


#define RX_MQTT_EVENT_BASE_TYPE_NAME "MqttEventBase"
#define RX_MQTT_EVENT_BASE_TYPE_ID 0xc0000f7

#define RX_MQTT_SIMPLE_EVENT_TYPE_NAME "MqttSimpleEvent"
#define RX_MQTT_SIMPLE_EVENT_TYPE_ID 0xc0000f8
#define RX_MQTT_SIMPLE_BROKER_EVENT_TYPE_NAME "MqttSimpleBrokerEvent"
#define RX_MQTT_SIMPLE_BROKER_EVENT_TYPE_ID 0xc0000f9


#define RX_NS_SYSTEM_MQTT_TCP_NAME "TcpMqttClient"
#define RX_NS_SYSTEM_MQTT_TCP_ID 0xc0000f1
#define RX_NS_SYSTEM_MQTT_NAME "MqttClient"
#define RX_NS_SYSTEM_MQTT_ID 0xc0000f2

#define TLS_PORT_TYPE_NAME "TlsPortType"
#define TLS_PORT_TYPE_ID 0xc0000b6
#define TLS_CLIENT_PORT_TYPE_NAME "TlsClientPortType"
#define TLS_CLIENT_PORT_TYPE_ID 0xc000107

#define TLS_PORT_OPTIONS_TYPE_NAME "TlsPortOptions"
#define TLS_PORT_OPTIONS_TYPE_ID 0xc0000b7

#define TLS_PORT_STATUS_TYPE_NAME "TlsPortStatus"
#define TLS_PORT_STATUS_TYPE_ID 0xc0000b8

#define TLS_PORT_TIMEOUTS_TYPE_NAME "TlsPortTimeouts"
#define TLS_PORT_TIMEOUTS_TYPE_ID 0xc0000b9


#define RX_EXTERN_MAPPER_TYPE_NAME "ExternMapper"
#define RX_EXTERN_MAPPER_TYPE_ID 0xc000033
#define RX_EXTERN_PARENT_MAPPER_TYPE_NAME "ExternStructuralMapper"
#define RX_EXTERN_PARENT_MAPPER_TYPE_ID 0xc00004d
#define RX_EXTERN_METHOD_MAPPER_TYPE_NAME "ExternMethodMapper"
#define RX_EXTERN_METHOD_MAPPER_TYPE_ID 0xc0000d5
#define RX_EXTERN_EVENT_MAPPER_TYPE_NAME "ExternEventMapper"
#define RX_EXTERN_EVENT_MAPPER_TYPE_ID 0xc0000db
//#define RX_SYSTEM_MAPPER_TYPE_NAME "SystemMapper"
//#define RX_SYSTEM_MAPPER_TYPE_ID 0xc000046
#define RX_EXTERN_SOURCE_TYPE_NAME "ExternSource"
#define RX_EXTERN_SOURCE_TYPE_ID 0xc000034
//#define RX_USER_SOURCE_TYPE_NAME "UserSource"
//#define RX_USER_SOURCE_TYPE_ID 0xc00004a
#define RX_PARENT_SOURCE_TYPE_NAME "StructuralSource"
#define RX_PARENT_SOURCE_TYPE_ID 0xc000047
#define RX_VARIABLE_SOURCE_TYPE_NAME "VariableSource"
#define RX_VARIABLE_SOURCE_TYPE_ID 0xc0000d9
#define RX_EXTERN_PARENT_SOURCE_TYPE_NAME "ExternStructuralSource"
#define RX_EXTERN_PARENT_SOURCE_TYPE_ID 0xc00004e
//#define RX_SYSTEM_SOURCE_TYPE_NAME "SystemSource"
//#define RX_SYSTEM_SOURCE_TYPE_ID 0xc000048
#define RX_REGISTER_SOURCE_TYPE_NAME "RegisterSource"
#define RX_REGISTER_SOURCE_TYPE_ID 0xc000035
//#define RX_REGISTER_VARIABLE_TYPE_NAME "RegisterVariable"
//#define RX_REGISTER_VARIABLE_TYPE_ID 0xc000069
#define RX_PLATFORM_SOURCE_TYPE_NAME "PlatformSource"
#define RX_PLATFORM_SOURCE_TYPE_ID 0xc000049

#define RX_SIMPLE_VARIABLE_TYPE_NAME "SimpleVariable"
#define RX_SIMPLE_VARIABLE_TYPE_ID 0xc0000e2

#define RX_COMPLEX_INPUT_VARIABLE_TYPE_NAME "ComplexInputVariable"
#define RX_COMPLEX_INPUT_VARIABLE_TYPE_ID 0xc0000e3
#define RX_COMPLEX_OUTPUT_VARIABLE_TYPE_NAME "ComplexOutputVariable"
#define RX_COMPLEX_OUTPUT_VARIABLE_TYPE_ID 0xc0000e4
#define RX_COMPLEX_IO_VARIABLE_TYPE_NAME "ComplexIOVariable"
#define RX_COMPLEX_IO_VARIABLE_TYPE_ID 0xc0000e5
#define RX_BRIDGE_VARIABLE_TYPE_NAME "BridgeVariable"
#define RX_BRIDGE_VARIABLE_TYPE_ID 0xc0000e6
#define RX_MUX_VARIABLE_TYPE_NAME "MultiplexerVariable"
#define RX_MUX_VARIABLE_TYPE_ID 0xc0000e7


#define RX_OPCUA_MAPPER_BASE_TYPE_NAME "OpcMapperBase"
#define RX_OPCUA_MAPPER_BASE_TYPE_ID 0xc0000a1
#define RX_OPCUA_SIMPLE_MAPPER_TYPE_NAME "OpcSimpleMapper"
#define RX_OPCUA_SIMPLE_MAPPER_TYPE_ID 0xc0000a0

#define RX_OPCUA_METHOD_MAPPER_BASE_TYPE_NAME "OpcMethodMapperBase"
#define RX_OPCUA_METHOD_MAPPER_BASE_TYPE_ID 0xc0000d6
#define RX_OPCUA_SIMPLE_METHOD_MAPPER_TYPE_NAME "OpcSimpleMethodMapper"
#define RX_OPCUA_SIMPLE_METHOD_MAPPER_TYPE_ID 0xc0000d2
 

#define RX_OPCUA_EVENT_MAPPER_BASE_TYPE_NAME "OpcEventMapperBase"
#define RX_OPCUA_EVENT_MAPPER_BASE_TYPE_ID 0xc0000dc
#define RX_OPCUA_SIMPLE_EVENT_MAPPER_TYPE_NAME "OpcSimpleEventMapper"
#define RX_OPCUA_SIMPLE_EVENT_MAPPER_TYPE_ID 0xc0000dd

#define RX_OPCUA_SOURCE_BASE_TYPE_NAME "OpcSourceBase"
#define RX_OPCUA_SOURCE_BASE_TYPE_ID 0xc0000a5
#define RX_OPCUA_SIMPLE_SOURCE_TYPE_NAME "OpcSimpleSource"
#define RX_OPCUA_SIMPLE_SOURCE_TYPE_ID 0xc0000a6

#define RX_OPCUA_METHOD_BASE_TYPE_NAME "OpcMethodBase"
#define RX_OPCUA_METHOD_BASE_TYPE_ID 0xc0000d7
#define RX_OPCUA_SIMPLE_METHOD_TYPE_NAME "OpcSimpleMethod"
#define RX_OPCUA_SIMPLE_METHOD_TYPE_ID 0xc0000d8

#define RX_OPCUA_EVENT_BASE_TYPE_NAME "OpcEventBase"
#define RX_OPCUA_EVENT_BASE_TYPE_ID 0xc0000de
#define RX_OPCUA_SIMPLE_EVENT_TYPE_NAME "OpcSimpleEvent"
#define RX_OPCUA_SIMPLE_EVENT_TYPE_ID 0xc0000df

#define RX_LINEAR_SCALING_FILTER_TYPE_NAME "LinearScaling"
#define RX_LINEAR_SCALING_FILTER_TYPE_ID 0xc000039
#define RX_LIMIT_FILTER_TYPE_NAME "LimitFilter"
#define RX_LIMIT_FILTER_TYPE_ID 0xc000065
#define RX_CUTOFF_FILTER_TYPE_NAME "CutoffScaling"
#define RX_CUTOFF_FILTER_TYPE_ID 0xc000066
#define RX_LO_CUTOFF_FILTER_TYPE_NAME "LowCutoffScaling"
#define RX_LO_CUTOFF_FILTER_TYPE_ID 0xc000067
#define RX_HI_CUTOFF_FILTER_TYPE_NAME "HighCutoffScaling"
#define RX_HI_CUTOFF_FILTER_TYPE_ID 0xc000068
#define RX_QUALITY_FILTER_TYPE_NAME "QualityFilter"
#define RX_QUALITY_FILTER_TYPE_ID 0xc00007c

#define RX_LATCH_FILTER_TYPE_NAME "LatchFilter"
#define RX_LATCH_FILTER_TYPE_ID 0xc0000a7

#define RX_HEX2DEC_FILTER_TYPE_NAME "Hex2DecFilter"
#define RX_HEX2DEC_FILTER_TYPE_ID 0xc0000a8
#define RX_ASCII_FILTER_TYPE_NAME "ASCIIFilter"
#define RX_ASCII_FILTER_TYPE_ID 0xc0000a2


#define RX_CALC_FILTER_TYPE_NAME "CalcFilter"
#define RX_CALC_FILTER_TYPE_ID 0xc0000b2


#define RX_ROUND_FILTER_TYPE_NAME "RoundFilter"
#define RX_ROUND_FILTER_TYPE_ID 0xc000108


#define RX_CUMULATIVE_SPEED_FILTER_TYPE_NAME "CumulativeSpeedFilter"
#define RX_CUMULATIVE_SPEED_FILTER_TYPE_ID 0xc0000b5

#define RX_SIMULATION_SOURCE_TYPE_NAME "SimulationSource"
#define RX_SIMULATION_SOURCE_TYPE_ID 0xc000040
#define RX_RAMP_SIMULATION_SOURCE_TYPE_NAME "RampSource"
#define RX_RAMP_SIMULATION_SOURCE_TYPE_ID 0xc000041
#define RX_SINE_SIMULATION_SOURCE_TYPE_NAME "SineSource"
#define RX_SINE_SIMULATION_SOURCE_TYPE_ID 0xc000041


#define RX_NS_SYSTEM_TCP_NAME "TcpServer"
#define RX_NS_SYSTEM_TCP_ID 0xc000042
#define RX_NS_SYSTEM_OPCUABIN_NAME "RxOpcUaTransport"
#define RX_NS_SYSTEM_OPCUABIN_ID 0xc000043
#define RX_NS_SYSTEM_OPCUABIN_SEC_NAME "RxOpcUaSecChannel"
#define RX_NS_SYSTEM_OPCUABIN_SEC_ID 0xc000081
#define RX_NS_SYSTEM_RXOPC_ADAPT_NAME "RxOpcAdapter"
#define RX_NS_SYSTEM_RXOPC_ADAPT_ID 0xc000111
#define RX_NS_SYSTEM_RXJSON_NAME "JSON"
#define RX_NS_SYSTEM_RXJSON_ID 0xc000044


#define RX_NS_SYSTEM_WS_TCP_NAME "WsTcpServer"
#define RX_NS_SYSTEM_WS_TCP_ID 0xc00010a
#define RX_NS_SYSTEM_RXWS_NAME "RxWs"
#define RX_NS_SYSTEM_RXWS_ID 0xc00010c
#define RX_NS_SYSTEM_WS_RXJSON_NAME "WsJSON"
#define RX_NS_SYSTEM_WS_RXJSON_ID 0xc00010b


#define RX_NS_SYSTEM_TCP_CLIENT_NAME "TcpClientServer"
#define RX_NS_SYSTEM_TCP_CLIENT_ID 0xc00008a
#define RX_NS_SYSTEM_OPCUABIN_CLIENT_NAME "RxOpcUaClientTransport"
#define RX_NS_SYSTEM_OPCUABIN_CLIENT_ID 0xc00008b
#define RX_NS_SYSTEM_OPCUABIN_CLIENT_SEC_NAME "RxOpcUaClientSecChannel"
#define RX_NS_SYSTEM_OPCUABIN_CLIENT_SEC_ID 0xc00008c
#define RX_NS_SYSTEM_RXJSON_CLIENT_NAME "JSONClient"
#define RX_NS_SYSTEM_RXJSON_CLIENT_ID 0xc00008d


#define RX_NS_SYSTEM_OPCUA_TCP_NAME "TcpOpcServer"
#define RX_NS_SYSTEM_OPCUA_TCP_ID 0xc000085
#define RX_NS_SYSTEM_OPCUA_NAME "OpcTransport"
#define RX_NS_SYSTEM_OPCUA_ID 0xc000086
#define RX_NS_SYSTEM_OPCUA_SEC_NAME "OpcSecChannel"
#define RX_NS_SYSTEM_OPCUA_SEC_ID 0xc000087
#define RX_NS_SYSTEM_OPCUA_SERVER_NAME "OpcServer"
#define RX_NS_SYSTEM_OPCUA_SERVER_ID 0xc000088


#define RX_NS_CHANGED_DATA_NAME "ItemChangeDetails"
#define RX_NS_CHANGED_DATA_ID 0xc00004f
#define RX_NS_CHANGED_DATA_EVENT_NAME "ItemChangeEvent"
#define RX_NS_CHANGED_DATA_EVENT_ID 0xc000050


#define RX_HTTP_HOST_NAME_OPTIONS_TYPE_NAME "HttpHostNamePortOptions"
#define RX_HTTP_HOST_NAME_OPTIONS_TYPE_ID 0xc0000eb

#define RX_NS_HTTP_HOST_NAME_TYPE_NAME "HttpHostNamePort"
#define RX_NS_HTTP_HOST_NAME_TYPE_ID 0xc0000ea

#define RX_HTTP_PATH_OPTIONS_TYPE_NAME "HttpAddressingPortOptions"
#define RX_HTTP_PATH_OPTIONS_TYPE_ID 0xc0000e9

#define RX_NS_HTTP_PATH_TYPE_NAME "HttpAddressingPort"
#define RX_NS_HTTP_PATH_TYPE_ID 0xc0000e8

#define RX_NS_HTTP_TYPE_NAME "HttpProtocol"
#define RX_NS_HTTP_TYPE_ID 0xc000051

#define RX_NS_HTTP_TCP_NAME "TcpHttp"
#define RX_NS_HTTP_TCP_ID 0xc000052
#define RX_NS_HTTP_TRANSP_NAME "HttpTransport"
#define RX_NS_HTTP_TRANSP_ID 0xc0000ec
#define RX_NS_HTTP_NAME "Http"
#define RX_NS_HTTP_ID 0xc000053


#define RX_NS_WS_TYPE_NAME "WSProtocol"
#define RX_NS_WS_TYPE_ID 0xc0000b4


#define RX_PATH_BIND_TYPE_NAME "PathBindData"
#define RX_PATH_BIND_TYPE_ID 0xc00010d



#define RX_TCP_HTTP_PORT_TYPE_ID 0xc000058
#define RX_TCP_HTTP_PORT_TYPE_NAME "SystemHttpPort"
#define RX_TCP_RX_PORT_TYPE_ID 0xc000059
#define RX_TCP_RX_PORT_TYPE_NAME "SystemRxPort"
#define RX_RX_OPC_JSON_TYPE_ID 0xc00010f
#define RX_RX_OPC_JSON_TYPE_NAME "SystemOPCJsonProtocol"
#define RX_TCP_RX_WS_PORT_TYPE_ID 0xc000109
#define RX_TCP_RX_WS_PORT_TYPE_NAME "SystemRxWSPort"
#define RX_TCP_RX_WS_JSON_PORT_TYPE_ID 0xc00010e
#define RX_TCP_RX_WS_JSON_PORT_TYPE_NAME "SystemWsJsonPort"
#define RX_TCP_OPCUA_PORT_TYPE_ID 0xc000084
#define RX_TCP_OPCUA_PORT_TYPE_NAME "SystemOpcPort"
#define RX_TCP_MQTT_PORT_TYPE_ID 0xc0000f3
#define RX_TCP_MQTT_PORT_TYPE_NAME "SystemMqttPort"


#define RX_DISPLAY_RESOURCE_TYPE_ID 0xc000074
#define RX_DISPLAY_RESOURCE_TYPE_NAME "DisplayResource"

#define RX_DISPLAY_STATUS_TYPE_ID 0xc000076
#define RX_DISPLAY_STATUS_TYPE_NAME "DisplayStatus"

#define RX_HTTP_DISPLAY_TYPE_ID 0xc000075
#define RX_HTTP_DISPLAY_TYPE_NAME "HttpDisplay"

#define RX_HTTP_DISPLAY_RESOURCE_TYPE_ID 0xc000073
#define RX_HTTP_DISPLAY_RESOURCE_TYPE_NAME "HttpDisplayResource"

#define RX_HTTP_DISPLAY_STATUS_TYPE_ID 0xc000077
#define RX_HTTP_DISPLAY_STATUS_TYPE_NAME "HttpDisplayStatus"


#define RX_STATIC_HTTP_DISPLAY_RESOURCE_TYPE_ID 0xc000078
#define RX_STATIC_HTTP_DISPLAY_RESOURCE_TYPE_NAME "StaticHttpDisplayResource"

#define RX_STATIC_HTTP_DISPLAY_TYPE_ID 0xc000072
#define RX_STATIC_HTTP_DISPLAY_TYPE_NAME "StaticHttpDisplay"


#define RX_MQTT_HTTP_DISPLAY_TYPE_ID 0xc000105
#define RX_MQTT_HTTP_DISPLAY_TYPE_NAME "MqttHttpDisplay"

#define RX_MQTT_HTTP_DISPLAY_OPTIONS_TYPE_ID 0xc000106
#define RX_MQTT_HTTP_DISPLAY_OPTIONS_TYPE_NAME "MqttDisplayOptions"


#define RX_STANDARD_HTTP_DISPLAY_TYPE_ID 0xc000079
#define RX_STANDARD_HTTP_DISPLAY_TYPE_NAME "StandardHttpDisplay"

#define RX_SIMPLE_HTTP_DISPLAY_TYPE_ID 0xc00007a
#define RX_SIMPLE_HTTP_DISPLAY_TYPE_NAME "SimpleHttpDisplay"


#define RX_MAIN_HTTP_DISPLAY_TYPE_ID 0xc0000b3
#define RX_MAIN_HTTP_DISPLAY_TYPE_NAME "MainHttpDisplay"

// upython stuff
#define RX_UPYTHON_METHOD_TYPE_NAME "uPyMethod"
#define RX_UPYTHON_METHOD_TYPE_ID 0xc0000a9
#define RX_UPYTHON_MODULE_METHOD_TYPE_NAME "uPyModuleMethod"
#define RX_UPYTHON_MODULE_METHOD_TYPE_ID 0xc0000fa
#define RX_UPYTHON_SCRIPT_METHOD_TYPE_NAME "uPyScriptMethod"
#define RX_UPYTHON_SCRIPT_METHOD_TYPE_ID 0xc0000fb

// discovery stuff
#define RX_PEER_CONNECTION_TYPE_NAME "PeerConnection"
#define RX_PEER_CONNECTION_TYPE_ID 0xc0000ae


#define RX_PEER_ENDPOINT_TYPE_NAME "PeerEndpoint"
#define RX_PEER_ENDPOINT_TYPE_ID 0xc0000af
#define RX_PEER_STATUS_TYPE_NAME "PeerStatus"
#define RX_PEER_STATUS_TYPE_ID 0xc0000b0

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
// last is 0xc000111!!!! 
//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////


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

//Storage related stuff
#define RX_FILE_STORAGE_FOLDER "storage"

