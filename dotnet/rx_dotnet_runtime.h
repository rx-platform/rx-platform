

/****************************************************************************
*
*  dotnet\rx_dotnet_runtime.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


#ifndef rx_dotnet_runtime_h
#define rx_dotnet_runtime_h 1



// rx_relation_impl
#include "system/runtime/rx_relation_impl.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_internal {
namespace dotnet {
class dotnet_relation;
class dotnet_method;

} // namespace dotnet
} // namespace rx_internal


#include "system/runtime/rx_value_templates.h"
#include "rx_dotnet_version.h"
#include "dotnet_api.h"
using namespace rx_platform::runtime;


namespace rx_internal {

namespace dotnet {
struct binded_value_data
{
    runtime::async_complex_value value;
    data::runtime_data_model model;
};
struct execute_value_data
{
    runtime::owned_complex_value value;
    data::runtime_data_model model;
};

typedef std::vector<std::unique_ptr<binded_value_data> > binded_values_type;
typedef std::vector< std::unique_ptr<runtime::async_complex_value > > relation_values_type;
typedef std::vector< std::unique_ptr<execute_value_data> > method_values_type;






class dotnet_object_runtime : public rx_platform::runtime::items::object_runtime  
{
//    DECLARE_CODE_INFO("rx", RX_DOTNET_MAJOR_VERSION, RX_DOTNET_MINOR_VERSION, RX_DOTNET_BUILD_NUMBER, "\
//dotnet Core object runtime implementation");

    DECLARE_REFERENCE_PTR(dotnet_object_runtime);


    struct execute_transaction_data
    {
        rx_reference<dotnet_method> method;
	};
	typedef std::map<uint32_t, execute_transaction_data> execute_transactions_type;
	typedef std::unique_ptr<std::map<string_type, rx_reference<dotnet_relation> > > my_relations_type;

  public:
      dotnet_object_runtime (string_type conn_data, rx_node_id type_id, dotnet_code_info code_info);

      ~dotnet_object_runtime();


      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name) const;

      void post_own_job (plugin_job_struct* what);

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);

      void received_execute_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_simple_value&&, rx_result&&)> callback);

      rx_result execute_method (uint32_t trans_id, const string_type& method, const string_type& value, rx_reference<dotnet_method> whose);

      void received_execute_done (uint32_t transId, const string_type& val, rx_result result);

      void register_relation (const string_type& name, rx_reference<dotnet_relation> who);


      static std::map<rx_node_id, dotnet_object_runtime::smart_ptr> runtime_instances;

      static locks::slim_lock instances_lock;


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);



      my_relations_type my_relations_;


      string_array conn_data_;

      string_array init_data_;

      string_array rel_data_;

      string_array method_data_;

      binded_values_type binded_values_;

      relation_values_type relation_values_;

      method_values_type method_values_;

      execute_transactions_type execute_transactions_;

      locks::slim_lock execute_transactions_lock_;

      rx_node_id type_id_;

      dotnet_code_info code_info_;


};






class dotnet_method : public rx_platform::logic::method_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
.NET hosting method.");

    DECLARE_REFERENCE_PTR(dotnet_method)

  public:
      dotnet_method();

      ~dotnet_method();


      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      rx_result execute (execute_data data, runtime::runtime_process_context* ctx);

      void send_execute_result (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id);

      void send_execute_result (data::runtime_values_data out_val, rx_result&& result, runtime_transaction_id_t id);

      void received_execute_done (uint32_t transId, const string_type& val, rx_result result);


  protected:

  private:


      rx_reference<dotnet_object_runtime> my_object_;


      data::runtime_data_model inputs_;

      data::runtime_data_model outputs_;

      string_type method_name_;


};






class dotnet_relation : public rx_platform::runtime::relations::relation_runtime  
{
    DECLARE_REFERENCE_PTR(dotnet_relation);

  public:
      dotnet_relation();

      ~dotnet_relation();


      rx_result initialize_relation (runtime::runtime_init_context& ctx);

      rx_result deinitialize_relation (runtime::runtime_deinit_context& ctx);

      rx_result start_relation (runtime::runtime_start_context& ctx, bool is_target);

      rx_result stop_relation (runtime::runtime_stop_context& ctx, bool is_target);

      void set_index (size_t index);


  protected:

  private:

      rx_result_with<platform_item_ptr> resolve_runtime_sync (const rx_node_id& id);

      void relation_connected ();

      void relation_disconnected ();



      rx_reference<dotnet_object_runtime> my_object_;

      rx_reference<dotnet_object_runtime> other_object_;


      string_type relation_name_;

      size_t index_;


};






class dotnet_event : public rx_platform::runtime::blocks::event_runtime  
{
    DECLARE_REFERENCE_PTR(dotnet_event)

  public:
      dotnet_event (string_type conn_data, rx_node_id type_id);

      ~dotnet_event();


      rx_result initialize_event (runtime::runtime_init_context& ctx);

      rx_result start_event (runtime::runtime_start_context& ctx);

      rx_result stop_event (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_event (runtime::runtime_deinit_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name) const;

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);

      void fire_event (string_type data, string_type queue, bool state, bool remove);


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);



      string_type conn_data_;

      binded_values_type binded_values_;

      rx_node_id type_id_;

      rx_node_id object_id_;

      string_type path_;


};






class dotnet_mapper : public rx_platform::runtime::blocks::mapper_runtime  
{
    DECLARE_REFERENCE_PTR(dotnet_mapper)

  public:
      dotnet_mapper (string_type conn_data, rx_node_id type_id);

      ~dotnet_mapper();


      rx_result initialize_mapper (runtime::runtime_init_context& ctx);

      rx_result start_mapper (runtime::runtime_start_context& ctx);

      rx_result stop_mapper (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_mapper (runtime::runtime_deinit_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name) const;

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);



      string_type conn_data_;

      binded_values_type binded_values_;

      rx_node_id type_id_;

      rx_node_id object_id_;

      string_type path_;


};






class dotnet_source : public rx_platform::runtime::blocks::source_runtime  
{
    DECLARE_REFERENCE_PTR(dotnet_source)

  public:
      dotnet_source (string_type conn_data, rx_node_id type_id);

      ~dotnet_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      rx_result start_source (runtime::runtime_start_context& ctx);

      rx_result stop_source (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_source (runtime::runtime_deinit_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name) const;

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);

      void source_changed (rx_value&& val);

      void source_result (rx_result&& result, runtime_transaction_id_t id);

      void source_execute_result (rx_simple_value out_val, rx_result&& result, runtime_transaction_id_t id);


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);

      rx_result source_write (write_data&& data, runtime_process_context* ctx);

      rx_result source_execute (execute_data&& data, runtime_process_context* ctx);



      string_type conn_data_;

      binded_values_type binded_values_;

      rx_node_id type_id_;

      rx_node_id object_id_;

      string_type path_;


};






class dotnet_struct : public rx_platform::runtime::blocks::struct_runtime  
{
    DECLARE_REFERENCE_PTR(dotnet_struct)

  public:
      dotnet_struct (string_type conn_data, rx_node_id type_id);

      ~dotnet_struct();


      rx_result initialize_struct (runtime::runtime_init_context& ctx);

      rx_result start_struct (runtime::runtime_start_context& ctx);

      rx_result stop_struct (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_struct (runtime::runtime_deinit_context& ctx);

      void fill_code_info (std::ostream& info, const string_type& name) const;

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);



      binded_values_type binded_values_;

      rx_node_id type_id_;

      rx_node_id object_id_;

      string_type path_;

      string_array conn_data_;

      string_array init_data_;


};






class dotnet_appliacation : public rx_platform::runtime::items::application_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
dotnet application class.");

    DECLARE_REFERENCE_PTR(dotnet_appliacation);

  public:
      dotnet_appliacation();

      ~dotnet_appliacation();


      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);


  protected:

  private:


};






class dotnet_domain : public rx_platform::runtime::items::domain_runtime  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
dotnet domain class.");

    DECLARE_REFERENCE_PTR(dotnet_domain);

  public:
      dotnet_domain();

      ~dotnet_domain();


      rx_result initialize_runtime (runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime_deinit_context& ctx);

      rx_result start_runtime (runtime_start_context& ctx);

      rx_result stop_runtime (runtime_stop_context& ctx);


  protected:

  private:


};






class dotnet_monitored_struct : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(dotnet_monitored_struct)

  public:
      dotnet_monitored_struct (string_type conn_data, rx_node_id type_id);

      ~dotnet_monitored_struct();


      rx_result initialize_struct (runtime::runtime_init_context& ctx);

      rx_result start_struct (runtime::runtime_start_context& ctx);

      rx_result stop_struct (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_struct (runtime::runtime_deinit_context& ctx);

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);



      rx_reference<dotnet_domain> domain_;


      binded_values_type binded_values_;

      rx_node_id type_id_;

      rx_node_id object_id_;

      string_type path_;

      string_array conn_data_;

      string_array init_data_;


};






class dotnet_monitored_object : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(dotnet_monitored_object)

  public:
      dotnet_monitored_object (string_type conn_data, rx_node_id type_id);

      ~dotnet_monitored_object();


      rx_result initialize_object (runtime::runtime_init_context& ctx);

      rx_result start_object (runtime::runtime_start_context& ctx);

      rx_result stop_object (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_object (runtime::runtime_deinit_context& ctx);

      void received_write_value (uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);


  protected:

  private:

      void binded_value_changed (int idx, const values::rx_simple_value& val);



      rx_reference<dotnet_domain> domain_;


      binded_values_type binded_values_;

      rx_node_id type_id_;

      rx_node_id object_id_;

      string_type path_;

      string_array conn_data_;

      string_array init_data_;


};


} // namespace dotnet
} // namespace rx_internal



#endif
