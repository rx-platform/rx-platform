

/****************************************************************************
*
*  system\runtime\rx_resolvers.h
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


#ifndef rx_resolvers_h
#define rx_resolvers_h 1


#include "system/runtime/rx_runtime_helpers.h"
#include "system/server/rx_ns.h"




namespace rx_platform {

namespace runtime {

namespace resolvers {





class port_resolver_user 
{

  public:

      virtual bool port_connected (rx_port_impl_ptr port, rx_node_id id) = 0;

      virtual void port_disconnected () = 0;

      virtual rx_reference_ptr get_reference () = 0;

      virtual ~port_resolver_user() = default;
  protected:

  private:


};






class runtime_subscriber 
{

  public:

      virtual void runtime_appeared (platform_item_ptr&& item) = 0;

      virtual void runtime_destroyed (const rx_node_id& id) = 0;

      virtual rx_reference_ptr get_reference () = 0;

      virtual ~runtime_subscriber() = default;
  protected:

  private:


};






class item_port_resolver : public runtime_subscriber  
{
    enum class resolver_state
    {
        idle = 0,
        waiting = 1,
        querying = 2,
        same_thread = 3,
        other_thread = 4,
        stopped = 5
    };
    struct resolve_result
    {
        rx_port_impl_ptr port;
        rx_node_id id;
        operator bool() const
        {
            return port;
        }
    };
    resolver_state my_state_ = resolver_state::idle;

  public:

      rx_result init (const rx_item_reference& ref, port_resolver_user* user, ns::rx_directory_resolver* dirs);

      void deinit ();

      void runtime_appeared (platform_item_ptr&& item);

      void runtime_destroyed (const rx_node_id& id);

      rx_reference_ptr get_reference ();


  protected:

  private:


      port_resolver_user *user_;


      ns::rx_directory_resolver* directories_;

      rx_item_reference port_reference_;

      rx_node_id resolved_id_;


};


} // namespace resolvers
} // namespace runtime
} // namespace rx_platform



#endif
