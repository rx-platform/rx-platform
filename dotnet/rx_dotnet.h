

/****************************************************************************
*
*  dotnet\rx_dotnet.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_dotnet_h
#define rx_dotnet_h 1



// rx_commands
#include "terminal/rx_commands.h"
// rx_internal_builders
#include "sys_internal/rx_internal_builders.h"
// rx_dotnet_host
#include "dotnet/rx_dotnet_host.h"



#include "system/hosting/rx_host.h"


namespace rx_internal {

namespace dotnet {
class dotnet_object_runtime;
class dotnet_event;
class dotnet_mapper;
class dotnet_source;
class dotnet_struct;
struct dotnet_runtime_info_t
{
    string_type runtime_version;
    string_type rx_native_version;
	string_type rx_dotnet_version;
};





class dotnet_command : public terminal::commands::server_command  
{

  public:
      dotnet_command();

      ~dotnet_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);


  private:

      bool do_reload_command (const string_type& what, std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);



};






class dotnet_hosting 
{

  public:
      ~dotnet_hosting();


      static dotnet_hosting& instance ();

      rx_result initialize (hosting::rx_platform_host* host, const configuration_data_t& data);

      rx_result start (hosting::rx_platform_host* host, const configuration_data_t& data, initialize_data_type& init_data);

      void deinitialize (rx_void_callback callback);

      dotnet_runtime_info_t get_dotnet_info () const;

      rx_result do_build (configuration_data_t& config, startup_create_data_type& data);

      std::vector<std::pair<string_type, string_type> > get_loaded_libraries ();

      void reload_assembly (const string_type& what, rx_result_callback callback);

      void received_result_callback (uint32_t transId, rx_result result);

      void received_void_callback (uint32_t transId);

      void received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, rx_result_callback callback);

      void bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_object_runtime> ref);

      void unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr);

      void bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_struct> ref);

      void unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_source> ref);

      void unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_mapper> ref);

      void unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_event> ref);

      void unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose);

      void send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose);

      rx_result execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value);

      rx_result source_write (uint32_t trans_id, void* whose, typed_value_type val);

      void received_execute_done (uint32_t transId, void* inst, const string_type& val, rx_result result);

      void received_source_change (void* inst, rx_value val);

      void received_source_write_done (uint32_t trans_id, void* inst, rx_result result);


  protected:

  private:
      dotnet_hosting();



      std::unique_ptr<dotnet_host> host_;


      string_type dotnet_host_version_;


};







class dotnet_builder : public builders::rx_platform_builder  
{

  public:

      rx_result do_build (configuration_data_t& config, startup_create_data_type& data);


  protected:

  private:


};


} // namespace dotnet
} // namespace rx_internal



#endif
