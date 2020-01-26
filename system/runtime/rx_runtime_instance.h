

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


namespace rx_platform {
namespace runtime {
namespace items {
class application_instance_data;
class domain_instance_data;

} // namespace items
} // namespace runtime
} // namespace rx_platform




namespace rx_platform {

namespace runtime {

namespace items {





class object_instance_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      bool connect_domain (rx_domain_ptr&& domain, const rx_object_ptr& whose);


      rx_node_id domain_id;


  protected:

      rx_domain_ptr my_domain_;


  private:


};






class domain_instance_data 
{
    typedef std::map<rx_node_id, rx_object_ptr> objects_type;

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void get_objects (api::query_result& result);

      void add_object (rx_object_ptr what);

      void remove_object (rx_object_ptr what);

      bool connect_application (rx_application_ptr&& app, const rx_domain_ptr& whose);


      rx_node_id app_id;

      int processor;

      rx_node_ids objects;


  protected:

  private:


      objects_type objects_;

      rx_application_ptr my_application_;


};






class port_instance_data 
{

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void connect_application (rx_application_ptr&& app, const rx_port_ptr& whose);


      rx_node_id app_id;

      rx_item_reference up_port;


  protected:

  private:


      rx_application_ptr my_application_;


};






class application_instance_data 
{
    typedef std::vector<rx_domain_ptr> domains_type;
    typedef std::vector<rx_port_ptr> ports_type;

  public:

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      void get_ports (api::query_result& result);

      void add_port (rx_port_ptr what);

      void add_domain (rx_domain_ptr what);

      void remove_port (rx_port_ptr what);

      void remove_domain (rx_domain_ptr what);

      void get_domains (api::query_result& result);


      int processor;


  protected:

  private:


      domains_type domains_;

      ports_type ports_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
