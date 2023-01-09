

/****************************************************************************
*
*  protocols\opcua\rx_opcua_std.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_opcua_std_h
#define rx_opcua_std_h 1



// rx_opcua_addr_space
#include "protocols/opcua/rx_opcua_addr_space.h"

#include "lib/rx_const_size_vector.h"
#include "lib/rx_values.h"


namespace protocols {

namespace opcua {

namespace opcua_addr_space {
enum class std_address_space_type
{
    basic_server    = 1,
    full            = 255
};

class opcua_std_address_space;
class opcua_std_address_space_builder;

struct reference_data_argument_t
{
    const opcua_std_ref_t* data{ nullptr };
    const uint32_t size{ 0 };
};
struct opcua_std_node_argument_t
{
    const char* name;
    const node_class_type class_type;
    const uint32_t id;
    const reference_data_argument_t references;
    const reference_data_argument_t inverse_references;
    const uint32_t type_id{ 0 };
    const uint16_t bit_options{ 0 };
    const char* inverse_name{ nullptr };

};




class opcua_std_node : public opcua_node_base  
{
protected:
    typedef std::bitset<16> bit_data_type;

    // bit indexes in bit_data
    static constexpr int abstract_idx = 0;
    static constexpr int symmetric_idx = 1;
    static constexpr int no_loops_idx = 2;
    static constexpr int historizing_idx = 3;
    static constexpr int executable_idx = 4;
    static constexpr int user_executable_idx = 5;

    static constexpr int events_subscribe_idx = 6;
    static constexpr int events_history_read_idx = 7;
    static constexpr int events_history_write_idx = 8;


  public:
      opcua_std_node();

      opcua_std_node (uint32_t id);

      opcua_std_node (const opcua_std_node_argument_t& arg);


      void read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;

      std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep);

      void browse (const opcua_browse_description& to_browse, browse_result_internal& result, opcua_browse_context* ctx) const;

      void translate (const relative_path& path, browse_path_result& results, opcua_browse_context* ctx) const;

      node_class_type get_node_class () const;

      rx_node_id get_node_id () const;

      qualified_name get_browse_name () const;

      string_type get_display_name () const;

      rx_node_id get_type_id () const;

      rx_result set_node_value (values::rx_value&& val);

      node_references& get_reference_data ();


      uint32_t node_id;

      ~opcua_std_node() = default;
      opcua_std_node(const opcua_std_node&) = default;
      opcua_std_node(opcua_std_node&&) noexcept = default;
      opcua_std_node& operator=(const opcua_std_node&) = default;
      opcua_std_node& operator=(opcua_std_node&&) noexcept = default;
  protected:

      node_class_type node_class;

      bit_data_type bit_data;


  private:

      const char* name;

      const char* inverse_name;

      uint32_t type_id;



      node_references references_;

      friend class opcua_std_address_space_builder;
      friend rx_result build_standard_address_space(opcua_std_address_space & server, const string_type & server_uri, const string_type & app_uri, const string_type& server_type);
    friend class opcua_std_address_space;
};


struct opcua_std_valued_node_argument_t
{
    opcua_std_node_argument_t base_data;
    uint32_t data_type_id;
    int value_rank;
    std::initializer_list<uint32_t> dimensions;
};




class opcua_std_valued_node : public opcua_std_node  
{

  public:
      opcua_std_valued_node();

      opcua_std_valued_node (opcua_std_valued_node_argument_t arg);


      void read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value_storage, const rx_time& config_ts) const;

      rx_result set_node_value (values::rx_value&& val);


  protected:

  private:

      uint32_t data_type_id;

      data_value value;

      int value_rank;

      const_size_vector<uint32_t> dimensions;


      friend class opcua_std_address_space_builder;
     // friend rx_result build_standard_address_space_UANodeSet2(opcua_std_address_space& server);
};







class opcua_std_address_space : public opcua_address_space_base  
{
    typedef std::vector<opcua_std_node> registered_nodes_type;
    typedef std::vector<opcua_std_valued_node> registered_valued_nodes_type;
    typedef std::map<uint32_t, std::set<uint32_t> > references_tree_type;

  public:
      opcua_std_address_space();

      ~opcua_std_address_space();


      void read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const;

      std::pair<opcua_result_t, runtime_transaction_id_t> write_attribute (const rx_node_id& node_id, attribute_id id, const string_type& range, const data_value& value, opcua_server_endpoint_ptr ep);

      void browse (const opcua_view_description& view, const std::vector<opcua_browse_description>& to_browse, std::vector<browse_result_internal>& results) const;

      void translate (const std::vector<browse_path>& browse_paths, std::vector<browse_path_result>& results, opcua_address_space_base* root) const;

      rx_result fill_relation_types (const rx_node_id& base_id, bool include_subtypes, std::set<rx_node_id>& buffer) const;

      rx_result set_node_value (const rx_node_id& id, values::rx_value&& val);

      locks::rw_slim_lock* get_lock ();

      const locks::rw_slim_lock* get_lock () const;

      opcua_node_base* connect_node_reference (opcua_node_base* node, const reference_data& ref_data, bool inverse);

      opcua_result_t register_value_monitor (opcua_subscriptions::opcua_monitored_value* who, data_value& val);

      opcua_result_t unregister_value_monitor (opcua_subscriptions::opcua_monitored_value* who);


  protected:

  private:

      opcua_std_address_space::registered_nodes_type::const_iterator get_by_id (uint32_t id) const;

      opcua_std_address_space::registered_nodes_type::iterator get_by_id (uint32_t id);

      opcua_std_address_space::registered_valued_nodes_type::const_iterator get_valued_by_id (uint32_t id) const;

      opcua_std_address_space::registered_valued_nodes_type::iterator get_valued_by_id (uint32_t id);



      locks::rw_slim_lock ns_lock_;

      registered_nodes_type registered_nodes_;

      registered_valued_nodes_type registered_valued_nodes_;

      rx_time config_ts_;

      references_tree_type references_tree_;

      friend class opcua_std_address_space_builder;
      friend rx_result build_standard_address_space(opcua_std_address_space& server, const string_type& server_uri, const string_type& app_uri, const string_type& server_type);
};


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols



#endif
