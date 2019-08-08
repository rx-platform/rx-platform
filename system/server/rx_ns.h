

/****************************************************************************
*
*  system\server\rx_ns.h
*
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


#ifndef rx_ns_h
#define rx_ns_h 1


#include "lib/rx_lock.h"

// rx_storage
#include "system/storage_base/rx_storage.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"

namespace rx_platform {
namespace ns {
class rx_platform_item;

} // namespace ns
} // namespace rx_platform



#include "system/runtime/rx_runtime_helpers.h"

/////////////////////////////////////////////////////////////
// logging macros for host library
#define NAMESPACE_LOG_INFO(src,lvl,msg) RX_LOG_INFO("NS",src,lvl,msg)
#define NAMESPACE_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("NS",src,lvl,msg)
#define NAMESPACE_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("NS",src,lvl,msg)
#define NAMESPACE_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("NS",src,lvl,msg)
#define NAMESPACE_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("NS",src,lvl,msg)
#define NAMESPACE_LOG_TRACE(src,lvl,msg) RX_TRACE("NS",src,lvl,msg)

using namespace rx;
using namespace rx::values;
using namespace rx_platform;


namespace rx_platform {

enum rx_object_command_t
{
	rx_turn_off = 0,
	rx_turn_on,
	rx_set_blocked,
	rx_reset_blocked,
	rx_set_test,
	rx_reset_test
};
namespace meta
{
class meta_data;
}
typedef typename meta::meta_data meta_data_t;

bool rx_is_valid_name_character(char ch);
bool rx_is_valid_namespace_name(const string_type& name);

namespace ns
{
class rx_platform_directory;
class rx_platform_item;
}
namespace prog
{
class server_command_base;
}
typedef rx::pointers::reference<prog::server_command_base> server_command_base_ptr;
typedef rx::pointers::reference<ns::rx_platform_item> platform_item_ptr;
typedef rx::pointers::reference<ns::rx_platform_directory> rx_directory_ptr;
typedef std::vector<platform_item_ptr> platform_items_type;
typedef std::vector<rx_directory_ptr> platform_directories_type;


namespace ns {


struct namespace_data_t
{
	namespace_data_t()
	{
		build_system_from_code = false;
	}
	string_type system_storage_reference;
	string_type user_storage_reference;
	string_type test_storage_reference;
	bool build_system_from_code;
};

void fill_attributes_string(namespace_item_attributes attr, string_type& str);







class rx_platform_directory : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_directory);
	typedef std::map<string_type, rx_platform_directory::smart_ptr>  sub_directories_type;
	typedef std::map<string_type, platform_item_ptr> sub_items_type;
	typedef std::unordered_set<string_type> reserved_type;

  public:
      rx_platform_directory (const string_type& name, namespace_item_attributes attrs, rx_storage_ptr storage = rx_storage_ptr::null_ptr);

      ~rx_platform_directory();


      virtual void get_content (platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern) const;

      void structure_lock ();

      void structure_unlock ();

      rx_directory_ptr get_parent () const;

      virtual rx_directory_ptr get_sub_directory (const string_type& path) const;

      string_type get_name () const;

      void fill_path (string_type& path) const;

      void set_parent (rx_directory_ptr parent);

      namespace_item_attributes get_attributes () const;

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual rx_item_type get_type_id () const;

      virtual platform_item_ptr get_sub_item (const string_type& path) const;

      void structure_lock () const;

      void structure_unlock () const;

      virtual void get_value (rx_value& value);

      void fill_dir_code_info (std::ostream& info);

      virtual void get_value (const string_type& name, rx_value& value);

      rx_result add_sub_directory (rx_directory_ptr who);

      rx_result add_item (platform_item_ptr who);

      rx_result delete_item (platform_item_ptr who);

      virtual rx_result delete_item (const string_type& path);

      rx_result_with<rx_directory_ptr> add_sub_directory (const string_type& path);

      rx_result delete_sub_directory (const string_type& path);

      bool empty () const;

      rx_result reserve_name (const string_type& name, string_type& path);

      rx_result cancel_reserve (const string_type& name);

      virtual meta_data_t meta_info () const;

      rx_result_with<rx_storage_ptr> resolve_storage () const;

	  template<class TImpl>
	  rx_result add_item(TImpl who);
  protected:

  private:


      rx_reference<rx_platform_directory> parent_;

      sub_directories_type sub_directories_;

      sub_items_type sub_items_;

      meta::meta_data meta_;

      rx_reference<storage_base::rx_platform_storage> storage_;


      rx::locks::slim_lock structure_lock_;

      reserved_type reserved_;


};







class rx_platform_item : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_item);	

  public:
      rx_platform_item();

      ~rx_platform_item();


      virtual void code_info_to_string (string_type& info);

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual rx_item_type get_type_id () const = 0;

      void lock ();

      void unlock ();

      virtual values::rx_value get_value () const = 0;

      rx_directory_ptr get_parent () const;

      void set_parent (rx_directory_ptr parent);

      virtual rx_result generate_json (std::ostream& def, std::ostream& err) const = 0;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream) = 0;

      virtual string_type get_name () const = 0;

      virtual size_t get_size () const = 0;

      rx_result save () const;

      virtual const meta_data_t& meta_info () const = 0;

      string_type callculate_path () const;

      bool is_object () const;

      bool is_type () const;

      virtual rx_result read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx) const = 0;

      virtual rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx) = 0;

      virtual rx_result do_command (rx_object_command_t command_type) = 0;

      virtual rx_result browse (const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items) = 0;

      virtual rx_result connect_items (const string_array& paths, std::function<void(std::vector<rx_result_with<runtime_handle_t> >)> callback, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx) = 0;


  protected:

  private:


      rx_reference<rx_platform_directory> parent_;


      locks::lockable item_lock_;


};






class rx_names_cache 
{
	  typedef std::unordered_map<string_type, platform_item_ptr> name_items_hash_type;

  public:
      rx_names_cache();


      platform_item_ptr get_cached_item (const string_type& name) const;

      rx_result insert_cached_item (const string_type& name, platform_item_ptr item);

      static bool should_cache (const platform_item_ptr& item);


  protected:

  private:


      name_items_hash_type name_items_hash_;


};


} // namespace ns
} // namespace rx_platform



#endif
