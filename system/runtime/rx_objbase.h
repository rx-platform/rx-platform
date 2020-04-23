

/****************************************************************************
*
*  system\runtime\rx_objbase.h
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


#ifndef rx_objbase_h
#define rx_objbase_h 1


#include "protocols/ansi_c/common_c/rx_protocol_base.h"

// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_io_buffers
#include "system/runtime/rx_io_buffers.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "lib/security/rx_security.h"
#include "system/callbacks/rx_callback.h"
#include "rx_runtime_helpers.h"
using namespace rx;
using namespace rx_platform::ns;
using rx::values::rx_value;
using rx::values::rx_simple_value;
namespace rx_internal
{
namespace sys_runtime
{
namespace algorithms
{
class application_algorithms;
}
}
}


namespace rx_platform {

void rx_split_item_path(const string_type& full_path, string_type& object_path, string_type& item_path);

typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
typedef std::stack<buffer_ptr, std::vector<buffer_ptr> > buffers_type;




namespace runtime {

namespace items {
// instance data forwards
class port_instance_data;






class object_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
object class. basic implementation of an object");

	DECLARE_REFERENCE_PTR(object_runtime);

    friend class algorithms::runtime_holder<meta::object_types::object_type>;

  public:
      object_runtime();

      ~object_runtime();


      virtual rx_result initialize_runtime (runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime_stop_context& ctx);


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


};






class application_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system application class. basic implementation of a application");

	DECLARE_REFERENCE_PTR(application_runtime);

    friend class algorithms::runtime_holder<meta::object_types::application_type>;

  public:
      application_runtime();

      ~application_runtime();


      virtual rx_result initialize_runtime (runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime_stop_context& ctx);

      virtual int get_executer ();


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


};






class domain_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system domain class. basic implementation of a domain");

	DECLARE_REFERENCE_PTR(domain_runtime);
    
    friend class algorithms::runtime_holder<meta::object_types::domain_type>;

  public:
      domain_runtime();

      ~domain_runtime();


      virtual rx_result initialize_runtime (runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime_stop_context& ctx);

      virtual int get_executer ();


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:

  private:


};







class port_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system port class. basic implementation of a port");

	DECLARE_REFERENCE_PTR(port_runtime);

    friend class algorithms::runtime_holder<meta::object_types::port_type>;
    friend class port_instance_data;

  public:
      port_runtime();

      ~port_runtime();


      virtual rx_protocol_stack_entry* create_stack_entry ();

      virtual rx_result initialize_runtime (runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime_stop_context& ctx);

      virtual rx_port_impl_ptr up_stack () const = 0;

      virtual rx_port_impl_ptr down_stack () const = 0;

      virtual void connect_up_stack (rx_port_impl_ptr who) = 0;

      virtual void connect_down_stack (rx_port_impl_ptr who) = 0;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }



      static rx_item_type type_id;

      template<typename valT>
      valT get_binded_as(runtime_handle_t handle, const valT& default_value)
      {
          if (context_)
          {
              values::rx_simple_value temp_val;
              auto result = context_->get_value(handle, temp_val);
              if (result)
              {
                  return values::extract_value<valT>(temp_val.get_storage(), default_value);
              }
          }
          return default_value;
      }
      template<typename valT>
      void set_binded_as(runtime_handle_t handle, valT&& value)
      {
          if (context_)
          {
              values::rx_simple_value temp_val;
              temp_val.assign_static<valT>(std::forward<valT>(value));
              auto result = context_->set_value(handle, std::move(temp_val));
          }
      }
  protected:

      virtual void structure_changed () = 0;


      algorithms::runtime_process_context * get_context ()
      {
        return context_;
      }



      rx_security_handle_t get_identity () const
      {
        return identity_;
      }



  private:

      virtual bool has_up_port () const = 0;



      algorithms::runtime_process_context *context_;


      rx_thread_handle_t executer_;

      rx_security_handle_t identity_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
