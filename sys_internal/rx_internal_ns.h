

/****************************************************************************
*
*  sys_internal\rx_internal_ns.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_internal_ns_h
#define rx_internal_ns_h 1



// rx_host
#include "system/hosting/rx_host.h"
// rx_platform_item
#include "system/server/rx_platform_item.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_internal_objects
#include "sys_internal/rx_internal_objects.h"

#include "terminal/rx_terminal_style.h"
#include "system/server/rx_server.h"
#include "system/serialization/rx_serialization_defs.h"
#include "system/meta/rx_types.h"
#include "system/storage_base/rx_storage.h"
using namespace rx_platform::ns;


namespace rx_internal {

namespace internal_ns {






class platform_root : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(platform_root);

	DECLARE_CODE_INFO("rx", 0,6,0, "\
root server directory:\r\n\
contains root server folders\
");

  public:
      platform_root();

      ~platform_root();


      static rx_namespace_item get_cached_item (const string_type& name);

      static rx_result insert_cached_item (const string_type& name, const rx_namespace_item& item);


  protected:

  private:


      static rx_platform::ns::rx_names_cache cache_;


};






class user_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(user_directory);
	DECLARE_CODE_INFO("rx", 0,5,0, "\
server directory:\r\n\
used to create user defined folders...\
");

  public:
      user_directory (const string_type& name);

      ~user_directory();


  protected:

  private:


};






class unassigned_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(unassigned_directory);
	DECLARE_CODE_INFO("rx", 0,5,0, "\
storing unassigned domains and applications\r\n\
All objects here are with unassigned state and hawing a bad quality\
");

  public:
      unassigned_directory();

      ~unassigned_directory();


  protected:

  private:


};






class world_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(world_directory);
	DECLARE_CODE_INFO("rx", 0,5,0, "\
user directory:\r\n\
used to create user defined folders...\
");

  public:
      world_directory();

      ~world_directory();


  protected:

  private:


};






template <class TImpl>
class rx_item_implementation : public rx_platform::ns::rx_platform_item  
{

  public:
      rx_item_implementation (TImpl impl);


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const meta_data_t& meta_info () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      rx_result read_value (const string_type& path, rx_value& value) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      rx_result do_command (rx_object_command_t command_type);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor);

	  ~rx_item_implementation() = default;
  protected:

  private:


      TImpl impl_;


};






template <class TImpl>
class rx_meta_item_implementation : public rx_platform::ns::rx_platform_item  
{

  public:
      rx_meta_item_implementation (TImpl impl);


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const meta_data_t& meta_info () const;

      rx_result read_value (const string_type& path, rx_value& value) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      rx_result do_command (rx_object_command_t command_type);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor);


  protected:

  private:


      TImpl impl_;


};






class internal_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(internal_directory);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
server directory:\r\n\
used to create system defined folders...\
");

  public:
      internal_directory (const string_type& name);

      ~internal_directory();


  protected:

  private:


};






template <class TImpl>
class rx_other_implementation : public rx_platform::ns::rx_platform_item  
{

  public:
      rx_other_implementation (TImpl impl);


      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      string_type get_name () const;

      void fill_code_info (std::ostream& info, const string_type& name);

      rx_node_id get_node_id () const;

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const meta_data_t& meta_info () const;

      rx_result read_value (const string_type& path, rx_value& value) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx);

      rx_result do_command (rx_object_command_t command_type);

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

      std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor);

      rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx);

      string_type get_definition_as_json () const;

      rx_platform_item::smart_ptr clone () const;

      rx_thread_handle_t get_executer () const;

      rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor);


  protected:

  private:


      TImpl impl_;


};






class system_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(system_directory);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
system directory:\r\n\
used to hold system folders...\
");

  public:
      system_directory();

      ~system_directory();


  protected:

  private:


};






class host_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(host_directory);
	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
host directory:\r\n\
used to hold host folders...\
");

  public:
      host_directory();

      ~host_directory();


  protected:

  private:


};






class plugin_directory : public rx_platform::ns::rx_platform_directory  
{
	DECLARE_REFERENCE_PTR(plugin_directory);
	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
plugin directory:\r\n\
used to hold specific plugin folders...\
");

  public:
      plugin_directory (rx_plugin_ptr plugin);

      ~plugin_directory();


  protected:

  private:


};


} // namespace internal_ns
} // namespace rx_internal



#endif
