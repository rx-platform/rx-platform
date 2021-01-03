

/****************************************************************************
*
*  system\runtime\rx_objbase.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "system/server/rx_server.h"

// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_io_buffers
#include "system/runtime/rx_io_buffers.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"
// dummy
#include "dummy.h"
// rx_ptr
#include "lib/rx_ptr.h"

#include "lib/security/rx_security.h"
#include "system/callbacks/rx_callback.h"
#include "rx_process_context.h"
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

template <class ptrT>
struct constructed_data_t
{
    ptrT ptr;
    std::function<void(const rx_node_id&)> register_f;
    std::function<void(const rx_node_id&)> unregister_f;
};



namespace runtime {
namespace io_types
{
namespace ports_templates
{
template <typename translatorT, typename addrT>
class routing_endpoint;
}
}

namespace items {
// instance data forwards
class port_instance_data;






class object_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0, 3, 0, "\
object class. basic implementation of an object");

	DECLARE_REFERENCE_PTR(object_runtime);

    friend class algorithms::runtime_holder<meta::object_types::object_type>;
    friend class object_instance_data;

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

  private:


      runtime_process_context *context_;


};






class application_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system application class. basic implementation of a application");

	DECLARE_REFERENCE_PTR(application_runtime);

    friend class algorithms::runtime_holder<meta::object_types::application_type>;
    friend class application_instance_data;

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

  private:


      runtime_process_context *context_;


};






class domain_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,1, "\
system domain class. basic implementation of a domain");

	DECLARE_REFERENCE_PTR(domain_runtime);

    friend class algorithms::runtime_holder<meta::object_types::domain_type>;
    friend class domain_instance_data;

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

  private:


      runtime_process_context *context_;


};







class port_runtime : public rx::pointers::reference_object  
{
	DECLARE_CODE_INFO("rx", 0,5,0, "\
system port class. basic implementation of a port");
    
	DECLARE_REFERENCE_PTR(port_runtime);

    friend class algorithms::runtime_holder<meta::object_types::port_type>;
    friend class port_instance_data;
    template <typename translatorT, typename addrT>
    friend class io_types::ports_templates::routing_endpoint;

  public:
      port_runtime();

      ~port_runtime();


      virtual rx_result initialize_runtime (runtime_init_context& ctx);

      virtual rx_result start_runtime (runtime_start_context& ctx);

      virtual rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      virtual rx_result stop_runtime (runtime_stop_context& ctx);

      virtual void stack_assembled ();

      virtual void stack_disassembled ();

      virtual rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      virtual rx_result_with<rx_protocol_stack_endpoint*> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      virtual rx_result stop_passive ();

      virtual rx_protocol_stack_endpoint* construct_endpoint ();

      rx_result register_routing_endpoint (rx_protocol_stack_endpoint* what);

      rx_result unbind_stack_endpoint (rx_protocol_stack_endpoint* what);

      rx_result disconnect_stack_endpoint (rx_protocol_stack_endpoint* what);

      virtual void destroy_endpoint (rx_protocol_stack_endpoint* what) = 0;

      threads::job_thread* get_jobs_queue ();

      void add_periodic_job (jobs::periodic_job::smart_ptr job);

      threads::job_thread* get_io_queue ();

      virtual void extract_bind_address (const data::runtime_values_data& binder_data, io::any_address& local_addr, io::any_address& remote_addr);

      virtual rx_result_with<security::security_context_ptr> create_security_context ();


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
      valT get_binded_as(runtime_handle_t handle, const valT& default_value) const
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
      template<typename funcT, typename... Args>
      void send_function(funcT&& func, Args&&... args)
      {
          auto job = rx_create_job<smart_ptr, funcT, Args...>()(smart_this(), std::forward<funcT>(func), std::forward<Args>(args)...);
          get_jobs_queue()->append(job);
      }
      template<typename funcT, typename... Args>
      void send_io_function(funcT&& func, Args&&... args)
      {
          auto job = rx_create_job<smart_ptr, funcT, Args...>()(smart_this(), std::forward<funcT>(func), std::forward<Args>(args)...);
          get_io_queue()->append(job);
      }
      template<typename funcT, typename... Args>
      rx_timer_ptr create_timer_function(funcT&& func, Args&&... args)
      {
          auto job = rx_create_timer_job<smart_ptr, funcT, Args...>()(smart_this(), std::forward<funcT>(func), std::forward<Args>(args)...);
          add_periodic_job(job);
          return job;
      }
      template<class refT, class funcT, class callbackT>
      void do_io_data_with_callback(funcT&& what, callbackT&& callback)
      {
          rx_thread_handle_t ctx = RX_DOMAIN_IO;
          rx_do_with_callback(ctx, smart_this(), std::forward<funcT>(what), std::forward<callbackT>(callback));
      }
      template<class refT, class funcT, class callbackT>
      void do_base_data_with_callback(funcT&& what, callbackT&& callback)
      {
          rx_thread_handle_t ctx = RX_DOMAIN_META;
          rx_do_with_callback(ctx, smart_this(), std::forward<funcT>(what), std::forward<callbackT>(callback));
      }
  protected:

      rx_result listen (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result connect (const protocol_address* local_address, const protocol_address* remote_address);

      rx_result add_stack_endpoint (rx_protocol_stack_endpoint* what, const io::any_address& local_addr, const io::any_address& remote_addr);

      rx_result close_all_endpoints ();


      runtime_process_context * get_context ()
      {
        return context_;
      }



  private:


      runtime_process_context *context_;


      rx_thread_handle_t executer_;

      rx_port_ptr runtime_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
