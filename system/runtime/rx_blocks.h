

/****************************************************************************
*
*  system\runtime\rx_blocks.h
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


#ifndef rx_blocks_h
#define rx_blocks_h 1



// rx_callback
#include "system/callbacks/rx_callback.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_operational
#include "system/runtime/rx_operational.h"
// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_logic
#include "system/logic/rx_logic.h"

#include "system/server/rx_ns.h"
#include "system/callbacks/rx_callback.h"
using namespace rx_platform::ns;
using namespace rx::values;


namespace rx_platform {
namespace meta
{
class runtime_data_prototype;

namespace def_blocks
{
	class complex_data_type;
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
}

namespace runtime {

namespace blocks {





class filter_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
filter runtime. basic implementation of an filter runtime");

	DECLARE_REFERENCE_PTR(filter_runtime);

	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::filter_type;

  public:
      filter_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class mapper_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
mapper runtime. basic implementation of an mapper runtime");

	DECLARE_REFERENCE_PTR(mapper_runtime);
	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::mapper_type;

  public:
      mapper_runtime();

      ~mapper_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class struct_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
struct runtime. basic implementation of an struct runtime");

	DECLARE_REFERENCE_PTR(struct_runtime);

	typedef std::vector<mapper_runtime::smart_ptr> mappers_type;
	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::struct_type;

  public:
      struct_runtime();

      struct_runtime (const string_type& name, const rx_node_id& id, bool system = false);


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

      variable_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      ~variable_runtime();


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

	friend class meta::def_blocks::complex_data_type;
	friend class meta::basic_types::source_type;

  public:
      source_runtime();

      ~source_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      bool is_input () const
      {
        return input_;
      }


      bool is_output () const
      {
        return output_;
      }



      static string_type type_name;


  protected:

  private:


      rx_reference<variable_runtime> my_variable_;


      bool input_;

      bool output_;


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


      string_type get_type_name () const;

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

  private:


};







class runtime_holder 
{
	typedef std::vector<program_runtime_ptr> programs_type;

  public:

      rx_result read_value (const string_type& path, std::function<void(rx_value)> callback, api::rx_context ctx, rx_thread_handle_t whose) const;

      rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx, rx_thread_handle_t whose);

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result connect_items (const string_array& paths, operational::rx_tags_callback* monitor, std::vector<rx_result_with<runtime_handle_t> >& results, bool& has_errors);

      rx_result disconnect_items (const std::vector<runtime_handle_t>& items, operational::rx_tags_callback* monitor, std::vector<rx_result>& results, bool& has_errors);

      rx_result do_command (rx_object_command_t command_type);

      void set_runtime_data (meta::runtime_data_prototype& prototype);

      structure::hosting_object_data get_object_state () const;

      void fill_data (const data::runtime_values_data& data);

      void collect_data (data::runtime_values_data& data) const;

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);

      bool process_runtime (runtime_process_context& ctx);

      rx_result browse (const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items);

	  template<typename valT>
	  valT get_binded_as(runtime_handle_t handle, const valT& default_value)
	  {
		  values::rx_simple_value temp_val;
		  auto result = binded_tags_.get_value(handle, temp_val);
		  if (result)
		  {
			  return values::extract_value<valT>(temp_val.get_storage(), default_value);
		  }
		  return default_value;
	  }
	  template<typename valT>
	  void set_binded_as(runtime_handle_t handle, valT&& value)
	  {
		  values::rx_simple_value temp_val;
		  temp_val.assign_static<valT>(std::move(value));
		  auto result = binded_tags_.set_value(handle, std::move(temp_val));
	  }
	  template<typename valT>
	  valT get_local_as(const string_type& path, const valT& default_value)
	  {
		  return item_->get_local_as<valT>(path, default_value);
	  }
  protected:

  private:

      rx_mode_type get_mode () const
      {
        return mode_;
      }


      rx_time get_change_time () const
      {
        return change_time_;
      }




      operational::connected_tags connected_tags_;

      operational::binded_tags binded_tags_;

      programs_type programs_;

      structure::runtime_item::smart_ptr item_;


      rx_mode_type mode_;

      rx_time change_time_;

      jobs::job_ptr process_job_;


};


} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
