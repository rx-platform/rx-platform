

/****************************************************************************
*
*  discovery\rx_discovery_items.cpp
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


#include "pch.h"

#include "system/runtime/rx_holder_algorithms.h"
#include "system/meta/rx_obj_types.h"

// rx_discovery_main
#include "discovery/rx_discovery_main.h"
// rx_discovery_items
#include "discovery/rx_discovery_items.h"



namespace rx_internal {

namespace discovery {

// Class rx_internal::discovery::peer_item 

peer_item::peer_item (rx_item_type t, meta_data m, peer_connection_ptr conn)
      : type(t),
        meta(std::move(m)),
        connection(std::move(conn))
{
    DISCOVERY_LOG_DEBUG("peer_object", 200, "{rx-platform} discovery peer item "s + meta.get_full_path() + " created.");
}


peer_item::~peer_item()
{
	DISCOVERY_LOG_DEBUG("peer_object", 200, "{rx-platform} discovery peer item "s + meta.get_full_path() + " destroyed.");
}



// Class rx_internal::discovery::peer_item_stub 

peer_item_stub::~peer_item_stub()
{
}



// Parameterized Class rx_internal::discovery::peer_type_item 

template <class typeT>
peer_type_item<typeT>::peer_type_item()
{
}


template <class typeT>
peer_type_item<typeT>::~peer_type_item()
{
}



} // namespace discovery
} // namespace rx_internal

template class rx_internal::discovery::peer_type_item<object_types::object_type>;
