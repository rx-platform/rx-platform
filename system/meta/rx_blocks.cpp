

/****************************************************************************
*
*  system\meta\rx_blocks.cpp
*
*  Copyright (c) 2018 Dusan Ciric
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


// rx_objbase
#include "system/meta/rx_objbase.h"
// rx_blocks
#include "system/meta/rx_blocks.h"

#include "system/server/rx_server.h"
#include "sys_internal/rx_internal_ns.h"
#include "lib/rx_ser_lib.h"
#include "system/serialization/rx_ser.h"
#include "sys_internal/rx_internal_ns.h"


namespace rx_platform {

namespace objects {

namespace blocks {

// Class rx_platform::objects::blocks::complex_runtime_item 

complex_runtime_item::complex_runtime_item()
{
}

complex_runtime_item::complex_runtime_item (const string_type& name, const rx_node_id& id, bool system)
	: parent_(id)
{
}


complex_runtime_item::~complex_runtime_item()
{
}



rx_value complex_runtime_item::get_value (const string_type path) const
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto it = names_cache_.find(mine);
		if (it != names_cache_.end())
		{// found it
			if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return sub_items_[(it->second&RT_INDEX_MASK)]->get_value(bellow);
			}
			else
			{// this here should never happed
				RX_ASSERT(false);
			}

		}
	}
	else// its' ours
	{
		auto it = names_cache_.find(path);
		if (it != names_cache_.end())
		{// search const values first
			if ((it->second&RT_CONST_IDX_MASK) == RT_CONST_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < const_values_.size());
				return const_values_[(it->second&RT_INDEX_MASK)].get_value({ my_object_->get_change_time(),my_object_->get_mode() });
			}
			else if ((it->second&RT_VALUE_IDX_MASK) == RT_VALUE_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < values_.size());
				return values_[(it->second&RT_INDEX_MASK)].get_value({ my_object_->get_change_time(),my_object_->get_mode() });
			}
			else if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return sub_items_[(it->second&RT_INDEX_MASK)]->get_value(nullptr);
			}
		}
	}

	rx_value ret;
	ret.set_time(rx_time::now());
	ret.set_quality(RX_BAD_QUALITY_CONFIG_ERROR);
	return ret;

}

uint32_t complex_runtime_item::set_hosting_object (object_runtime_ptr obj)
{
	RX_ASSERT(!my_object_);
	if (my_object_)
		return RX_ERROR;

	my_object_ = obj;
	return RX_OK;
}

object_runtime_ptr& complex_runtime_item::get_hosting_object ()
{
	return my_object_;
}

void complex_runtime_item::object_state_changed (const rx_time& now)
{
	for (auto& one : names_cache_)
	{
		uint32_t type = (one.second& RT_TYPE_MASK);
		switch (type)
		{
		case RT_CONST_IDX_MASK:
		{
			if ((one.second&RT_CALLBACK_MASK) != 0)
			{// has callback so fire all, its quality changed
				(*(const_values_callbacks_[RT_CALLBACK_INDEX(one.second)]))(
					const_values_[(one.second&RT_INDEX_MASK)].get_value({ my_object_->get_change_time(), my_object_->get_mode() }), 0);
			}

		}
		break;
		case RT_VALUE_IDX_MASK:
		{

		}
		break;
		case RT_COMPLEX_IDX_MASK:
		{
			sub_items_[(one.second&RT_INDEX_MASK)]->object_state_changed(now);
		}
		break;

		}
	}
}

value_callback_t* complex_runtime_item::get_callback (const string_type& path, rx_value& val)
{
	size_t idx = path.find(RX_OBJECT_DELIMETER);
	string_type mine;
	if (idx != string_type::npos)
	{// bellow us, split it
		mine = path.substr(0, idx);
		string_type bellow = path.substr(idx + 1);
		auto it = names_cache_.find(mine);
		if (it != names_cache_.end())
		{// found it
			if ((it->second&RT_COMPLEX_IDX_MASK) == RT_COMPLEX_IDX_MASK)
			{// has to be but?!?, it's fast so lets do it
				return sub_items_[(it->second&RT_INDEX_MASK)]->get_callback(bellow, val);
			}
			else
			{// this here should never happend
				RX_ASSERT(false);
			}

		}
	}
	else// its' ours
	{
		auto it = names_cache_.find(path);
		if (it != names_cache_.end())
		{// search const values first
			if ((it->second&RT_CONST_IDX_MASK) == RT_CONST_IDX_MASK)
			{// const value
				RX_ASSERT((it->second&RT_INDEX_MASK) < const_values_.size());
				if (it->second&RT_CALLBACK_MASK)
				{// we have callback mask so return it
					return const_values_callbacks_[(it->second&RT_CONST_IDX_MASK)];
				}
				else
				{// create new callback
					value_callback_t *ret = new value_callback_t;
					uint32_t idx = RT_CALLBACK_INDEX(it->second);
					if (idx >= const_values_callbacks_.size())
						const_values_callbacks_.resize(idx + 1);
					const_values_callbacks_[idx] = ret;
					return ret;

				}
			}

		}
	}
	return nullptr;
}

uint32_t complex_runtime_item::register_sub_item (const string_type& name, complex_runtime_item* val)
{
	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		uint32_t idx = (uint32_t)(sub_items_.size());
		idx |= RT_COMPLEX_IDX_MASK;
		sub_items_.emplace_back(val);
		names_cache_.emplace(name, idx);
		val->set_hosting_object(my_object_);
		names_cache_.emplace(name, idx);
		return RX_OK;
	}
	return RX_ERROR;
}

rx_value complex_runtime_item::get_value ()
{
	rx_value ret;
	ret.set_time(get_hosting_object()->get_created_time());
	ret.set_quality(RX_Q_TYPE_MISMATCH);
	return ret;
}

bool complex_runtime_item::serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_array("Items", names_cache_.size()))
		return false;
	for (const auto& one : names_cache_)
	{
		switch (one.second&RT_TYPE_MASK)
		{
		case RT_CONST_IDX_MASK:
		{// const value
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Name", one.first.c_str()))
				return false;
			if (!stream.write_string("Type", const_value_item::get_type_name().c_str()))
				return false;
			if (!stream.start_object("Val"))
				return false;
			const_values_[one.second&RT_INDEX_MASK].serialize_definition(stream, type, { ts, mode });
			if (!stream.end_object())//Val
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		break;
		case RT_VALUE_IDX_MASK:
		{// simple value
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Name", one.first.c_str()))
				return false;
			if (!stream.write_string("Type", value_item::get_type_name().c_str()))
				return false;
			if (!stream.start_object("Val"))
				return false;
			values_[one.second&RT_INDEX_MASK].serialize_definition(stream, type, { ts, mode });
			if (!stream.end_object())//Val
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		break;
		case RT_COMPLEX_IDX_MASK:
		{// complex item
			if (!stream.start_object("Item"))
				return false;
			if (!stream.write_string("Name", one.first.c_str()))
				return false;
			if (!stream.write_string("Type", sub_items_[one.second&RT_INDEX_MASK]->get_type_name().c_str()))
				return false;
			if(!sub_items_[one.second&RT_INDEX_MASK]->serialize_definition(stream,type,ts,mode))
				return false;
			if (!stream.end_object())//Item
				return false;
		}
		break;
		default:
			RX_ASSERT(false);
		}
	}
	if (!stream.end_array())
		return false;
	return true;
}

bool complex_runtime_item::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	RX_ASSERT(false);//fuck it!!!
	return false;
}

string_type complex_runtime_item::get_const_name (uint32_t name_idx) const
{
	uint32_t idx = name_idx & RT_INDEX_MASK;
	const auto& it = indexes_cache_.find(idx);
	if (it == indexes_cache_.end())
		return  it->second;
	else
		return rx_get_error_text(RX_INTERNAL_ERROR_NO_REGISTERED_NAME);
}

void complex_runtime_item::get_sub_items (server_items_type& items, const string_type& pattern) const
{
	for (const auto& one : names_cache_)
	{
		switch (one.second&RT_TYPE_MASK)
		{
		case RT_CONST_IDX_MASK:
		{// const value
			items.push_back(sys_internal::internal_ns::simple_platform_item::smart_ptr(
				one.first
				, const_values_[(one.second&RT_INDEX_MASK)].get_value({ my_object_->get_modified_time()
				, my_object_->get_mode() })
				, namespace_item_read_access
				, RX_CONST_VALUE_TYPE_NAME
				, my_object_->get_created_time()));
		}
		break;
		case RT_VALUE_IDX_MASK:
		{// const value
			items.push_back(sys_internal::internal_ns::simple_platform_item::smart_ptr(
				one.first
				, values_[(one.second&RT_INDEX_MASK)].get_value({ my_object_->get_modified_time() , my_object_->get_mode() })
				, (namespace_item_attributes)(namespace_item_read_access | (values_[(one.second&RT_INDEX_MASK)].is_readonly() ? namespace_item_null : namespace_item_write_access))
				, RX_VALUE_TYPE_NAME
				, my_object_->get_created_time()));
		}
		break;
		case RT_COMPLEX_IDX_MASK:
		{// complex item
		}
		break;
		default:
			RX_ASSERT(false);
		}
	}
}

uint32_t complex_runtime_item::register_const_value (const string_type& name, rx_simple_value&& val)
{
	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		uint32_t idx = (uint32_t)(const_values_.size());
		idx |= RT_CONST_IDX_MASK;
		const_values_.emplace_back(std::move(val));
		names_cache_.emplace(name, idx);
		return RX_OK;
	}
	return RX_ERROR;
}

uint32_t complex_runtime_item::register_value (const string_type& name, rx_timed_value&& val)
{
	auto it = names_cache_.find(name);
	if (it == names_cache_.end())
	{
		uint32_t idx = (uint32_t)(values_.size());
		idx |= RT_VALUE_IDX_MASK;
		values_.emplace_back(std::move(val));
		names_cache_.emplace(name, idx);
		return RX_OK;
	}
	return RX_ERROR;
}

string_type complex_runtime_item::get_type_name () const
{
	return RX_NULL_ITEM_NAME;
}


// Class rx_platform::objects::blocks::const_value_item 

const uint32_t const_value_item::type_id_ = RT_TYPE_ID_CONST_VALUE;


bool const_value_item::serialize_definition (base_meta_writer& stream, uint8_t type, const object_state_data& data) const
{
	if (!storage_.serialize(stream))
		return false;
	return true;
}

bool const_value_item::deserialize_definition (base_meta_reader& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode)
{
	return false;
}

bool const_value_item::has_own_time () const
{
  return false;

}

namespace_item_attributes const_value_item::get_attributes () const
{
	return namespace_item_attributes::namespace_item_read_access;
}

void const_value_item::item_lock ()
{
}

void const_value_item::item_unlock ()
{
}

void const_value_item::item_lock () const
{
}

void const_value_item::item_unlock () const
{
}

void const_value_item::get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info)
{
}

string_type const_value_item::get_type_name ()
{
	return RX_CONST_VALUE_TYPE_NAME;
}

rx_value const_value_item::get_value (const object_state_data& data) const
{
	rx_value temp;
	temp.assign_static(0);
	return temp;
}

uint32_t const_value_item::register_value (const string_type& name, const rx_value& val)
{
	return 0;
}


// Class rx_platform::objects::blocks::filter_runtime 

string_type filter_runtime::type_name = RX_CPP_FILTER_TYPE_NAME;

filter_runtime::filter_runtime()
	: my_item_(std::make_unique<complex_runtime_item>("_unnamed", rx_node_id::null_id, false))
{
}



string_type filter_runtime::get_type_name () const
{
  return type_name;

}


// Class rx_platform::objects::blocks::mapper_runtime 

mapper_runtime::mapper_runtime()
{
}


mapper_runtime::~mapper_runtime()
{
}



// Class rx_platform::objects::blocks::source_runtime 

string_type source_runtime::type_name = RX_CPP_SOURCE_TYPE_NAME;

source_runtime::source_runtime()
{
}


source_runtime::~source_runtime()
{
}



// Class rx_platform::objects::blocks::struct_runtime 

string_type struct_runtime::type_name = RX_CPP_STRUCT_TYPE_NAME;

struct_runtime::struct_runtime()
{
}

struct_runtime::struct_runtime (const string_type& name, const rx_node_id& id, bool system)
	: complex_runtime_item(name, id, system)
{
}



bool struct_runtime::serialize_definition (base_meta_writer& stream, uint8_t type, const rx_time& ts, const rx_mode_type& mode) const
{
	if (!stream.start_object(struct_runtime::type_name.c_str()))
		return false;
	if (!meta_data_.serialize_checkable_definition(stream, type))
		return false;
	if (!complex_runtime_item::serialize_definition(stream, type, ts, mode))
		return false;
	if (!stream.end_object())
		return false;
	return true;
}

bool struct_runtime::deserialize_definition (base_meta_reader& stream, uint8_t type)
{
	RX_ASSERT(false);//
	return true;
}

meta::checkable_data& struct_runtime::meta_data ()
{
  return meta_data_;

}

string_type struct_runtime::get_type_name () const
{
  return type_name;

}


const meta::checkable_data& struct_runtime::meta_data () const
{
  return meta_data_;
}


// Class rx_platform::objects::blocks::value_item 

const uint32_t value_item::type_id_ = RT_TYPE_ID_VALUE;


bool value_item::serialize_definition (base_meta_writer& stream, uint8_t type, const object_state_data& data) const
{
	if (!storage_.serialize(stream))
		return false;
	return true;
}

bool value_item::deserialize_definition (base_meta_reader& stream, uint8_t type, const rx_mode_type& mode)
{
	return false;
}

rx_value value_item::get_value (const object_state_data& data) const
{
	rx_value val;
	storage_.get_value(val, std::max(get_change_time(), data.ts), data.mode);
	return val;
}

string_type value_item::get_type_name ()
{
	return RX_VALUE_TYPE_NAME;
}


// Class rx_platform::objects::blocks::variable_runtime 

string_type variable_runtime::type_name = RX_CPP_VARIABLE_TYPE_NAME;

variable_runtime::variable_runtime()
{
}

variable_runtime::variable_runtime (const string_type& name, const rx_node_id& id, bool system)
	: my_item_(std::make_unique<complex_runtime_item>(name, id, system))
{
}


variable_runtime::~variable_runtime()
{
}



// Class rx_platform::objects::blocks::event_runtime 

string_type event_runtime::type_name = RX_CPP_EVENT_TYPE_NAME;

event_runtime::event_runtime()
{
}



string_type event_runtime::get_type_name () const
{
  return type_name;

}


} // namespace blocks
} // namespace objects
} // namespace rx_platform

