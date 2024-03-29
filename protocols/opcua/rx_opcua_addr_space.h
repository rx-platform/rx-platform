

/****************************************************************************
*
*  protocols\opcua\rx_opcua_addr_space.h
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


#ifndef rx_opcua_addr_space_h
#define rx_opcua_addr_space_h 1


#include "lib/rx_lock.h"
#include "rx_opcua_params.h"
using namespace protocols::opcua::common;


namespace protocols {
namespace opcua {
namespace opcua_subscriptions {
class opcua_monitored_value;
} // namespace opcua_subscriptions

namespace opcua_addr_space {
class node_references;

} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols




namespace protocols {

namespace opcua {

namespace opcua_addr_space {
class opcua_address_space_base;

struct opcua_browse_context
{
    opcua_browse_context(const opcua_address_space_base* a)
        : root(a)
    {
    }
    const opcua_address_space_base* root;
};






class opcua_node_base 
{

  public:

      virtual void read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value, const rx_time& config_ts) const = 0;

      virtual std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep) = 0;

      virtual std::pair<opcua_result_t, runtime_transaction_id_t> execute (std::vector<variant_type> args, opcua_server_endpoint_ptr ep) = 0;

      virtual void browse (const opcua_browse_description& to_browse, browse_result_internal& result, opcua_browse_context* ctx) const = 0;

      virtual void translate (const relative_path& path, browse_path_result& results, opcua_browse_context* ctx) const = 0;

      virtual node_class_type get_node_class () const = 0;

      virtual rx_node_id get_node_id () const = 0;

      virtual qualified_name get_browse_name () const = 0;

      virtual string_type get_display_name () const = 0;

      virtual rx_node_id get_type_id () const = 0;

      virtual rx_result set_node_value (values::rx_value&& val) = 0;

      virtual node_references& get_reference_data () = 0;

      virtual ~opcua_node_base() = default;
  protected:

  private:


};


struct opcua_std_ref_t
{
    uint32_t ref_id;
    uint32_t target_id;
    uint32_t target_idx;
};




class reference_data 
{

  public:
      reference_data();

      reference_data (rx_node_id ref_id, rx_node_id targ_id);


      std::weak_ptr<opcua_node_base> resolved_node;


      rx_node_id reference_id;

      rx_node_id target_id;

      reference_data(const reference_data&) = default;
      reference_data(reference_data&&) noexcept = default;
      reference_data& operator=(const reference_data&) = default;
      reference_data& operator=(reference_data&&) noexcept = default;
      ~reference_data() = default;
  protected:

  private:


};







class opcua_address_space_base 
{

  public:

      virtual void read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const = 0;

      virtual std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (const rx_node_id& node_id, attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep) = 0;

      virtual std::pair<opcua_result_t, runtime_transaction_id_t> execute (const rx_node_id& node_id, const rx_node_id& method_id, std::vector<variant_type> args, opcua_server_endpoint_ptr ep) = 0;

      virtual void browse (const opcua_view_description& view, const std::vector<opcua_browse_description>& to_browse, std::vector<browse_result_internal>& results) const = 0;

      virtual void translate (const std::vector<browse_path>& browse_paths, std::vector<browse_path_result>& results, opcua_address_space_base* root) const = 0;

      virtual rx_result fill_relation_types (const rx_node_id& base_id, bool include_subtypes, std::set<rx_node_id>& buffer) const = 0;

      virtual rx_result set_node_value (const rx_node_id& id, values::rx_value&& val) = 0;

      virtual locks::rw_slim_lock* get_lock () = 0;

      virtual const locks::rw_slim_lock* get_lock () const = 0;

      virtual std::shared_ptr<opcua_node_base> connect_node_reference (std::shared_ptr<opcua_node_base> node, const reference_data& ref_data, bool inverse) = 0;

      virtual bool disconnect_node_reference (std::shared_ptr<opcua_node_base> node, const reference_data& ref_data, bool inverse) = 0;

      virtual opcua_result_t register_value_monitor (opcua_subscriptions::opcua_monitored_value* who, data_value& val) = 0;

      virtual opcua_result_t unregister_value_monitor (opcua_subscriptions::opcua_monitored_value* who) = 0;

      opcua_address_space_base() = default;
      virtual ~opcua_address_space_base() = default;
      // no move and no copy!
      opcua_address_space_base(const opcua_address_space_base&) = delete;
      opcua_address_space_base(opcua_address_space_base&&) = delete;
      opcua_address_space_base& operator=(const opcua_address_space_base&) = delete;
      opcua_address_space_base& operator=(opcua_address_space_base&&) = delete;
  protected:

  private:


};






class node_references 
{
  public:
        typedef std::list<reference_data> references_type;

  public:

      bool connect_node_reference (std::shared_ptr<opcua_node_base> node, const reference_data& ref_data, bool inverse);

      bool disconnect_node_reference (std::shared_ptr<opcua_node_base> node, const reference_data& ref_data, bool inverse);


      references_type references;

      references_type inverse_references;


  protected:

  private:


};






class opcua_base_node_type : public opcua_node_base  
{

  public:
      opcua_base_node_type (node_class_type node_cls);

      virtual ~opcua_base_node_type();


      void read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;

      std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep);

      std::pair<opcua_result_t, runtime_transaction_id_t> execute (std::vector<variant_type> args, opcua_server_endpoint_ptr ep);

      void browse (const opcua_browse_description& to_browse, browse_result_internal& result, opcua_browse_context* ctx) const;

      void translate (const relative_path& path, browse_path_result& results, opcua_browse_context* ctx) const;

      node_class_type get_node_class () const;

      rx_node_id get_node_id () const;

      qualified_name get_browse_name () const;

      string_type get_display_name () const;

      node_references& get_reference_data ();

      rx_result set_node_value (values::rx_value&& val);


      node_references references;


      rx_node_id node_id;

      qualified_name browse_name;

      string_type display_name;

      localized_text description;

      write_mask_type write_mask;

      write_mask_type user_write_mask;

      role_permissions_type role_permissions;

      role_permissions_type user_role_permissions;

      access_restrictions_type access_restrictions;


  protected:

      virtual void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;


      rx_time timestamp_;


  private:

      node_class_type node_class;



};

typedef std::unique_ptr<opcua_base_node_type> opcua_node_ptr;





class opcua_variable_base_node : public opcua_base_node_type  
{

  public:
      opcua_variable_base_node();


      rx_result set_node_value (values::rx_value&& val);


      rx_node_id data_type;

      data_value value;

      int32_t value_rank;

      std::vector<uint32_t> array_dimenstions;

      opc_access_level access_level;

      opc_access_level user_access_level;

      double sampling_interval;

      bool historizing;

      opc_access_level_ex access_level_ex;


  protected:

      void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;


  private:


};






class opcua_reference_type : public opcua_base_node_type  
{

  public:
      opcua_reference_type();


      bool is_abstract;

      bool symmetric;

      localized_text inverse_name;


  protected:

      void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;


  private:


};






class opcua_property_node : public opcua_variable_base_node  
{

  public:
      opcua_property_node();


      rx_node_id get_type_id () const;


  protected:

      void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;


  private:


};






class opcua_variable_node : public opcua_variable_base_node  
{

  public:
      opcua_variable_node();


      rx_node_id get_type_id () const;


      rx_node_id type_id;


  protected:

      void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts);


  private:


};






class opcua_object_node : public opcua_base_node_type  
{

  public:
      opcua_object_node();


      rx_node_id get_type_id () const;


      uint8_t event_notifier;

      rx_node_id type_id;


  protected:

      void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;


  private:


};






class opcua_method_node : public opcua_base_node_type  
{

  public:
      opcua_method_node();


      rx_node_id get_type_id () const;


      bool executable;

      bool user_executable;

      rx_node_id type_id;


  protected:

      void internal_read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;


  private:


};


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols



#endif
