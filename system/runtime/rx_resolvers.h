

/****************************************************************************
*
*  system\runtime\rx_resolvers.h
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


#ifndef rx_resolvers_h
#define rx_resolvers_h 1


#include "system/runtime/rx_runtime_helpers.h"
#include "system/server/rx_ns.h"
#include "system/meta/rx_obj_types.h"




namespace rx_platform {

namespace runtime {

namespace resolvers {
enum class resolver_state : uint_fast8_t
{
    idle = 0,
    waiting = 1,
    querying = 2,
    resolved = 3,
    stopped = 4
};





template <class typeT>
class resolver_user 
{

  public:

      virtual bool runtime_connected (platform_item_ptr&& item, typename typeT::RImplPtr implementation) = 0;

      virtual void runtime_disconnected () = 0;

      virtual rx_reference_ptr get_reference () = 0;

      virtual ~resolver_user() = default;
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






template <class typeT>
class runtime_resolver : public runtime_subscriber  
{
    struct resolve_result
    {
        typename typeT::RImplPtr implementation;
        platform_item_ptr item;
        operator bool() const
        {
            return implementation;
        }
        resolve_result() = default;
        resolve_result(resolve_result&&) = default;
        resolve_result& operator=(resolve_result&&) = default;
    };
    resolver_state my_state_ = resolver_state::idle;

  public:

      rx_result start_resolver (const rx_item_reference& ref, resolver_user<typeT>* user, ns::rx_directory_resolver* dirs);

      void stop_resolver ();

      void runtime_appeared (platform_item_ptr&& item);

      void runtime_destroyed (const rx_node_id& id);

      rx_reference_ptr get_reference ();

      typedef resolver_user<typeT> resolver_user_type;
  protected:

  private:


      resolver_user<typeT>* user_;


      ns::rx_directory_resolver* directories_;

      rx_item_reference runtime_reference_;

      rx_node_id resolved_id_;


};






class item_resolver_user 
{

  public:

      virtual bool runtime_connected (platform_item_ptr&& item) = 0;

      virtual void runtime_disconnected () = 0;

      virtual rx_reference_ptr get_reference () = 0;

      virtual ~item_resolver_user() = default;
  protected:

  private:


};






class runtime_item_resolver : public runtime_subscriber  
{
    resolver_state my_state_ = resolver_state::idle;

  public:

      rx_result start_resolver (const rx_item_reference& ref, item_resolver_user* user, ns::rx_directory_resolver* dirs);

      void stop_resolver ();

      void runtime_appeared (platform_item_ptr&& item);

      void runtime_destroyed (const rx_node_id& id);

      rx_reference_ptr get_reference ();


  protected:

  private:


      item_resolver_user *user_;


      ns::rx_directory_resolver* directories_;

      rx_item_reference runtime_reference_;

      rx_node_id resolved_id_;


};


} // namespace resolvers
} // namespace runtime
} // namespace rx_platform



#endif
