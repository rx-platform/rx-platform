

/****************************************************************************
*
*  dotnet\rx_dotnet_host.h
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


#ifndef rx_dotnet_host_h
#define rx_dotnet_host_h 1


#include "dotnet_api.h"
#include "rx_dotnet_os.h"

// rx_dotnet_runtime
#include "dotnet/rx_dotnet_runtime.h"



namespace rx_internal {

namespace dotnet {





class dotnet_configuration 
{

  public:
      dotnet_configuration (const configuration_data_t& data, const rx_dotnet_config_data_t& init_config);

      ~dotnet_configuration();


      string_type get_runtime_config () const;


  protected:

  private:


      dotnet_data_t config_;

      rx_dotnet_config_data_t runtime_config_;


};






class dotnet_host 
{
public:
    dotnet_host(const dotnet_host&) = delete;
	dotnet_host& operator=(const dotnet_host&) = delete;
	dotnet_host(dotnet_host&&) noexcept = delete;
	dotnet_host& operator=(dotnet_host&&) noexcept = delete;    

	typedef std::map<uint32_t, rx_result_callback> result_callbacks_type;
    typedef std::map<uint32_t, rx_platform::callback::rx_any_callback<> > void_callbacks_type;

	typedef std::map<void*, rx_reference<dotnet_object_runtime> > runtime_objects_type;
    typedef std::map<void*, rx_reference<dotnet_source> > runtime_sources_type;
    typedef std::map<void*, rx_reference<dotnet_mapper> > runtime_mappers_type;
    typedef std::map<void*, rx_reference<dotnet_event> > runtime_events_type;
    typedef std::map<void*, rx_reference<dotnet_struct> > runtime_structs_type;

  public:
      dotnet_host (const configuration_data_t& data, const rx_dotnet_config_data_t& init_config);

      ~dotnet_host();


      rx_result initialize (hosting::rx_platform_host* host, const string_array& host_files, const string_array& app_files, const string_array& lib_files, string_type& version);

      rx_result start ();

      void deinitialize (rx_void_callback callback);

      rx_result do_build (configuration_data_t& config, startup_create_data_type& data);

      rx_result do_reload (const string_type& lib);

      void do_unload (const string_type& lib, rx_result_callback callback);

      void received_result_callback (uint32_t transId, rx_result result);

      void received_void_callback (uint32_t transId);

      void received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, rx_result_callback callback);

      void bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_object_runtime> ref);

      void unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr);

      void runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose);

      void send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose);

      rx_result execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value);

      void received_execute_done (uint32_t transId, void* inst, const string_type& val, rx_result result);

      void bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_struct> ref);

      void unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_source> ref);

      void unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_mapper> ref);

      void unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_event> ref);

      void unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      rx_result source_write (uint32_t trans_id, void* whose, typed_value_type val);

      void received_source_change (void* inst, rx_value val);

      void received_source_write_done (uint32_t trans_id, void* inst, rx_result result);


  protected:

  private:

      rx_result bind_functions ();


      const deinit_hosting_entry_point_fn& get_deinit_fn () const;



      dotnet_configuration configuration_;

      runtime_objects_type runtime_objects_;

      runtime_sources_type runtime_sources_;

      runtime_mappers_type runtime_mappers_;

      runtime_events_type runtime_events_;

      runtime_structs_type runtime_structs_;


      init_hosting_entry_point_fn init_fn_;

      start_hosting_entry_point_fn start_fn_;

      deinit_hosting_entry_point_fn deinit_fn_;

      build_library_hosting_entry_point_fn build_types_fn_;

      bind_object_entry_point_fn bind_object_fn_;

      bind_object_entry_point_fn unbind_object_fn_;

      deinit_library_hosting_entry_point_fn deinit_library_fn_;

      result_callbacks_type result_callbacks_;

      void_callbacks_type void_callbacks_;

      locks::slim_lock callbacks_lock_;

      value_changed_entry_point_fn values_callback_;

      initial_values_entry_point_fn initial_values_;

      execute_method_entry_point_fn execute_method_;

      source_write_entry_point_fn source_write_;

      mapped_changed_entry_point_fn mapped_changed_;


};


} // namespace dotnet
} // namespace rx_internal



#endif
