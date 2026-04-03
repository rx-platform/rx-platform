

/****************************************************************************
*
*  dotnet_wp\dotnet_wp_host.h
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


#ifndef dotnet_wp_host_h
#define dotnet_wp_host_h 1


#include "dotnet_wp_api.h"
#include "rx_dotnet_os.h"




namespace rx_internal {

namespace dotnet {





class dotnet_configuration 
{

  public:
      dotnet_configuration (const rx_dotnet_config_data_t& init_config);

      ~dotnet_configuration();


      string_type get_runtime_config () const;

      bool should_break () const;


  protected:

  private:


      rx_dotnet_config_data_t runtime_config_;


};






class dotnet_wp_host 
{
    typedef std::map<uint32_t, rxwp_result_callback> result_callbacks_type;
    typedef std::map<uint32_t, rxwp_void_callback> void_callbacks_type;

  public:
      dotnet_wp_host (const rx_dotnet_config_data_t& init_config);

      ~dotnet_wp_host();


      rx_result initialize (const string_array& host_files, const string_array& app_files, const string_array& lib_files, string_type& version);

      rx_result start ();

      void deinitialize (uint32_t trans_id, rxwp_void_callback callback);

      rx_result do_build ();

      rx_result do_reload (uint32_t trans_id, const string_type& lib);

      void do_unload (uint32_t trans_id, const string_type& lib, rxwp_result_callback callback);

      void received_result_callback (uint32_t transId, rx_result result);

      void received_void_callback (uint32_t transId);

      void received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, rxwp_result_callback callback);

      void bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr);

      void runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose);

      void send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose);

      rx_result execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value);

      void bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      rx_result source_write (uint32_t trans_id, void* whose, typed_value_type val);

      void received_source_change (void* inst, rx_value val);

      void received_source_write_done (uint32_t trans_id, void* inst, rx_result result);

      void received_fire_event (void* inst, string_type val, string_type queue, bool state, bool remove);


  protected:

  private:

      rx_result bind_functions ();


      const deinit_hosting_entry_point_fn& get_deinit_fn () const;



      dotnet_configuration configuration_;


      init_hosting_entry_point_fn init_fn_;

      start_hosting_entry_point_fn start_fn_;

      deinit_hosting_entry_point_fn deinit_fn_;

      build_library_hosting_entry_point_fn build_types_fn_;

      bind_object_entry_point_fn bind_object_fn_;

      bind_object_entry_point_fn unbind_object_fn_;

      deinit_library_hosting_entry_point_fn deinit_library_fn_;

      value_changed_entry_point_fn values_callback_;

      initial_values_entry_point_fn initial_values_;

      execute_method_entry_point_fn execute_method_;

      source_write_entry_point_fn source_write_;

      mapped_changed_entry_point_fn mapped_changed_;

      result_callbacks_type result_callbacks_;

      void_callbacks_type void_callbacks_;

      locks::slim_lock callbacks_lock_;

      string_type hosted_version_;


};


} // namespace dotnet
} // namespace rx_internal



#endif
