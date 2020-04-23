

/****************************************************************************
*
*  system\runtime\rx_runtime_instance.h
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


#ifndef rx_runtime_instance_h
#define rx_runtime_instance_h 1


#include "rx_runtime_helpers.h"


// rx_identity
#include "system/server/rx_identity.h"

namespace rx_platform {
namespace runtime {
namespace items {
class application_instance_data;
class domain_instance_data;

} // namespace items
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {
enum class rx_domain_priority : uint8_t
{
    low = 0,
    normal = 1,
    high = 2,
    realtime = 3,
    // default value
    standard = 1,
    priority_count = 4
};

namespace runtime {

namespace items {





class object_instance_data 
{

  public:
      object_instance_data();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      bool connect_domain (rx_domain_ptr&& domain, rx_object_ptr whose);

      bool disconnect_domain (rx_object_ptr whose);

      static rx_result before_init_runtime (rx_object_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_object_ptr what, runtime::runtime_start_context& ctx, operational::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_object_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_object_ptr what, runtime::runtime_stop_context& ctx);

      const security::security_context_ptr& get_security_context () const;


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }



      rx_node_id domain_id;


  protected:

      rx_domain_ptr my_domain_;


  private:


      rx_thread_handle_t executer_;


};






class domain_instance_data 
{
    typedef std::map<rx_node_id, rx_object_ptr> objects_type;

  public:
      domain_instance_data();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void get_objects (api::query_result& result);

      void add_object (rx_object_ptr what);

      void remove_object (rx_object_ptr what);

      bool connect_application (rx_application_ptr&& app, rx_domain_ptr whose);

      bool disconnect_application (rx_domain_ptr whose);

      static rx_result before_init_runtime (rx_domain_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_domain_ptr what, runtime::runtime_start_context& ctx, operational::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_domain_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_domain_ptr what, runtime::runtime_stop_context& ctx);

      const security::security_context_ptr& get_security_context () const;


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }



      rx_node_id app_id;

      int processor;

      rx_node_ids objects;

      rx_domain_priority priority;


  protected:

  private:


      objects_type objects_;

      rx_application_ptr my_application_;


      rx_thread_handle_t executer_;


};






class port_instance_data 
{

  public:
      port_instance_data();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      bool connect_application (rx_application_ptr&& app, rx_port_ptr whose);

      bool disconnect_application (rx_port_ptr whose);

      static rx_result before_init_runtime (rx_port_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_port_ptr what, runtime::runtime_start_context& ctx, operational::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_port_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_port_ptr what, runtime::runtime_stop_context& ctx);

      const security::security_context_ptr& get_security_context () const;


      const rx_application_ptr get_my_application () const;


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }



      rx_node_id app_id;

      rx_item_reference up_port;

      ~port_instance_data() = default;
      port_instance_data(port_instance_data&& right);
      port_instance_data(const port_instance_data& right);
  protected:

  private:


      rx_application_ptr my_application_;

      security_context_holder identity_;


      rx_thread_handle_t executer_;


};






class application_instance_data 
{
    typedef std::map<rx_node_id, rx_domain_ptr> domains_type;
    typedef std::map<rx_node_id, rx_port_ptr> ports_type;

  public:
      application_instance_data();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void get_ports (api::query_result& result);

      void add_port (rx_port_ptr what);

      void add_domain (rx_domain_ptr what);

      void remove_port (rx_port_ptr what);

      void remove_domain (rx_domain_ptr what);

      void get_domains (api::query_result& result);

      static rx_result before_init_runtime (rx_application_ptr what, runtime::runtime_init_context& ctx);

      static rx_result before_start_runtime (rx_application_ptr what, runtime::runtime_start_context& ctx, operational::binded_tags* binded);

      static rx_result after_deinit_runtime (rx_application_ptr what, runtime::runtime_deinit_context& ctx);

      static rx_result after_stop_runtime (rx_application_ptr what, runtime::runtime_stop_context& ctx);

      const security::security_context_ptr& get_security_context () const;


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }



      int processor;

      rx_domain_priority priority;

      ~application_instance_data() = default;
      application_instance_data(application_instance_data&& right);
      application_instance_data(const application_instance_data& right);
  protected:

  private:


      domains_type domains_;

      ports_type ports_;

      security_context_holder identity_;


      locks::slim_lock domains_lock_;

      rx_thread_handle_t executer_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
