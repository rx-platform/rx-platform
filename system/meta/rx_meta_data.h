

/****************************************************************************
*
*  system\meta\rx_meta_data.h
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


#ifndef rx_meta_data_h
#define rx_meta_data_h 1



// initial version of an item
#define RX_INITIAL_ITEM_VERSION 0x10000

//#include "system/server/rx_ns.h"

// rx_storage
#include "system/storage_base/rx_storage.h"
// rx_ser_lib
#include "lib/rx_ser_lib.h"

namespace rx_platform {
namespace runtime {
namespace objects {
class object_runtime;

} // namespace objects
} // namespace runtime
} // namespace rx_platform


using namespace rx_platform;
using namespace rx_platform::ns;


namespace rx_platform {

enum namespace_item_attributes
{
	namespace_item_null = 0,
	namespace_item_read_access = 1,
	namespace_item_write_access = 2,
	namespace_item_delete_access = 4,
	namespace_item_pull_access = 8,
	namespace_item_execute_access = 0x10,
	// special type of item
	namespace_item_system = 0x20,
	namespace_item_internal = 0x40,
	// combinations
	namespace_item_full_type_access = 7,
	namespace_item_full_access = 0x17,
	namespace_item_system_access = 0x21,
	namespace_item_internal_access = 0x61,
	namespace_item_system_storage = 0x60
};

namespace api
{
struct query_result_detail;
}

namespace meta {

enum rx_storage_type
{
	invalid_storage = 0,
	system_storage,
	user_storage,
	extern_storage,
	test_storage
};





class storage_data 
{

  public:

      rx_result_with<rx_storage_ptr> resolve_storage () const;

      string_type storage_name () const;

      void assign_storage (rx_storage_type storage_type);


      rx_storage_type get_storage_type () const
      {
        return storage_type_;
      }



  protected:

  private:


      rx_storage_type storage_type_;


};







class meta_data 
{

  public:
      meta_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, namespace_item_attributes attrs, const string_type& path, rx_time now = rx_time::now());

      meta_data (namespace_item_attributes attrs = namespace_item_null, rx_time now = rx_time::now());


      rx_result check_in (base_meta_reader& stream);

      rx_result check_out (base_meta_writer& stream) const;

      rx_result serialize_meta_data (base_meta_writer& stream, uint8_t type, const string_type& object_type) const;

      rx_result deserialize_meta_data (base_meta_reader& stream, uint8_t type, string_type& object_type);

      values::rx_value get_value () const;

      void construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, namespace_item_attributes attributes, const string_type& path);

      bool get_system () const;

      static rx_result_with<platform_item_ptr> deserialize_runtime_item (base_meta_reader& stream, uint8_t type);

      rx_result resolve ();

      void fill_query_result (api::query_result_detail& item) const;

      void set_path (const string_type& path);

      string_type get_full_path () const;


      const rx_node_id& get_parent () const
      {
        return parent_;
      }


      uint32_t get_version () const
      {
        return version_;
      }


      rx_time get_created_time () const
      {
        return created_time_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }


      const string_type& get_name () const
      {
        return name_;
      }


      const rx_node_id& get_id () const
      {
        return id_;
      }


      namespace_item_attributes get_attributes () const
      {
        return attributes_;
      }


      const string_type& get_path () const
      {
        return path_;
      }



      storage_data storage_info;


  protected:

  private:


      rx_node_id parent_;

      uint32_t version_;

      rx_time created_time_;

      rx_time modified_time_;

      string_type name_;

      rx_node_id id_;

      namespace_item_attributes attributes_;

      string_type path_;


};


} // namespace meta
} // namespace rx_platform



#endif
