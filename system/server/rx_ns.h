

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

// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_values
#include "lib/rx_values.h"
// rx_storage
#include "system/storage_base/rx_storage.h"


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

namespace sys_internal
{
namespace internal_ns
{
class namespace_algorithms;
}// internal_ns
}// sys_internal


namespace rx_platform {

enum class rx_object_command_t
{
	rx_turn_off = 0,
	rx_turn_on,
	rx_set_blocked,
	rx_reset_blocked,
	rx_set_test,
	rx_reset_test
};

void rx_split_path(const string_type& full_path, string_type& directory_path, string_type& item_path);

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
}
namespace prog
{
class server_command_base;
}
typedef rx::pointers::reference<prog::server_command_base> server_command_base_ptr;
typedef rx::pointers::reference<ns::rx_platform_directory> rx_directory_ptr;
typedef std::vector<rx_directory_ptr> platform_directories_type;


namespace ns {


struct namespace_data_t
{
	string_type system_storage_reference;
	string_type user_storage_reference;
	string_type test_storage_reference;
};

void fill_attributes_string(namespace_item_attributes attr, string_type& str);







class rx_namespace_item 
{

  public:
      rx_namespace_item();

      rx_namespace_item (const platform_item_ptr& who);


      operator bool () const;

      string_type callculate_path (rx_directory_ptr dir) const;

      bool is_object () const;

      bool is_type () const;


      const meta::meta_data& get_meta () const
      {
        return meta_;
      }


      const rx::values::rx_value& get_value () const
      {
        return value_;
      }



      rx_item_type get_type () const
      {
        return type_;
      }


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }


	  rx_namespace_item(const rx_namespace_item& right) = default;
	  rx_namespace_item(rx_namespace_item&& right) = default;
	  rx_namespace_item& operator=(const rx_namespace_item & right) = default;
	  rx_namespace_item& operator=(rx_namespace_item && right) = default;
  protected:

  private:


      meta::meta_data meta_;

      rx::values::rx_value value_;


      rx_item_type type_;

      rx_thread_handle_t executer_;


};

typedef std::vector<rx_namespace_item> platform_items_type;





class rx_platform_directory : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_directory);
	typedef std::map<string_type, rx_platform_directory::smart_ptr>  sub_directories_type;
	typedef std::map<string_type, rx_namespace_item> sub_items_type;
	typedef std::unordered_set<string_type> reserved_type;

  public:
      rx_platform_directory (const string_type& name, namespace_item_attributes attrs, rx_storage_ptr storage = rx_storage_ptr::null_ptr);

      ~rx_platform_directory();


      virtual void get_content (platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern) const;

      rx_directory_ptr get_parent () const;

      virtual rx_directory_ptr get_sub_directory (const string_type& path) const;

      string_type get_name () const;

      void fill_path (string_type& path) const;

      namespace_item_attributes get_attributes () const;

      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual rx_item_type get_type_id () const;

      virtual void get_value (rx_value& value);

      void fill_dir_code_info (std::ostream& info);

      virtual void get_value (const string_type& name, rx_value& value);

      rx_result add_sub_directory (rx_directory_ptr who);

      rx_result add_item (platform_item_ptr&& who);

      rx_result delete_item (platform_item_ptr who);

      virtual rx_result delete_item (const string_type& path);

      rx_result_with<rx_directory_ptr> add_sub_directory (const string_type& path);

      rx_result delete_sub_directory (const string_type& path);

      bool empty () const;

      rx_result reserve_name (const string_type& name, string_type& path);

      rx_result cancel_reserve (const string_type& name);

      virtual meta_data_t meta_info () const;

      rx_result_with<rx_storage_ptr> resolve_storage () const;

      rx_namespace_item get_sub_item (const string_type& path);

      rx_result add_item (const rx_namespace_item& what);

	  template<class TImpl>
	  rx_result add_item(TImpl who);
  protected:

  private:

      void structure_lock ();

      void structure_unlock ();

      void set_parent (rx_directory_ptr parent);

      void structure_lock () const;

      void structure_unlock () const;



      rx_reference<rx_platform_directory> parent_;

      sub_directories_type sub_directories_;

      sub_items_type sub_items_;

      meta::meta_data meta_;

      rx_reference<storage_base::rx_platform_storage> storage_;


      rx::locks::slim_lock structure_lock_;

      reserved_type reserved_;


    friend class sys_internal::internal_ns::namespace_algorithms;
};







class rx_names_cache 
{
	  typedef std::unordered_map<string_type, rx_namespace_item> name_items_hash_type;

  public:
      rx_names_cache();


      rx_namespace_item get_cached_item (const string_type& name) const;

      rx_result insert_cached_item (const string_type& name, const rx_namespace_item& item);

      static bool should_cache (const platform_item_ptr& item);

      static bool should_cache (const rx_namespace_item& item);


  protected:

  private:


      name_items_hash_type name_items_hash_;


};







class rx_directory_resolver 
{
	struct resolver_data
	{
		string_type path;
		rx_directory_ptr dir;
		bool resolved;
	};
	typedef std::vector<resolver_data> directories_type;

  public:

      rx_namespace_item resolve_path (const string_type& path);

      void add_paths (std::initializer_list<string_type> paths);


  protected:

  private:


      directories_type directories_;


};


} // namespace ns
} // namespace rx_platform



#endif
