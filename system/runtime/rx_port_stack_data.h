

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
    typedef std::map<addr_pair_t, rx_port_ptr> passive_map_type;
    typedef std::map<rx_port_ptr, addr_pair_t> inverse_passive_map_type;

  public:
      port_passive_map();


      rx_result register_passive (rx_port_ptr who, io::any_address& local_addr, io::any_address& remote_addr, rx_port_ptr owner);

      rx_result unregister_passive (rx_port_ptr who, rx_port_ptr owner);

      rx_port_ptr get_binded_port (const io::any_address& local_addr, const io::any_address& remote_addr);

      std::vector<rx_port_ptr> get_binded ();

      bool empty () const;


      bool stack_binded;

      rx_port_ptr bind_port;


  protected:

  private:


      passive_map_type passive_map_;

      inverse_passive_map_type inverse_passive_map_;

      locks::slim_lock map_lock_;


};






class port_active_map 
{
    typedef std::map<rx_protocol_stack_endpoint*, rx_port_ptr> endpoints_map_type;

  public:

      rx_result register_endpoint (rx_protocol_stack_endpoint* what, rx_port_ptr whose, rx_port_ptr owner);

      rx_result unregister_endpoint (rx_protocol_stack_endpoint* what, rx_port_ptr owner);

      void close_all_endpoints ();


  protected:

  private:


      endpoints_map_type endpoints_map_;

      endpoints_map_type upper_endpoints_map_;

      locks::slim_lock map_lock_;


};






class port_build_map 
{
    typedef std::set<rx_port_ptr> registered_type;

  public:
      port_build_map();


      rx_result register_port (rx_port_ptr who, rx_port_ptr owner);

      rx_result unregister_port (rx_port_ptr who, rx_port_ptr owner);

      std::vector<rx_port_ptr> get_registered ();


      rx_port_ptr stack_top;

      bool stack_ready;


  protected:

  private:

      registered_type registered_;



};






class port_stack_data 
{

  public:

      port_passive_map passive_map;

      port_active_map active_map;

      port_build_map build_map;


  protected:

  private:


};


} // namespace io_types
} // namespace runtime
} // namespace rx_platform



#endif
