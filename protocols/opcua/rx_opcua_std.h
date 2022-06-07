

/****************************************************************************
*
*  protocols\opcua\rx_opcua_std.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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



namespace protocols {

namespace opcua {

namespace opcua_addr_space {
class opcua_std_address_space;
rx_result build_standard_address_space_UANodeSet2(opcua_std_address_space& server);





class opcua_std_node : public opcua_node_base  
{
    typedef std::bitset<8> bit_data_type;

    // bit indexes in bit_data
    static constexpr int abstract_idx = 0;
    static constexpr int symmetric_idx = 1;
    static constexpr int no_loops_idx = 2;
    static constexpr int historizing_idx = 3;
    static constexpr int executable_idx = 4;
    static constexpr int user_executable_idx = 5;

  public:
      opcua_std_node();

      opcua_std_node (uint32_t id);


      void read_attribute (attribute_id id, const string_type& range, const string_type& encoding, data_value& value, const rx_time& config_ts) const;

      rx_node_id get_node_id () const;


      uint32_t node_id;

      ~opcua_std_node() = default;
      opcua_std_node(const opcua_std_node&) = default;
      opcua_std_node(opcua_std_node&&) noexcept = default;
      opcua_std_node& operator=(const opcua_std_node&) = default;
      opcua_std_node& operator=(opcua_std_node&&) noexcept = default;
  protected:

  private:

      node_class_type node_class;

      const char* name;

      const char* inverse_name;

      bit_data_type bit_data;


      friend rx_result build_standard_address_space_UANodeSet2(opcua_std_address_space& server);

    friend class opcua_std_address_space;
};







class opcua_std_address_space : public opcua_address_space_base  
{
    typedef std::vector<opcua_std_node> registered_nodes_type;


  public:
      opcua_std_address_space();

      ~opcua_std_address_space();


      rx_result register_node (opcua_node_base* what);

      rx_result unregister_node (opcua_node_base* what);

      void read_attributes (const std::vector<read_value_id>& to_read, std::vector<data_value>& values) const;


  protected:

  private:

      opcua_std_address_space::registered_nodes_type::const_iterator get_by_id (uint32_t id) const;

      opcua_std_address_space::registered_nodes_type::iterator get_by_id (uint32_t id);



      locks::slim_lock ns_lock_;

      registered_nodes_type registered_nodes_;

      rx_time config_ts_;

      friend rx_result build_standard_address_space_UANodeSet2(opcua_std_address_space& server);

};


} // namespace opcua_addr_space
} // namespace opcua
} // namespace protocols



#endif
