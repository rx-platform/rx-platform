

/****************************************************************************
*
*  system\meta\rx_checkable.cpp
*
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_blocks
#include "system/runtime/rx_blocks.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_checkable
#include "system/meta/rx_checkable.h"



namespace rx_platform {

namespace meta {

// Class rx_platform::meta::checkable_data 

checkable_data::checkable_data()
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
{
}

checkable_data::checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, namespace_item_attributes attrs)
      : version_(RX_INITIAL_ITEM_VERSION),
        created_time_(rx_time::now()),
        modified_time_(rx_time::now())
	, name_(name)
	, id_(id)
	, attributes_(attrs)
	, parent_(parent)
{
}



bool checkable_data::serialize_node (base_meta_writer& stream, uint8_t type, const rx_value_union& value) const
{
	if (!stream.write_header(type, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, type);*/
	/*if(!ret)
		return false;
*/
	if (!stream.write_footer())
		return false;

	return true;
}

bool checkable_data::deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value)
{
	return false;
}

bool checkable_data::check_in (base_meta_reader& stream)
{
	return false;
}

bool checkable_data::check_out (base_meta_writer& stream) const
{
	if (!stream.write_header(STREAMING_TYPE_CHECKOUT, 0))
		return false;

	/*std::function<void(base_meta_writter& stream, uint8_t)> func(std::bind(&metaT::serialize_definition, this, _1, _2));
	func(stream, STREAMING_TYPE_CHECKOUT);
*/

	if (!stream.write_footer())
		return false;

	return true;
}

bool checkable_data::serialize_checkable_definition (base_meta_writer& stream, uint8_t type) const
{
	if (!stream.start_object("Meta"))
		return false;
	if (!stream.write_id("NodeId", id_))
		return false;
	if (!stream.write_byte("Attrs", (uint8_t)attributes_))
		return false;
	if (!stream.write_string("Name", name_.c_str()))
		return false;
	if (!stream.write_id("SuperId", parent_))
		return false;
	if (!stream.write_version("Ver", version_))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool checkable_data::deserialize_checkable_definition (base_meta_reader& stream, uint8_t type)
{
	if (!stream.read_id("NodeId", id_))
		return false;
	uint8_t temp_byte;
	if (!stream.read_byte("Attrs", temp_byte))
		return false;
	attributes_ = (namespace_item_attributes)temp_byte;
	if (!stream.read_string("Name", name_))
		return false;
	if (!stream.read_id("SuperId", parent_))
		return false;
	if (!stream.read_version("Ver", version_))
		return false;
	return true;
}

values::rx_value checkable_data::get_value () const
{
	values::rx_value temp;
	temp.assign_static(version_, modified_time_);
	return temp;
}

void checkable_data::construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, bool system)
{
	name_ = name;
	id_ = id;
	parent_ = type_id;
}

bool checkable_data::get_system () const
{
	return (attributes_&namespace_item_system) != namespace_item_null;
}


} // namespace meta
} // namespace rx_platform

