

/****************************************************************************
*
*  system\runtime\rx_port_stack_data.h
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


#ifndef rx_port_stack_data_h
#define rx_port_stack_data_h 1


#include "lib/rx_io_addr.h"
#include "rx_runtime_helpers.h"
using namespace rx_platform;




namespace rx_platform {

namespace runtime {

namespace io_types {





class port_passive_map 
{
    typedef std::pair<io::any_address, io::any_address> addr_pair_t;
    struct registered_port_data
    {
        rx_port_ptr port;
        addr_pair_t passive_entry;
        bool binded;
        std::vector<rx_protocol_stack_endpoint*> endpoints;
    };
    typedef std::vector<registered_port_data> registered_type;
    typedef std::map<addr_pair_t, rx_port_ptr> passive_map_type;

  public:

      rx_result register_port (rx_port_ptr who, rx_port_ptr owner);

      rx_result register_passive (rx_port_ptr who, const io::any_address& local_addr, const io::any_address& remote_addr, rx_port_ptr owner);

      rx_result unregister_passive (rx_port_ptr who, rx_port_ptr owner);

      rx_result unregister_port (rx_port_ptr who, rx_port_ptr owner);

      rx_port_ptr get_binded_port (const io::any_address& local_addr, const io::any_address& remote_addr);

      std::vector<rx_port_ptr> get_registered ();

      std::vector<rx_port_ptr> get_binded ();


  protected:

  private:

      registered_type registered_;



      passive_map_type passive_map_;


};






class port_stack_data 
{

  public:

      port_passive_map passive_map;


      rx_port_ptr stack_top;

      bool stack_ready;


  protected:

  private:


};


} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
