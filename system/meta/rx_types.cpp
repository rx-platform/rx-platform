

/****************************************************************************
*
*  system\meta\rx_types.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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

#include "rx_configuration.h"

// rx_types
#include "system/meta/rx_types.h"

#include "sys_internal/rx_internal_ns.h"
using namespace rx_platform::meta::meta_algorithm;


namespace rx_platform {

namespace meta {

namespace basic_types {

// Class rx_platform::meta::basic_types::event_type 

rx_item_type event_type::type_id = rx_item_type::rx_event_type;


platform_item_ptr event_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::basic_types::filter_type 

rx_item_type filter_type::type_id = rx_item_type::rx_filter_type;


platform_item_ptr filter_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::basic_types::mapper_type 

rx_item_type mapper_type::type_id = rx_item_type::rx_mapper_type;


platform_item_ptr mapper_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::basic_types::source_type 

rx_item_type source_type::type_id = rx_item_type::rx_source_type;


platform_item_ptr source_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::basic_types::struct_type 

rx_item_type struct_type::type_id = rx_item_type::rx_struct_type;


platform_item_ptr struct_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::basic_types::variable_type 

rx_item_type variable_type::type_id = rx_item_type::rx_variable_type;


rx_value variable_type::get_value () const
{
	rx_value temp;
	temp.assign_static(meta_info.version, meta_info.created_time);
	return temp;
}

platform_item_ptr variable_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());

}


// Class rx_platform::meta::basic_types::method_type 

rx_item_type method_type::type_id = rx_item_type::rx_method_type;


platform_item_ptr method_type::get_item_ptr () const
{
	return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());
}


// Class rx_platform::meta::basic_types::program_type 

rx_item_type program_type::type_id = rx_item_type::rx_program_type;


platform_item_ptr program_type::get_item_ptr () const
{
	return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());
}


// Class rx_platform::meta::basic_types::display_type 

rx_item_type display_type::type_id = rx_item_type::rx_display_type;


platform_item_ptr display_type::get_item_ptr () const
{
	return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());
}


// Class rx_platform::meta::basic_types::data_type 

rx_item_type data_type::type_id = rx_item_type::rx_data_type;


platform_item_ptr data_type::get_item_ptr () const
{
  return std::make_unique<rx_internal::internal_ns::rx_meta_item_implementation<smart_ptr> >(smart_this());
}


} // namespace basic_types
} // namespace meta
} // namespace rx_platform

