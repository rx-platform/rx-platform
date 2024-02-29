

/****************************************************************************
*
*  lib\rx_meta_data.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_meta_data_h
#define rx_meta_data_h 1



// initial version of an item
#define RX_INITIAL_ITEM_VERSION 0x10000



#define RX_DIR_DELIMETER '/'
#define RX_OBJECT_DELIMETER '.'
#define RX_DIR_OBJECT_DELIMETER '/'
#define RX_PATH_CURRENT '.'
#define RX_PATH_PARENT '#'
#define RX_SOURCE_DELIMETER '#'
#define RX_DIR_DELIMETER_STR "/"
#define RX_OBJECT_DELIMETER_STR "."
#define RX_DIR_OBJECT_DELIMETER_STR "/"
#define RX_PATH_CURRENT_STR "."
#define RX_PATH_PARENT_STR '$'
#define RX_DEFAULT_VARIABLE_NAME "_"
#define RX_SOURCE_DELIMETER_STR "#"
#define RX_MACRO_SYMBOL '$'
#define RX_MACRO_SYMBOL_STR "$"
// cpp classes stuff
#define RX_CPP_OBJECT_CLASS_TYPE_NAME "object_type"
#define RX_CPP_OBJECT_TYPE_NAME "object"
#define RX_CPP_OBJECT_TYPE_ID "object"
#define RX_CPP_DOMAIN_CLASS_TYPE_NAME "domain_type"
#define RX_CPP_DOMAIN_TYPE_NAME "domain"
#define RX_CPP_APPLICATION_CLASS_TYPE_NAME "application_type"
#define RX_CPP_APPLICATION_TYPE_NAME "application"
#define RX_CPP_PORT_CLASS_TYPE_NAME "port_type"
#define RX_CPP_PORT_TYPE_NAME "port"
#define RX_TEST_CASE_TYPE_NAME "test_case"

#define RX_CPP_DIRECORY_TYPE_NAME "directory"
#define RX_CPP_COMMAND_TYPE_NAME "command"

#define RX_CONST_VALUE_TYPE_NAME "const_value"
#define RX_VALUE_TYPE_NAME "value"
#define RX_INDIRECT_VALUE_TYPE_NAME "value"

#define RX_CPP_VARIABLE_CLASS_TYPE_NAME "variable_type"
#define RX_CPP_VARIABLE_TYPE_NAME "variable"
#define RX_CPP_STRUCT_CLASS_TYPE_NAME "struct_type"
#define RX_CPP_STRUCT_TYPE_NAME "struct"
#define RX_CPP_MAPPER_CLASS_TYPE_NAME "mapper_type"
#define RX_CPP_MAPPER_TYPE_NAME "mapper"
#define RX_CPP_SOURCE_CLASS_TYPE_NAME "source_type"
#define RX_CPP_SOURCE_TYPE_NAME "source"
#define RX_CPP_FILTER_CLASS_TYPE_NAME "filter_type"
#define RX_CPP_FILTER_TYPE_NAME "filter"
#define RX_CPP_EVENT_CLASS_TYPE_NAME "event_type"
#define RX_CPP_EVENT_TYPE_NAME "event"
#define RX_CPP_RELATION_CLASS_TYPE_NAME "relation_type"
#define RX_CPP_RELATION_TYPE_NAME "relation"
#define RX_CPP_RELATION_TARGET_TYPE_NAME "relation_target"

#define RX_CPP_METHOD_CLASS_TYPE_NAME "method_type"
#define RX_CPP_METHOD_TYPE_NAME "method"
#define RX_CPP_PROGRAM_CLASS_TYPE_NAME "program_type"
#define RX_CPP_PROGRAM_TYPE_NAME "program"
#define RX_CPP_DATA_CLASS_TYPE_NAME "data_type"
#define RX_CPP_DATA_TYPE_NAME "data"
#define RX_CPP_DISPLAY_CLASS_TYPE_NAME "display_type"
#define RX_CPP_DISPLAY_TYPE_NAME "display"

//#include "system/server/rx_ns.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"



namespace rx {

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
    rx_relation_type = 15,
    rx_program_type = 16,
    rx_method_type = 17,
    rx_data_type = 18,
    rx_display_type = 19,
    rx_relation = 20,

    rx_first_invalid = 21,

    rx_test_case_type = 0xfe,
    rx_invalid_type = 0xff
};


bool rx_is_runtime(rx_item_type type);



struct rx_update_runtime_data
{
    rx_uuid checkout;
    bool increment_version = false;
    bool initialize_data = false;
    bool release_forced = false;
};


struct rx_update_type_data
{
    rx_uuid checkout;
    bool increment_version = false;
};


enum class rx_object_command_t
{
    rx_turn_off = 0,
    rx_turn_on,
    rx_set_blocked,
    rx_reset_blocked,
    rx_set_test,
    rx_reset_test
};

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
    value_attribute_type = 8,
    relation_attribute_type = 9,
    relation_target_attribute_type = 10,
    method_attribute_type = 11,
    program_attribute_type = 12,
    data_attribute_type = 13,
    display_attribute_type = 14,
    const_array_attribute_type = 15,
    value_array_attribute_type = 16,
    variable_array_attribute_type = 17,
    struct_array_attribute_type = 18,
    data_array_attribute_type = 19,

    const_data_attribute_type = 20,
    value_data_attribute_type = 21,
    variable_data_attribute_type = 22,
    const_data_array_attribute_type = 23,
    value_data_array_attribute_type = 24,
    variable_data_array_attribute_type = 25,
};
struct runtime_item_attribute
{
    rx_attribute_type type;
    string_type name;
    string_type full_path;
    rx_value value;
    bool is_complex() const
    {
        return type != const_attribute_type && type != value_attribute_type;
    }
};


enum rx_item_state : uint8_t
{
    rx_item_state_unknown = 0,
    rx_item_state_ok = 1,
    rx_item_state_need_reinit = 2,
    rx_item_state_deleted = 3
};


string_type rx_item_type_name(rx_item_type type);
string_type rx_runtime_attribute_type_name(rx_attribute_type type);
rx_item_type rx_parse_type_name(const string_type name);



extern rx_uuid running_at_local;
extern rx_uuid running_shared;





class meta_data 
{

  public:
      meta_data();


      rx_result serialize_meta_data (base_meta_writer& stream, uint8_t type, rx_item_type object_type) const;

      rx_result deserialize_meta_data (base_meta_reader& stream, uint8_t type, rx_item_type& object_type);

      rx_result check_in (base_meta_reader& stream);

      rx_result check_out (base_meta_writer& stream) const;

      bool is_system () const;

      values::rx_value get_value () const;

      string_type get_full_path () const;

      void get_full_path_with_buffer (string_type& buffer) const;

      rx_item_reference create_item_reference ();

      rx_item_reference create_weak_item_reference (const string_array& dirs);

      void increment_version (bool full_ver);

      void set_id (const rx_node_id& id);

      void set_parent (const rx_node_id& id);

      void set_name (const string_type& str);

      void set_path (const string_type& str);

      void set_created_time (rx_time tm);

      void set_modified_time (rx_time tm);

      void set_attributes (namespace_item_attributes val);

      void set_version (uint32_t val);

      void set_run_at (const string_type& run);


      rx_node_id id;

      string_type name;

      string_type path;

      string_type run_at;

      rx_item_reference parent;

      rx_time created_time;

      rx_time modified_time;

      uint32_t version;

      namespace_item_attributes attributes;

      rx_uuid running_at;


  protected:

  private:


};

meta_data create_meta_for_new(const meta_data& proto);

} // namespace rx



#endif
