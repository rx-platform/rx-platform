

/****************************************************************************
*
*  runtime_internal\rx_runtime_instance.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_runtime_instance_h
#define rx_runtime_instance_h 1


#include "system/runtime/rx_runtime_helpers.h"
#include "lib/rx_io_addr.h"

// rx_runtime_data
#include "system/meta/rx_runtime_data.h"
// rx_identity
#include "system/server/rx_identity.h"
// rx_port_instance
#include "interfaces/rx_port_instance.h"
// rx_security
#include "lib/security/rx_security.h"

namespace rx_internal {
namespace sys_runtime {
namespace runtime_core {
namespace runtime_data {
class application_instance_data;
class domain_instance_data;

} // namespace runtime_data
} // namespace runtime_core
} // namespace sys_runtime
} // namespace rx_internal


using namespace rx_platform::meta::runtime_data;


namespace rx_internal {

namespace sys_runtime {

namespace runtime_core {

namespace runtime_data {





class object_instance_data 
{

  public:
      object_instance_data (const object_data& data, int rt_behavior);


      bool connect_domain (rx_domain_ptr&& domain, rx_object_ptr whose);

      bool disconnect_domain (rx_object_ptr whose);

      static rx_result before_init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_object_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx);

      security::security_context_ptr get_security_context () const;


      const rx_platform::meta::runtime_data::object_data& get_data () const
      {
        return data_;
      }



      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }


      object_instance_data(object_instance_data&& right) noexcept = default;
      object_instance_data(object_instance_data& right) = default;
  protected:

      rx_domain_ptr my_domain_;


  private:


      rx_platform::meta::runtime_data::object_data data_;


      rx_thread_handle_t executer_;


};






class domain_instance_data 
{
    typedef std::map<rx_node_id, rx_object_ptr> objects_type;

  public:
      domain_instance_data (const domain_data& data, int rt_behavior);


      void get_objects (api::query_result& result);

      void add_object (rx_object_ptr what);

      void remove_object (rx_object_ptr what);

      bool connect_application (rx_application_ptr&& app, rx_domain_ptr whose);

      bool disconnect_application (rx_domain_ptr whose);

      static rx_result before_init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_domain_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx);

      security::security_context_ptr get_security_context () const;

      std::vector<rx_object_ptr> get_objects ();


      const rx_platform::meta::runtime_data::domain_data& get_data () const
      {
        return data_;
      }



      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }



      rx_node_ids objects;

      domain_instance_data(domain_instance_data&& right) noexcept = default;
      domain_instance_data(const domain_instance_data& right) = default;
  protected:

  private:


      objects_type objects_;

      rx_application_ptr my_application_;

      rx_platform::meta::runtime_data::domain_data data_;


      rx_thread_handle_t executer_;


};






class application_instance_data 
{
    typedef std::map<rx_node_id, rx_domain_ptr> domains_type;
    typedef std::map<rx_node_id, rx_port_ptr> ports_type;

  public:
      application_instance_data (const application_data& data, int rt_behavior);


      void get_ports (api::query_result& result, bool extern_only);

      void add_port (rx_port_ptr what);

      void add_domain (rx_domain_ptr what);

      void remove_port (rx_port_ptr what);

      void remove_domain (rx_domain_ptr what);

      void get_domains (api::query_result& result);

      static rx_result before_init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx, tag_blocks::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_application_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx);

      security::security_context_ptr get_security_context () const;

      std::vector<rx_domain_ptr> get_domains ();

      std::vector<rx_port_ptr> get_ports ();


      const rx_platform::meta::runtime_data::application_data& get_data () const
      {
        return data_;
      }



      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }


      application_instance_data(application_instance_data&& right) noexcept = default;
      application_instance_data(const application_instance_data& right) = default;
  protected:

  private:


      domains_type domains_;

      ports_type ports_;

      rx_platform::runtime::items::security_context_holder identity_;

      rx_platform::meta::runtime_data::application_data data_;

      rx_reference<rx::security::security_context> security_ctx_;


      rx_thread_handle_t executer_;

      locks::slim_lock domains_lock_;


};


} // namespace runtime_data
} // namespace runtime_core
} // namespace sys_runtime
} // namespace rx_internal



#endif
