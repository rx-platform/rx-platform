

/****************************************************************************
*
*  interfaces\rx_port_instance.h
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


#ifndef rx_port_instance_h
#define rx_port_instance_h 1



// rx_identity
#include "system/server/rx_identity.h"
// rx_runtime_data
#include "system/meta/rx_runtime_data.h"
// rx_port_stack_data
#include "interfaces/rx_port_stack_data.h"
// rx_security
#include "security/rx_security.h"

namespace rx_platform {
namespace runtime {
namespace items {
class port_runtime;
} // namespace items
} // namespace runtime
} // namespace rx_platform

namespace rx_internal {
namespace sys_runtime {
namespace runtime_core {
namespace runtime_data {
class application_instance_data;

} // namespace runtime_data
} // namespace runtime_core
} // namespace sys_runtime
} // namespace rx_internal


#include "lib/rx_io_addr.h"
#include "system/runtime/rx_runtime_helpers.h"
using namespace rx_platform;
using namespace rx_platform::runtime;
using namespace rx_platform::meta::runtime_data;


namespace rx_internal {

namespace interfaces {

namespace port_stack {





class port_active_behavior 
{

  public:

      virtual bool is_extern () = 0;

      virtual bool is_endpoit_binder () = 0;

      virtual bool is_endpoint_user () = 0;

      virtual ~port_active_behavior() = default;
  protected:

  private:


};






class port_build_behavior 
{

  public:

      virtual bool is_assemble_sender () = 0;

      virtual bool is_assemble_subscriber () = 0;

      virtual bool is_external () = 0;

      virtual bool is_application () = 0;

      virtual ~port_build_behavior() = default;
  protected:

  private:


};






class port_passive_behavior 
{

  public:

      virtual bool is_listen_sender () = 0;

      virtual bool is_connect_sender () = 0;

      virtual bool is_listen_subscriber () = 0;

      virtual bool is_connect_subscriber () = 0;

      virtual ~port_passive_behavior() = default;
  protected:

  private:


};


} // namespace port_stack
} // namespace interfaces

namespace sys_runtime {

namespace runtime_core {

namespace runtime_data {





class port_behaviors 
{

  public:

      std::unique_ptr<interfaces::port_stack::port_build_behavior> build_behavior;

      std::unique_ptr<interfaces::port_stack::port_passive_behavior> passive_behavior;

      std::unique_ptr<interfaces::port_stack::port_active_behavior> active_behavior;

	  port_behaviors(int arg) {} // Constructor for initializing with an integer, if needed
      port_behaviors() = default;
      ~port_behaviors() = default;
      port_behaviors(port_behaviors&&) noexcept = default;
      port_behaviors(const port_behaviors&) = delete;
      port_behaviors& operator=(port_behaviors&&) noexcept = default;
      port_behaviors& operator=(const port_behaviors&) = delete;
  protected:

  private:


};






class port_instance_data 
{

  public:
      port_instance_data (const port_data& data, port_behaviors&& rt_behavior);

      ~port_instance_data();


      bool connect_application (rx_application_ptr&& app, rx_port_ptr whose);

      bool disconnect_application (rx_port_ptr whose);

      static rx_result before_init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_port_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx);

      security::security_context_ptr get_security_context () const;


      rx_application_ptr get_my_application ();

      const rx_platform::meta::runtime_data::port_data& get_data () const
      {
        return data_;
      }



      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }



      port_behaviors behavior;

      interfaces::port_stack::port_stack_data stack_data;

      port_instance_data(port_instance_data&& right) noexcept = default;
      port_instance_data(const port_instance_data & right) = default;
  protected:

  private:


      rx_application_ptr my_application_;

      rx_platform::runtime::items::security_context_holder identity_;

      rx_platform::meta::runtime_data::port_data data_;

      rx_reference<rx_platform::runtime::items::port_runtime> implementation_;

      rx_reference<rx_platform::security::security_context> security_ctx_;


      rx_thread_handle_t executer_;

      bool mine_security_;


};







struct ports_auto_lock 
{

      ports_auto_lock (runtime_core::runtime_data::port_instance_data* data);

      ~ports_auto_lock();

  public:

  protected:

  private:


      port_instance_data* data_;


};


} // namespace runtime_data
} // namespace runtime_core
} // namespace sys_runtime
} // namespace rx_internal



#endif
