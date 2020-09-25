

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
#include "system/runtime/rx_rt_struct.h"
using namespace rx_platform::ns;
using namespace rx::values;


namespace rx_platform {

namespace runtime {

namespace blocks {





class filter_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
filter runtime. basic implementation of an filter runtime");

	DECLARE_REFERENCE_PTR(filter_runtime);
    friend class structure::filter_data;

  public:
      filter_runtime();

      virtual ~filter_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_filter (runtime::runtime_init_context& ctx);

      virtual rx_result start_filter (runtime::runtime_start_context& ctx);

      virtual rx_result stop_filter (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_filter (runtime::runtime_deinit_context& ctx);


      static string_type type_name;


  protected:
      template<typename valT>
      valT get_binded_as(runtime_process_context* ctx, runtime_handle_t handle, const valT& default_value) const
      {
          if (ctx)
          {
              values::rx_simple_value temp_val;
              auto result = ctx->get_value(handle, temp_val);
              if (result)
              {
                  return values::extract_value<valT>(temp_val.get_storage(), default_value);
              }
          }
          return default_value;
      }
      template<typename valT>
      void set_binded_as(runtime_process_context* ctx, runtime_handle_t handle, valT&& value)
      {
          if (ctx)
          {
              values::rx_simple_value temp_val;
              temp_val.assign_static<valT>(std::forward<valT>(value));
              auto result = ctx->set_value(handle, std::move(temp_val));
          }
      }
  private:

      virtual bool supports_input () const;

      virtual bool supports_output () const;

      virtual rx_result filter_input (rx_value& val, runtime_process_context* ctx);

      virtual rx_result filter_output (rx_simple_value& val, runtime_process_context* ctx);



      structure::filter_data* container_;


};






class source_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
source runtime. basic implementation of an source runtime");

	DECLARE_REFERENCE_PTR(source_runtime);
    friend class structure::source_data;

  public:
      source_runtime();

      virtual ~source_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_source (runtime::runtime_init_context& ctx);

      virtual rx_result start_source (runtime::runtime_start_context& ctx);

      virtual rx_result stop_source (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_source (runtime::runtime_deinit_context& ctx);

      bool is_input () const;

      bool is_output () const;


      static string_type type_name;


  protected:

      rx_result source_value_changed (rx_value&& val);

      void source_result_received (rx_result&& result, runtime_transaction_id_t id);


      rx_value_t get_value_type () const;


  private:

      virtual bool supports_input () const;

      virtual bool supports_output () const;

      virtual rx_result source_write (structure::write_data&& data, runtime_process_context* ctx);



      structure::source_data* container_;

      rx_value_t value_type_;


};






class struct_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
struct runtime. basic implementation of an struct runtime");

	DECLARE_REFERENCE_PTR(struct_runtime);
    friend class structure::struct_data;

  public:
      struct_runtime();

      virtual ~struct_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_struct (runtime::runtime_init_context& ctx);

      virtual rx_result start_struct (runtime::runtime_start_context& ctx);

      virtual rx_result stop_struct (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_struct (runtime::runtime_deinit_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class variable_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
variable runtime. basic implementation of an variable runtime");

	DECLARE_REFERENCE_PTR(variable_runtime);
    friend class structure::variable_data;

  public:
      variable_runtime();

      virtual ~variable_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_variable (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_variable (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_variable (runtime::runtime_start_context& ctx);

      virtual rx_result stop_variable (runtime::runtime_stop_context& ctx);


      static string_type type_name;


  protected:

  private:

      virtual rx_value select_variable_input (runtime_process_context* ctx, runtime_sources_type& sources);

      virtual rx_result variable_write (structure::write_data&& data, runtime_process_context* ctx, runtime_sources_type& sources);



};






class event_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
event runtime. basic implementation of an event runtime");

	DECLARE_REFERENCE_PTR(event_runtime);
    friend class structure::event_data;

  public:
      event_runtime();

      virtual ~event_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_event (runtime::runtime_init_context& ctx);

      virtual rx_result start_event (runtime::runtime_start_context& ctx);

      virtual rx_result stop_event (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_event (runtime::runtime_deinit_context& ctx);


      static string_type type_name;


  protected:

  private:


};






class mapper_start_context 
{

  public:

      rx_value& init_value ();


  protected:

  private:


      rx_value* initial_value_;


};






class mapper_stop_context 
{

  public:

  protected:

  private:


};







class mapper_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
mapper runtime. basic implementation of an mapper runtime");

	DECLARE_REFERENCE_PTR(mapper_runtime);
    friend class structure::mapper_data;

  public:
      mapper_runtime();

      virtual ~mapper_runtime();


      string_type get_type_name () const;

      virtual rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      virtual rx_result start_mapper (runtime::runtime_start_context& ctx);

      virtual rx_result stop_mapper (runtime::runtime_stop_context& ctx);

      virtual rx_result deinitialize_mapper (runtime::runtime_deinit_context& ctx);

      bool can_read () const;

      bool can_write () const;

      threads::job_thread* get_jobs_queue ();


      static string_type type_name;


  protected:

      void mapper_write_pending (values::rx_simple_value&& value, runtime_transaction_id_t id);

      void map_current_value () const;


      rx_value_t get_value_type () const;


  private:

      virtual bool supports_read () const;

      virtual bool supports_write () const;

      virtual void mapped_value_changed (rx_value&& val);

      virtual void mapper_result_received (rx_result&& result, runtime_transaction_id_t id);



      structure::mapper_data* container_;

      rx_value_t value_type_;


};


} // namespace blocks
} // namespace runtime
} // namespace rx_platform



#endif
