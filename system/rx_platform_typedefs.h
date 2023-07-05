

/****************************************************************************
*
*  system\rx_platform_typedefs.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_platform_typedefs_h
#define rx_platform_typedefs_h 1


#include "lib/rx_const_size_vector.h"



namespace rx_platform {

namespace ns
{
class rx_platform_item;
class rx_platform_directory;
}
typedef std::unique_ptr<rx_platform::ns::rx_platform_item> platform_item_ptr;
typedef rx::pointers::reference<ns::rx_platform_directory> rx_directory_ptr;
typedef std::vector<rx_directory_ptr> platform_directories_type;

namespace storage_base
{
class rx_storage_item;
class rx_roles_storage_item;
class rx_platform_storage;
}
typedef std::unique_ptr<storage_base::rx_storage_item> rx_storage_item_ptr;
typedef rx_reference<storage_base::rx_platform_storage> rx_storage_ptr;
typedef std::unique_ptr<storage_base::rx_roles_storage_item> rx_roles_storage_item_ptr;


namespace threads
{
class job_thread;
}
namespace logic
{
class program_runtime;
class method_runtime;
}
typedef rx_reference<logic::program_runtime> program_runtime_ptr;
typedef rx_reference<logic::method_runtime> method_runtime_ptr;

namespace displays
{
class display_runtime;
}
typedef rx_reference<displays::display_runtime> display_runtime_ptr;

namespace api
{
struct query_result;
}
namespace meta
{

namespace meta_algorithm
{
template <class typeT>
class object_types_algorithm;
template <class typeT>
class basic_types_algorithm;
template <class typeT>
class object_data_algorithm;
template <class typeT>
class meta_blocks_algorithm;
class relation_blocks_algorithm;
class complex_data_algorithm;
}


namespace object_types
{
class object_type;
class application_type;
class domain_type;
class port_type;
class relation_type;

}
namespace basic_types
{
class variable_type;
class event_type;
class filter_type;
class source_type;
class mapper_type;
class struct_type;
class data_type;
class method_type;
class program_type;
class display_type;
}

namespace runtime_data
{
class object_runtime_data;
class domain_runtime_data;
class port_runtime_data;
class application_runtime_data;
}


}
namespace runtime {

struct write_data;
struct runtime_init_context;

namespace structure
{


// value options bitset
constexpr static int value_opt_readonly = 0;
constexpr static int value_opt_persistent = 1;

// internal status for state ignorant values (On, Test, Status Simulate)
constexpr static int opt_state_ignorant = 30;

// typedefs for masks
constexpr uint_fast8_t rt_const_index_type = 0x01;
constexpr uint_fast8_t rt_value_index_type = 0x02;
constexpr uint_fast8_t rt_variable_index_type = 0x03;
constexpr uint_fast8_t rt_struct_index_type = 0x04;
constexpr uint_fast8_t rt_source_index_type = 0x05;
constexpr uint_fast8_t rt_mapper_index_type = 0x06;
constexpr uint_fast8_t rt_filter_index_type = 0x07;
constexpr uint_fast8_t rt_event_index_type = 0x08;
constexpr uint_fast8_t rt_data_index_type = 0x09;
constexpr uint_fast8_t rt_const_data_index_type = 0x0a;
constexpr uint_fast8_t rt_value_data_index_type = 0x0b;

constexpr uint_fast8_t rt_bit_none = 0x00;
constexpr uint_fast8_t rt_bit_has_variables = 0x01;
constexpr uint_fast8_t rt_bit_has_structs = 0x02;
constexpr uint_fast8_t rt_bit_has_sources = 0x04;
constexpr uint_fast8_t rt_bit_has_mappers = 0x08;
constexpr uint_fast8_t rt_bit_has_filters = 0x10;
constexpr uint_fast8_t rt_bit_has_events = 0x20;

typedef uint_fast16_t members_index_type;

constexpr members_index_type rt_type_shift = 0xc;
constexpr members_index_type rt_type_mask = 0xfff;

struct index_data
{
    string_type name;
    members_index_type index;
};

};

namespace relations
{
class relation_runtime;
class relation_data;
}
namespace items {
class port_runtime;
class object_runtime;
class application_runtime;
class domain_runtime;

} // namespace items
namespace algorithms
{
template <class typeT>
class runtime_holder;
template <class typeT>
class runtime_holder_algorithms;
template <class typeT>
class runtime_scan_algorithms;
class runtime_relation_algorithms;
}


namespace tag_blocks
{
class rx_tags_callback;
typedef rx_reference<rx_tags_callback> tags_callback_ptr;
typedef std::function<void(const rx_value&)> binded_callback_t;
}
using tag_blocks::tags_callback_ptr;


namespace blocks
{
class variable_runtime;
class struct_runtime;
class source_runtime;
class mapper_runtime;
class filter_runtime;
class event_runtime;
}
namespace relations
{
class relation_data;
class relation_value_data;
class relation_runtime;
}


typedef rx::pointers::reference<blocks::struct_runtime> struct_runtime_ptr;
typedef rx::pointers::reference<blocks::variable_runtime> variable_runtime_ptr;
typedef rx::pointers::reference<blocks::source_runtime> source_runtime_ptr;
typedef rx::pointers::reference<blocks::mapper_runtime> mapper_runtime_ptr;
typedef rx::pointers::reference<blocks::filter_runtime> filter_runtime_ptr;
typedef rx::pointers::reference<blocks::event_runtime> event_runtime_ptr;
typedef rx::pointers::reference<relations::relation_runtime> relation_runtime_ptr;

namespace structure {
template<typename typeT>
class array_wrapper;
class const_value_data;
class value_data;
class full_value_data;
class struct_data;
class variable_data;
class event_data;
class filter_data;
class source_data;
class mapper_data;
class runtime_item;
//class write_context;
} // namespace structure

typedef std::unique_ptr<structure::runtime_item> runtime_item_ptr;

namespace logic_blocks
{
class method_data;
class program_data;
}


typedef rx::const_size_vector<structure::array_wrapper<structure::variable_data> > runtime_variables_type;
typedef rx::const_size_vector< structure::array_wrapper<structure::struct_data> > runtime_structs_type;
typedef rx::const_size_vector<structure::event_data> runtime_events_type;
typedef rx::const_size_vector<structure::filter_data> runtime_filters_type;
typedef rx::const_size_vector<structure::source_data> runtime_sources_type;
typedef rx::const_size_vector<structure::mapper_data> runtime_mappers_type;

union rt_value_ref_union
{
    structure::const_value_data* const_value;
    structure::value_data* value;
    structure::full_value_data* full_value;
    structure::variable_data* variable;
    logic_blocks::method_data* method;
    relations::relation_data* relation;
    relations::relation_value_data* relation_value;
};
enum class rt_value_ref_type
{
    rt_null = 0,
    rt_const_value = 1,
    rt_value = 2,
    rt_full_value = 3,
    rt_variable = 4,
    rt_method = 5,
    rt_relation = 6,
    rt_relation_value = 7
};
struct rt_value_ref
{
    rt_value_ref_type ref_type;
    rt_value_ref_union ref_value_ptr;
};

typedef std::unique_ptr<structure::runtime_item> rx_runtime_item_ptr;



}
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::domain_type> > rx_domain_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::port_type> > rx_port_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::object_type> > rx_object_ptr;
typedef rx_reference<runtime::algorithms::runtime_holder<meta::object_types::application_type> > rx_application_ptr;
typedef rx_reference<runtime::relations::relation_data> rx_relation_ptr;

typedef rx_reference<runtime::items::object_runtime> rx_object_impl_ptr;
typedef rx_reference<runtime::items::port_runtime> rx_port_impl_ptr;
typedef rx_reference<runtime::items::application_runtime> rx_application_impl_ptr;
typedef rx_reference<runtime::items::domain_runtime> rx_domain_impl_ptr;
typedef rx_reference<runtime::relations::relation_runtime> rx_relation_impl_ptr;




typedef pointers::reference<meta::object_types::object_type> rx_object_type_ptr;
typedef pointers::reference<meta::object_types::domain_type> rx_domain_type_ptr;
typedef pointers::reference<meta::object_types::application_type> rx_application_type_ptr;
typedef pointers::reference<meta::object_types::port_type> rx_port_type_ptr;
typedef pointers::reference<meta::object_types::relation_type> relation_type_ptr;


typedef pointers::reference<meta::basic_types::mapper_type> mapper_type_ptr;
typedef pointers::reference<meta::basic_types::struct_type> struct_type_ptr;
typedef pointers::reference<meta::basic_types::variable_type> variable_type_ptr;
typedef pointers::reference<meta::basic_types::source_type> source_type_ptr;
typedef pointers::reference<meta::basic_types::event_type> event_type_ptr;
typedef pointers::reference<meta::basic_types::filter_type> filter_type_ptr;
typedef pointers::reference<meta::basic_types::data_type> data_type_ptr;
typedef pointers::reference<meta::basic_types::method_type> method_type_ptr;
typedef pointers::reference<meta::basic_types::program_type> program_type_ptr;
typedef pointers::reference<meta::basic_types::display_type> display_type_ptr;


enum subscription_trigger_type
{
    subscription_trigger_periodic = 0,
    subscription_trigger_critical = 1,

    max_trigger_type = 1
};
enum class runtime_status_type
{
    info = 0,
    warning = 1,
    error = 2
};
struct runtime_status_record
{
    string_type message;
};
struct runtime_status_data
{
    runtime_status_type type;
    string_type path;
    runtime_status_record data;
};

}




#endif
