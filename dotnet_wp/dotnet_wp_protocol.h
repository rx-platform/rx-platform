

/****************************************************************************
*
*  dotnet_wp\dotnet_wp_protocol.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef dotnet_wp_protocol_h
#define dotnet_wp_protocol_h 1


#include "lib/rx_io_buffers.h"




namespace rx_internal
{
namespace dotnet {

struct rx_dotnet_config_data_t
{
    string_type config_tfm;
    string_type config_framework;
    string_type config_framework_version;

    string_type requested_version;
    bool server_gc = true;
    bool concurrent_gc = true;
    uint32_t max_threads = 0;
    uint32_t min_threads = 0;
    string_type host_path;
    string_type app_path;
    string_type lib_path;
    bool launch_debugger = false;
};


} // namespace dotnet
} // namespace rx_internal

#define RX_DOTNET_PIPE_BUFFER_SIZE 0x10000 //64 KiB for pipes

#ifdef _DEBUG
#define RX_DOTNET_INITIALIZATION_TIMEOUT 300000 //5 minutes for pipe operations in debug mode, can be increased if needed
#else
#define RX_DOTNET_INITIALIZATION_TIMEOUT 30000 //30 seconds for initialization of the dotnet runtime and application
											   //, this loads all assemblies
                                               //, can be increased if needed
#endif

#define RX_DOTNET_KNOWN 0xabcd1234


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

typedef struct dotnet_wp_message_header
{
	uint32_t known;//RX_DOTNET_KNOWN
	uint32_t size;// size of the message including the header
	uint32_t type;// type of the message, e.g. RX_DOTNET_WRITE_LOG
	uint32_t trans; // transaction id, used to correlate request and response, 0 if not applicable

} dotnet_wp_message_header_t;


#pragma pack(pop)


rx_protocol_result_t pipe_header_function(uint8_t* header, size_t* size);


#ifdef __cplusplus
}
#endif

#define RX_IS_DOTNET_RESPONSE(type) (((type) & 0x8000) != 0)

#define RX_DOTNET_INITIALIZE 0x0001
/*

string_type requested_version;
bool server_gc = true;
bool concurrent_gc = true;
uint32_t max_threads = 0;
uint32_t min_threads = 0;
string_type host_path;
string_type app_path;
string_type lib_path;
bool launch_debugger = false;
*/
#define RX_DOTNET_INITIALIZE_RESP 0x8001
/*
rx_result result;
string_type hosted_version;
string_type dotnet_host_version;
*/

#define RX_DOTNET_WRITE_LOG 0x0002
/*
int type, const char* library, const char* source, uint16_t level, const char* code, const char* message
*/


#define RX_DOTNET_CREATE_TYPES 0x0003
/*
const char* library
*/

#define RX_DOTNET_CREATE_TYPES_RESPONSE 0x8003
/*
rx_result result;
*/
#define RX_DOTNET_BUILD_TYPE 0x0004
/*
int type
string_type module,
rx_node_id id,
rx_node_id parent
string_type name
string_type path
uint32_t version
uint32_t flags
string_type def
int ownType
string_type conns
string_type code_info
*/

#define RX_DOTNET_BUILD_TYPE_RESPONSE 0x8004
/*
rx_result result;
*/

#define RX_DOTNET_START_MESSAGE 0x0005
/*
void
*/


#define RX_DOTNET_BUILD_RUNTIME 0x0006
/*
int type
string_type module,
rx_node_id id,
rx_node_id parent
string_type name
string_type path
uint32_t version
uint32_t flags
string_type def
int ownType
string_type conns
string_type code_info
*/

#define RX_DOTNET_BUILD_RUNTIME_RESPONSE 0x8006
/*
rx_result result;
*/



#define RX_DOTNET_BIND_RUNTIME 0x0007
/*rx_item_type type
rx_node_id id
string_type path
 rx_node_id type_id
 void* ptr
*/

#define RX_DOTNET_UNBIND_RUNTIME 0x0008
/*rx_item_type type
rx_node_id id
string_type path
 rx_node_id type_id
 void* ptr
*/


#define RX_DOTNET_VALUE_CHANGED 0x0009
/*
size_t idx;
rx_item_type type;
rx_simple_value val;
intptr_t whose;
*/



#define RX_DOTNET_INITIAL_VALUES 0x000a
/*
string_array names;
std::vector<rx_simple_value> values;
rx_item_type type;
intptr_t whose;
*/



#define RX_DOTNET_WP_WRITE_VALUE 0x000b
/*
rx_item_type type;
intptr_t inst;
size_t idx;
rx_simple_value val;
*/


#define RX_DOTNET_WP_RESULT 0x800b
/*
rx_result result;
*/


#define RX_DOTNET_EXECUTE 0x000c
/*
intptr_t inst;
string_type method;
string_type value;
*/

#define RX_DOTNET_EXECUTE_RESULT 0x800c
/*
rx_result result;
string_type value;
*/


#define RX_DOTNET_RELOAD 0x000d
/*
const char* library
*/

#define RX_DOTNET_RELOAD_RESPONSE 0x800d
/*
rx_result result;
*/

#define RX_DOTNET_UNLOAD 0x000e
/*
const char* library
*/

#define RX_DOTNET_UNLOAD_RESPONSE 0x800e
/*
rx_result result;
*/


#define RX_DOTNET_DELETE_RUNTIME 0x000f
/*
rx_item_type type
rx_node_id id
string_type library
*/


#define RX_DOTNET_DELETE_TYPE 0x0010
/*
rx_item_type type
rx_node_id id
string_type library
*/

#define RX_DOTNET_FIRE_EVENT 0x0011
/*
intptr_t inst;
string_type value;
string_type queue;
bool state;
bool remove;
*/


