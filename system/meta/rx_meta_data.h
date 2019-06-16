

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


namespace rx_platform {


enum rx_attribute_type
{
	invalid_attribute_type_type = 0,
	struct_attribute_type = 1,
	variable_attribute_type = 2,
	source_attribute_type = 3,
	filter_attribute_type = 4,
	event_attribute_type = 5,
	mapper_attribute_type = 6,
	const_attribute_type = 7,
	value_attribute_type = 8
};
struct runtime_item_attribute
{
	rx_attribute_type type;
	string_type name;
	string_type full_path;
};

enum rx_item_type : uint8_t
{
	rx_directory = 0,
	rx_application = 1,
	rx_application_type = 2,
	rx_domain = 3,
	rx_domain_type = 4,
	rx_object = 5,
	rx_object_type = 6,
	rx_port = 7,
	rx_port_type = 8,
	rx_struct_type = 9,
	rx_variable_type = 10,
	rx_source_type = 11,
	rx_filter_type = 12,
	rx_event_type = 13,
	rx_mapper_type = 14,
	rx_program = 15,
	rx_method = 16,

	rx_first_invalid = 17,

	rx_test_case_type = 0xfe,
	rx_invalid_type = 0xff
};

string_type rx_item_type_name(rx_item_type type);
rx_item_type rx_parse_type_name(const string_type name);

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
	namespace_item_full_type_access = 0xf,
	namespace_item_full_access = 0x1f,
	namespace_item_system_access = 0x29,
	namespace_item_internal_access = 0x61,
	// masks
	namespace_item_system_mask = 0x60
};

namespace api
{
struct rx_context;
struct query_result_detail;
struct query_result;
}

namespace meta {






class meta_data 
{

  public:
      meta_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, namespace_item_attributes attrs, const string_type& path, rx_time now = rx_time::now());

      meta_data (namespace_item_attributes attrs = namespace_item_null, rx_time now = rx_time::now());


      rx_result check_in (base_meta_reader& stream);

      rx_result check_out (base_meta_writer& stream) const;

      rx_result serialize_meta_data (base_meta_writer& stream, uint8_t type, rx_item_type object_type) const;

      rx_result deserialize_meta_data (base_meta_reader& stream, uint8_t type, rx_item_type& object_type);

      values::rx_value get_value () const;

      void construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, namespace_item_attributes attributes, const string_type& path);

      bool get_system () const;

      static rx_result_with<platform_item_ptr> deserialize_runtime_item (base_meta_reader& stream, uint8_t type);

      rx_result resolve ();

      void set_path (const string_type& path);

      string_type get_full_path () const;

      bool is_system () const;

      rx_result_with<rx_storage_ptr> resolve_storage () const;

      void increment_version (bool full_ver);


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
