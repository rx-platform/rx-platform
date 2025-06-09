

/****************************************************************************
*
*  discovery\rx_discovery_items.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef rx_discovery_items_h
#define rx_discovery_items_h 1


#include "rx_discovery_configuration.h"
#include "rx_discovery_main.h"

// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_internal {
namespace discovery {
class peer_item_stub;
class peer_connection;

} // namespace discovery
} // namespace rx_internal




namespace rx_internal {

namespace discovery {





class peer_item : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(peer_item);

  public:
      peer_item (rx_item_type t, meta_data m, peer_connection_ptr conn);

      ~peer_item();


      security::security_guard_ptr get_security_guard ();

      platform_item_ptr get_item_ptr () const;


      rx_item_type type;

      meta_data meta;

      peer_connection_ptr connection;


  protected:

  private:

      security::security_guard security_guard_;



      peer_connection_ptr peer_connection_;

      std::unique_ptr<peer_item_stub> stub_;


};






class peer_item_stub 
{

  public:
      virtual ~peer_item_stub();


  protected:

  private:


};






template <class typeT>
class peer_type_item : public peer_item_stub  
{
    typedef rx_reference<typeT> type_ptr_type;

  public:
      peer_type_item();

      ~peer_type_item();


  protected:

  private:


      type_ptr_type type_ptr_;


};


} // namespace discovery
} // namespace rx_internal



#endif