#define RX_DOTNET_STOP 0x0012
/*
*/

#define RX_DOTNET_WP_VOID 0x8012
/*
*/



namespace rx_internal {

namespace dotnet {





template <typename T>
class pipe_client_job 
{

  public:
      pipe_client_job (T&& t);


  protected:

  private:


      T arg_;


};






class worker_initialized_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_result action_result;

      string_type hosted_version;

      string_type dotnet_host_version;

      static constexpr uint32_t msg_id = RX_DOTNET_INITIALIZE_RESP;
  protected:

  private:


};






class write_log_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      int32_t type;

      string_type library;

      string_type source;

      uint16_t level;

      string_type code;

      string_type message;

      static constexpr uint32_t msg_id = RX_DOTNET_WRITE_LOG;
  protected:

  private:


};






class build_types_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      string_type library;

      static constexpr uint32_t msg_id = RX_DOTNET_CREATE_TYPES;
  protected:

  private:


};






class build_types_response 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_result action_result;

      static constexpr uint32_t msg_id = RX_DOTNET_CREATE_TYPES_RESPONSE;
  protected:

  private:


};






class build_one_type_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      string_type library;

      rx_node_id id;

      rx_node_id parent;

      string_type name;

      string_type path;

      uint32_t version;

      uint32_t flags;

      string_type def;

      int ownType;

      string_type conns;

      string_type code_info;

      static constexpr uint32_t msg_id = RX_DOTNET_BUILD_TYPE;
  protected:

  private:


};






class build_one_type_response 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_result action_result;

      static constexpr uint32_t msg_id = RX_DOTNET_BUILD_TYPE_RESPONSE;
  protected:

  private:


};






class dotnet_start_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);

      static constexpr uint32_t msg_id = RX_DOTNET_START_MESSAGE;
  protected:

  private:


};






class build_runtime_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      string_type library;

      rx_node_id id;

      rx_node_id parent;

      string_type name;

      string_type path;

      uint32_t version;

      uint32_t flags;

      string_type def;

      void* instance;

      static constexpr uint32_t msg_id = RX_DOTNET_BUILD_RUNTIME;
  protected:

  private:


};






class build_runtime_response 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_result action_result;

      static constexpr uint32_t msg_id = RX_DOTNET_BUILD_RUNTIME_RESPONSE;
  protected:

  private:


};






class dotnet_binding_runtime_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      rx_node_id id;

      string_type path;

      rx_node_id type_id;

      intptr_t ptr;


  protected:

  private:


};






class dotnet_unbind_runtime_message : public dotnet_binding_runtime_message  
{

  public:

      static constexpr uint32_t msg_id = RX_DOTNET_UNBIND_RUNTIME;
  protected:

  private:


};






class dotnet_bind_runtime_message : public dotnet_binding_runtime_message  
{

  public:

      static constexpr uint32_t msg_id = RX_DOTNET_BIND_RUNTIME;
  protected:

  private:


};






class dotnet_value_changed_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      size_t idx;

      rx_simple_value val;

      intptr_t whose;

      static constexpr uint32_t msg_id = RX_DOTNET_VALUE_CHANGED;
  protected:

  private:


};






class dotnet_initial_values_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      string_array names;

      std::vector<rx_simple_value> values;

      rx_item_type type;

      intptr_t whose;

      static constexpr uint32_t msg_id = RX_DOTNET_INITIAL_VALUES;
  protected:

  private:


};






class wp_write_value_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      intptr_t inst;

      size_t idx;

      rx_simple_value val;

      static constexpr uint32_t msg_id = RX_DOTNET_WP_WRITE_VALUE;
  protected:

  private:


};






class wp_result_response 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_result action_result;

      static constexpr uint32_t msg_id = RX_DOTNET_WP_RESULT;
  protected:

  private:


};






class dotnet_execute_method_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      intptr_t inst;

      string_type method;

      string_type value;

      static constexpr uint32_t msg_id = RX_DOTNET_EXECUTE;
  protected:

  private:


};






class dotnet_execute_method_response 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_result action_result;

      string_type result_value;

      static constexpr uint32_t msg_id = RX_DOTNET_EXECUTE_RESULT;
  protected:

  private:


};






class delete_one_type_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      string_type library;

      rx_node_id id;

      static constexpr uint32_t msg_id = RX_DOTNET_DELETE_TYPE;
  protected:

  private:


};






class reload_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      string_type library;

      static constexpr uint32_t msg_id = RX_DOTNET_RELOAD;
  protected:

  private:


};






class unload_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      string_type library;

      static constexpr uint32_t msg_id = RX_DOTNET_UNLOAD;
  protected:

  private:


};






class delete_one_runtime_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      rx_item_type type;

      string_type library;

      rx_node_id id;

      static constexpr uint32_t msg_id = RX_DOTNET_DELETE_RUNTIME;
  protected:

  private:


};






class dotnet_fire_event_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);


      intptr_t inst;

      string_type value;

      string_type queue;

      bool state;

      bool remove;

      static constexpr uint32_t msg_id = RX_DOTNET_FIRE_EVENT;
  protected:

  private:


};






class dotnet_stop_message 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);

      static constexpr uint32_t msg_id = RX_DOTNET_STOP;

  protected:

  private:


};






class wp_void_response 
{

  public:

      rx_result serialize (io::rx_io_buffer& buffer) const;

      rx_result deserialize (io::rx_const_io_buffer& buffer);

      static constexpr uint32_t msg_id = RX_DOTNET_WP_VOID;

  protected:

  private:


};


// Parameterized Class rx_internal::dotnet::pipe_client_job 

template <typename T>
pipe_client_job<T>::pipe_client_job (T&& t)
      : arg_(t)
{
}



} // namespace dotnet
} // namespace rx_internal



#endif
