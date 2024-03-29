

/****************************************************************************
*
*  protocols\opcua\rx_opcua_basic.h
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


#ifndef rx_opcua_basic_h
#define rx_opcua_basic_h 1



// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"
// rx_opcua_subscriptions
#include "protocols/opcua/rx_opcua_subscriptions.h"
// rx_opcua_std
#include "protocols/opcua/rx_opcua_std.h"
// rx_opcua_addr_space
#include "protocols/opcua/rx_opcua_addr_space.h"
// rx_opcua_server
#include "protocols/opcua/rx_opcua_server.h"

namespace protocols {
namespace opcua {
namespace opcua_basic_server {
class opcua_basic_method_mapper;
class opcua_basic_mapper;
class opcua_basic_server_port;

} // namespace opcua_basic_server
} // namespace opcua
} // namespace protocols


#include "rx_opcua_requests.h"
using namespace protocols::opcua::opcua_addr_space;


namespace protocols {

namespace opcua {

namespace opcua_basic_server {
constexpr const uint16_t default_basic_namespace = 3;






class opcua_basic_server_endpoint : public opcua_server_endpoint_base  
{
    DECLARE_REFERENCE_PTR(opcua_basic_server_endpoint);

  public:
      opcua_basic_server_endpoint (const application_description& app_description, const string_type& endpoint_url, const string_type& port_path, opcua_basic_server_port* port);

      ~opcua_basic_server_endpoint();


      void close_endpoint ();

      rx_result send_response (requests::opcua_response_ptr resp);


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      opcua_basic_server_port* get_port ()
      {
        return port_;
      }



      rx_protocol_stack_endpoint stack_entry;


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      rx_thread_handle_t executer_;

      opcua_basic_server_port* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< opcua_basic_server_endpoint  > opcua_basic_server_port_base;






class opcua_basic_node : public opcua_addr_space::opcua_variable_node  
{

  public:
      opcua_basic_node();

      ~opcua_basic_node();


      std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep);


      const string_type& get_path () const
      {
        return path_;
      }



  protected:

  private:


      rx_reference<opcua_basic_mapper> mapper_;


      string_type path_;


    friend class opcua_basic_mapper;
};






class opcua_basic_folder_node : public opcua_addr_space::opcua_object_node  
{

  public:
      opcua_basic_folder_node();

      ~opcua_basic_folder_node();


  protected:

  private:


};






class opcua_basic_method_node : public opcua_addr_space::opcua_method_node  
{

  public:
      opcua_basic_method_node();

      ~opcua_basic_method_node();


      std::pair<opcua_result_t, runtime_transaction_id_t> execute (std::vector<variant_type> args, opcua_server_endpoint_ptr ep);


      const string_type& get_path () const
      {
        return path_;
      }



  protected:

  private:


      rx_reference<opcua_basic_method_mapper> mapper_;


      string_type path_;


    friend class opcua_basic_method_mapper;
};






class opcua_simple_address_space : public opcua_addr_space::opcua_address_space_base  
{
    typedef std::map<rx_node_id, std::shared_ptr<opcua_basic_node> > variable_nodes_type;
    typedef std::map<rx_node_id, std::shared_ptr<opcua_method_node> >method_nodes_type;
    typedef std::map<rx_node_id, std::shared_ptr<opcua_basic_folder_node> > folder_nodes_type;
    typedef std::map<rx_node_id, std::shared_ptr<opcua_property_node> > argument_nodes_type;
    typedef std::map<rx_node_id, std::set<opcua_subscriptions::opcua_monitored_value*> > value_monitors_type;

  public:
      opcua_simple_address_space();


      void set_parent (opcua_addr_space::opcua_address_space_base* parent);

      rx_result register_node (std::shared_ptr<opcua_basic_node> what);

      rx_result unregister_node (std::shared_ptr<opcua_basic_node> what);

      rx_result register_node (std::shared_ptr<opcua_basic_method_node> what, std::shared_ptr<opcua_property_node> input_args, std::shared_ptr<opcua_property_node> output_args);

      rx_result unregister_node (std::shared_ptr<opcua_basic_method_node> what);

      void read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const;

      std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (const rx_node_id& node_id, attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep);

      std::pair<opcua_result_t, runtime_transaction_id_t> execute (const rx_node_id& node_id, const rx_node_id& method_id, std::vector<variant_type> args, opcua_server_endpoint_ptr ep);

      void browse (const opcua_view_description& view, const std::vector<opcua_browse_description>& to_browse, std::vector<browse_result_internal>& results) const;

      void translate (const std::vector<browse_path>& browse_paths, std::vector<browse_path_result>& results, opcua_address_space_base* root) const;

      rx_result fill_relation_types (const rx_node_id& base_id, bool include_subtypes, std::set<rx_node_id>& buffer) const;

      rx_result set_node_value (const rx_node_id& id, values::rx_value&& val);

      locks::rw_slim_lock* get_lock ();

      const locks::rw_slim_lock* get_lock () const;

      std::shared_ptr<opcua_node_base> connect_node_reference (std::shared_ptr<opcua_node_base> node, const opcua_addr_space::reference_data& ref_data, bool inverse);

      bool disconnect_node_reference (std::shared_ptr<opcua_node_base> node, const reference_data& ref_data, bool inverse);

      opcua_result_t register_value_monitor (opcua_subscriptions::opcua_monitored_value* who, data_value& val);

      opcua_result_t unregister_value_monitor (opcua_subscriptions::opcua_monitored_value* who);


  protected:

  private:

      rx_node_id get_folder_node (const string_type& folder_path, const rx_node_id& parent_id);

      std::shared_ptr<opcua_base_node_type> get_node (const rx_node_id& id);

      const std::shared_ptr<opcua_base_node_type> get_node (const rx_node_id& id) const;

      std::shared_ptr<opcua_variable_base_node> get_valued_node (const rx_node_id& id);



      opcua_addr_space::opcua_address_space_base *parent_;

      variable_nodes_type variable_nodes_;

      folder_nodes_type folder_nodes_;

      value_monitors_type value_monitors_;

      method_nodes_type method_nodes_;

      argument_nodes_type argument_nodes_;


      locks::slim_lock ns_lock_;

      rx_time config_ts_;


};






class opcua_basic_server_port : public opcua_basic_server_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 7, 0, "\
Basic OPC UA protocol port class. Basic implementation OPC UA binary protocol core with simple mapping.");

    DECLARE_REFERENCE_PTR(opcua_basic_server_port);

    class basic_server_resolver_user : public runtime::relation_subscriber
    {
    public:
        opcua_basic_server_port* my_port;
        void relation_connected(const string_type& name, const platform_item_ptr& item)
        {
            my_port->internal_port_connected(item);
        }
        void relation_disconnected(const string_type& name)
        {
            my_port->internal_port_disconnected();
        }
    };
    basic_server_resolver_user resolver_user_;
    friend class opcua_basic_server_port::basic_server_resolver_user;

  public:
      opcua_basic_server_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result register_node (std::shared_ptr<opcua_basic_node> node);

      rx_result unregister_node (std::shared_ptr<opcua_basic_node> node);

      rx_result register_node (std::shared_ptr<opcua_basic_method_node> node, std::shared_ptr<opcua_property_node> input_args, std::shared_ptr<opcua_property_node> output_args);

      rx_result unregister_node (std::shared_ptr<opcua_basic_method_node> node);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      opcua_addr_space::opcua_std_address_space std_address_space;

      opcua_simple_address_space address_space;

      opcua_subscriptions::opcua_subscriptions_collection subscriptions;

      rx_platform::runtime::io_types::simple_port_status status;


      static std::map<rx_node_id, opcua_basic_server_port::smart_ptr> runtime_instances;


  protected:

  private:

      bool internal_port_connected (const platform_item_ptr& item);

      void internal_port_disconnected ();



      application_description application_description_;

      string_type port_path_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_mapper_impl< protocols::opcua::opcua_basic_server::opcua_basic_server_port  > opcua_basic_mapper_base;


//typedef rx_platform::runtime::blocks::mapper_runtime opcua_basic_mapper_base;





class opcua_basic_mapper : public opcua_basic_mapper_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of OPC UA Basic Mapper");

    DECLARE_REFERENCE_PTR(opcua_basic_mapper);


    typedef std::map<runtime_transaction_id_t, opcua_server_endpoint_ptr> write_transactions_type;

  public:
      opcua_basic_mapper();

      ~opcua_basic_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      std::pair<opcua_result_t, runtime_transaction_id_t> write_value (const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep);


  protected:

  private:

      void mapped_value_changed (rx_value&& val, runtime::runtime_process_context* ctx);

      void mapper_result_received (rx_result&& result, runtime_transaction_id_t id, runtime::runtime_process_context* ctx);



      std::shared_ptr<opcua_basic_node> node_;


      locks::slim_lock transactions_lock_;

      write_transactions_type write_transactions_;


};







class opcua_basic_method_mapper : public opcua_basic_mapper_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of OPC UA Basic Method Mapper");

    DECLARE_REFERENCE_PTR(opcua_basic_method_mapper);


    typedef std::map<runtime_transaction_id_t, opcua_server_endpoint_ptr> execute_transactions_type;

  public:
      opcua_basic_method_mapper();

      ~opcua_basic_method_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      std::pair<opcua_result_t, runtime_transaction_id_t> execute (std::vector<variant_type> args, opcua_server_endpoint_ptr ep);


  protected:

  private:

      void mapper_execute_result_received (rx_result&& result, values::rx_simple_value out_data, runtime_transaction_id_t id, runtime::runtime_process_context* ctx);



      std::shared_ptr<opcua_basic_method_node> node_;

      std::shared_ptr<opcua_addr_space::opcua_property_node> outputs_node_;

      std::shared_ptr<opcua_addr_space::opcua_property_node> inputs_node_;


      locks::slim_lock transactions_lock_;

      execute_transactions_type execute_transactions_;


};


} // namespace opcua_basic_server
} // namespace opcua
} // namespace protocols



#endif
