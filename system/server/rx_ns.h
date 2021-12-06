

/****************************************************************************
*
*  system\server\rx_ns.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_ns_h
#define rx_ns_h 1


#include "lib/rx_lock.h"

// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_storage
#include "system/storage_base/rx_storage.h"
// rx_ptr
#include "lib/rx_ptr.h"


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

namespace rx_internal
{
namespace builders
{
class rx_platform_builder;
}
namespace internal_ns
{
class namespace_algorithms;
}// internal_ns
}// sys_internal


namespace rx_platform {


void rx_split_path(const string_type& full_path, string_type& directory_path, string_type& item_path);

namespace meta
{
class meta_data;
}
typedef typename meta::meta_data meta_data_t;

bool rx_is_valid_name_character(char ch);
bool rx_is_valid_namespace_name(const string_type& name);

typedef std::vector<rx_directory_ptr> platform_directories_type;


namespace ns {

void fill_attributes_string(namespace_item_attributes attr, string_type& str);







class rx_namespace_item 
{

  public:
      rx_namespace_item();

      rx_namespace_item (const platform_item_ptr& who);


      operator bool () const;

      bool is_object () const;

      bool is_type () const;

      rx_value get_value () const;


      const meta::meta_data& get_meta () const;


      rx_item_type get_type () const;

	  rx_namespace_item(const rx_namespace_item& right) = default;
	  rx_namespace_item(rx_namespace_item&& right) = default;
	  rx_namespace_item& operator=(const rx_namespace_item & right) = default;
	  rx_namespace_item& operator=(rx_namespace_item && right) = default;
  protected:

  private:


      meta::meta_data meta_;


      rx_item_type type_;

      uint32_t version_;


};

typedef std::vector<rx_namespace_item> platform_items_type;

using suggestions_type = std::set<string_type>;




class rx_platform_directory : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_platform_directory);
	typedef std::map<string_type, rx_platform_directory::smart_ptr>  sub_directories_type;
	typedef std::map<string_type, rx_namespace_item> sub_items_type;
	typedef std::unordered_set<string_type> reserved_type;
    friend class rx_internal::builders::rx_platform_builder;

  public:
      ~rx_platform_directory();


      virtual void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      virtual rx_item_type get_type_id () const;

      void fill_dir_code_info (std::ostream& info);

      virtual meta_data_t meta_info () const;

      virtual void list_content (platform_directories_type& sub_directories, platform_items_type& sub_items, const string_type& pattern) const;

      rx_result add_item (rx_namespace_item item);

      rx_result reserve_name (const string_type& name);

      rx_result cancel_reserve (const string_type& name);

      rx_namespace_item get_item (const string_type& name) const;

      rx_result delete_item (const string_type& name);


      rx_reference<storage_base::rx_platform_storage> get_storage ()
      {
        return storage_;
      }



  protected:

  private:


      sub_items_type sub_items_;

      meta::meta_data meta_;

      rx_reference<storage_base::rx_platform_storage> storage_;


      reserved_type reserved_;

      locks::slim_lock lock_;


    friend class rx_directory_cache;
    friend class rx_internal::internal_ns::namespace_algorithms;
};


} // namespace ns
} // namespace rx_platform



#endif
