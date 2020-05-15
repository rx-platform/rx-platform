

/****************************************************************************
*
*  system\runtime\rx_blocks.h
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


#ifndef rx_blocks_h
#define rx_blocks_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/server/rx_ns.h"
#include "system/callbacks/rx_callback.h"
#include "system/runtime/rx_runtime_helpers.h"
using namespace rx_platform::ns;
using namespace rx::values;


namespace rx_platform {
namespace meta
{
class runtime_data_prototype;

namespace def_blocks
{
	class complex_data_type;
    class source_attribute;
    class mapper_attribute;
    class filter_attribute;
}
namespace basic_types
{
	class struct_type;
	class variable_type;
	class source_type;
	class mapper_type;
	class filter_type;
	class event_type;
}
namespace object_types
{
	class object_data_type;
}
}

namespace runtime {

namespace relations {
class relation_instance_data;

} // namespace relations

namespace blocks {





class filter_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
filter runtime. basic implementation of an filter runtime");

	DECLARE_REFERENCE_PTR(filter_runtime);

    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::filter_attribute>;
	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::filter_type;
    friend class structure::filter_data;

  public:
      filter_runtime();

      virtual ~filter_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      bool is_input () const;

      bool is_output () const;

      virtual rx_result filter_write_value (rx_simple_value& val);

      virtual rx_result filter_read_value (rx_value& val);


      static string_type type_name;


  protected:

  private:

      virtual bool supports_input () const;

      virtual bool supports_output () const;



      structure::mapper_data* container_;

      io_capabilities io_;


};






class struct_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
struct runtime. basic implementation of an struct runtime");

	DECLARE_REFERENCE_PTR(struct_runtime);

	typedef std::vector<mapper_runtime_ptr> mappers_type;
	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::struct_type;

  public:
      struct_runtime();

      virtual ~struct_runtime();


      bool serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class variable_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
variable runtime. basic implementation of an variable runtime");

	DECLARE_REFERENCE_PTR(variable_runtime);

	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::variable_type;

  public:
      variable_runtime();

      virtual ~variable_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      virtual rx_result write_value (rx_simple_value&& val, const structure::write_context& ctx, runtime_sources_type& sources);

      virtual rx_value select_input (algorithms::runtime_process_context* ctx, runtime_sources_type& sources);


      static string_type type_name;


  protected:

  private:


};






class event_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
event runtime. basic implementation of an event runtime");

	DECLARE_REFERENCE_PTR(event_runtime);

	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::event_type;

  public:
      event_runtime();

      virtual ~event_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class source_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
source runtime. basic implementation of an source runtime");

	DECLARE_REFERENCE_PTR(source_runtime);

    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::source_attribute>;
	friend class meta::def_blocks::source_attribute;
	friend class meta::basic_types::source_type;
    friend class structure::source_data;

  public:
      source_runtime();

      virtual ~source_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      virtual rx_result write_value (rx_simple_value&& val, const structure::write_context& ctx);

      virtual rx_result filter_output_value (rx_simple_value& val, runtime_filters_type& filters);

      virtual rx_result filter_input_value (rx_simple_value& val, runtime_filters_type& filters);

      bool is_input () const;

      bool is_output () const;

      const rx_value& get_current_value () const;


      static string_type type_name;


  protected:

      rx_result value_changed (rx_value&& val);


  private:

      virtual bool supports_input () const;

      virtual bool supports_output () const;



      structure::source_data* container_;

      io_capabilities io_;

      rx_value current_value_;


};






class mapper_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
mapper runtime. basic implementation of an mapper runtime");

	DECLARE_REFERENCE_PTR(mapper_runtime);
    friend class meta::meta_algorithm::meta_blocks_algorithm<meta::def_blocks::mapper_attribute>;
	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::mapper_type;
    friend class structure::mapper_data;

  public:
      mapper_runtime();

      virtual ~mapper_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_mapper (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_mapper (runtime::runtime_start_context& ctx);

      virtual rx_result stop_mapper (runtime::runtime_stop_context& ctx);

      bool can_read () const;

      bool can_write () const;


      static string_type type_name;


  protected:

      void mapper_write_pending (values::rx_simple_value&& value, runtime_transaction_id_t id);

      void map_current_value () const;


  private:

      virtual bool supports_read () const;

      virtual bool supports_write () const;

      virtual void mapped_value_changed (rx_value&& val);

      virtual void mapper_write_result (rx_result&& result, runtime_transaction_id_t id);



      structure::mapper_data* container_;

      io_capabilities io_;


};


} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
