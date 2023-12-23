

/****************************************************************************
*
*  protocols\opcua\rx_opcua_value.cpp
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


// rx_opcua_value
#include "protocols/opcua/rx_opcua_value.h"

#include "rx_opcua_params.h"
#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "lib/rx_values.h"
#include "rx_opcua_addr_space.h"
using namespace protocols::opcua::ids;

namespace protocols {
namespace opcua {
namespace common {


uint32_t rx_quality_to_status_code(uint32_t quality)
{
	switch ((quality & RX_QUALITY_MASK))
	{
	case RX_GOOD_QUALITY:
		{
			return opcid_Good;
		}
	case RX_UNCERTAIN_QUALITY:
		{
			return opcid_Uncertain;
		}
	case RX_BAD_QUALITY:
		{
			return opcid_Bad;
		}
	case RX_DEAD_QUALITY:
		{
			return opcid_Bad_ObjectDeleted;
		}
	default:
		{
			RX_ASSERT(false);
			return opcid_Bad;
		}
	}

}


uint32_t status_code_to_rx_quality(uint32_t status)
{
	switch (status & opcid_QualityMask)
	{
	case opcid_Good:
		return RX_GOOD_QUALITY;
		break;
	case opcid_Uncertain:
		return RX_UNCERTAIN_QUALITY;
		break;
	case opcid_Bad:
		return RX_UNCERTAIN_QUALITY;
		break;
	default:
		return RX_BAD_QUALITY_QUALITY_INVALID;
	}
}




const uint8_t variant_type::get_type() const
{
	return type_;
}


const int variant_type::get_array_len() const
{
	return array_len_;
}

const std::vector<int>& variant_type::get_dimensions() const
{
	return dimensions_;
}

void assign_vunion(vunion_type& what, bool val)
{
	what.b_val = val;
}
void assign_vunion(vunion_type& what, int8_t val)
{
	what.sc_val = val;
}
void assign_vunion(vunion_type& what, uint8_t val)
{
	what.c_val = val;
}
void assign_vunion(vunion_type& what, int16_t val)
{
	what.sw_val = val;
}
void assign_vunion(vunion_type& what, uint16_t val)
{
	what.w_val = val;
}
void assign_vunion(vunion_type& what, int32_t val)
{
	what.sdw_val = val;
}
void assign_vunion(vunion_type& what, uint32_t val)
{
	what.dw_val = val;
}
void assign_vunion(vunion_type& what, int64_t val)
{
	what.sqw_val = val;
}
void assign_vunion(vunion_type& what, uint64_t val)
{
	what.qw_val = val;
}
void assign_vunion(vunion_type& what, rx_time_struct val)
{
	what.ft_val = val;
}
void assign_vunion(vunion_type& what, float val)
{
	what.f_val = val;
}
void assign_vunion(vunion_type& what, double val)
{
	what.d_val = val;
}

void assign_vunion(vunion_type& what, ua_extension* val)
{
	what.ext_val = val;
}


void assign_vunion(vunion_type& what, const string_type& val)
{
	what.str_val = new string_type(val);
}

template<typename T>
void assign_vunion(vunion_type& what, const std::vector<T>& val)
{
	if (!val.empty())
	{
		what.array_val = new vunion_type[val.size()];
		for (size_t i = 0; i < val.size(); i++)
		{
			assign_vunion(what.array_val[i], val[i]);
		}
	}
	else
	{
		what.array_val = nullptr;
	}
}
// bytestring specialization
template<>
void assign_vunion(vunion_type& what, const std::vector<std::byte>& val)
{
	what.bstr_val = new byte_string(val);
}

template void assign_vunion<string_type>(vunion_type& what, const std::vector<string_type>& val);
template void assign_vunion<byte_string>(vunion_type& what, const std::vector<byte_string>& val);

void assign_vunion(vunion_type& what, const qualified_name& val)
{
	what.qname_val = new qualified_name(val);
}
void assign_vunion(vunion_type& what, const localized_text& val)
{
	what.ltext_val = new localized_text(val);
}

void assign_vunion(vunion_type& what, const rx_node_id& val)
{
	what.node_val = new rx_node_id(val);
}

void assign_vunion(vunion_type& what, const data_value& val)
{
	what.data_val = new data_value(val);
}
void assign_vunion(vunion_type& what, const variant_type& val)
{
	what.var_val = new variant_type(val);
}

void assign_vunion(vunion_type& what, const rx_uuid& val)
{
	what.guid_val = new rx_uuid_t(val);
}



uint8_t convert_from_rx_value_type(rx_value_t rx_val, uint8_t hint)
{
	switch (rx_val & RX_SIMPLE_VALUE_MASK)
	{
	case RX_BOOL_TYPE:
		return opcid_Boolean;
	case RX_INT8_TYPE:
		return opcid_SByte;
	case RX_UINT8_TYPE:
		return opcid_Byte;
	case RX_INT16_TYPE:
		return opcid_Int16;
	case RX_UINT16_TYPE:
		return opcid_UInt16;
	case RX_INT32_TYPE:
		return opcid_Int32;
	case RX_UINT32_TYPE:
		if (hint == opcid_StatusCode)
			return opcid_StatusCode;
		else
			return opcid_UInt32;
	case RX_INT64_TYPE:
		return opcid_Int64;
	case RX_UINT64_TYPE:
		return opcid_UInt64;
	case RX_FLOAT_TYPE:
		return opcid_Float;
	case RX_DOUBLE_TYPE:
		return opcid_Double;
	case RX_COMPLEX_TYPE:
		return opcid_Double;
	case RX_STRING_TYPE:
		{
			switch (hint)
			{
			case opcid_XmlElement:
				return opcid_XmlElement;
			case opcid_LocalizedText:
				return opcid_LocalizedText;
			case opcid_QualifiedName:
				return opcid_LocalizedText;
			default:
				return opcid_String;
			}
		}
		break;
	case RX_TIME_TYPE:
		return opcid_DateTime;
	case RX_UUID_TYPE:
		return opcid_Guid;
	case RX_BYTES_TYPE:
		return opcid_ByteString;
	case RX_STRUCT_TYPE:
		RX_ASSERT(false);
		return opcid_Null;
	case RX_TYPE_TYPE:
		RX_ASSERT(false);
		return opcid_Null;
	case RX_NODE_ID_TYPE:
		if (hint == opcid_ExpandedNodeId)
			return opcid_ExpandedNodeId;
		else
			return opcid_NodeId;
	default:
		RX_ASSERT(false);
		return opcid_Null;
	}
}

uint8_t assign_vunion(vunion_type& what, const values::rx_value& value, uint8_t hint, uint16_t ns = 0)
{
	if (hint == opcid_DataValue)
	{
		data_value temp;
		temp.value.from_rx_value(value);
		temp.set_timestamp(value.get_time());
		temp.status_code = rx_quality_to_status_code(value.get_quality());
		assign_vunion(what, temp);
		return opcid_DataValue;
	}
	else if (hint == opcid_BaseDataType)
	{
		variant_type temp;
		temp.from_rx_value(value);
		assign_vunion(what, temp);
		return opcid_BaseDataType;
	}
	else
	{
		switch (value.get_type())
		{
		case RX_BOOL_TYPE:
			assign_vunion(what, value.extract_static<bool>(false));
			return opcid_Boolean;
		case RX_INT8_TYPE:
			assign_vunion(what, value.extract_static<int8_t>(0));
			return opcid_SByte;
		case RX_UINT8_TYPE:
			assign_vunion(what, value.extract_static<uint8_t>(0));
			return opcid_Byte;
		case RX_INT16_TYPE:
			assign_vunion(what, value.extract_static<int16_t>(0));
			return opcid_Int16;
		case RX_UINT16_TYPE:
			assign_vunion(what, value.extract_static<uint16_t>(0));
			return opcid_UInt16;
		case RX_INT32_TYPE:
			assign_vunion(what, value.extract_static<int32_t>(0));
			return opcid_Int32;
		case RX_UINT32_TYPE:
			assign_vunion(what, value.extract_static<uint32_t>(0));
			if (hint == opcid_StatusCode)
				return opcid_StatusCode;
			else
				return opcid_UInt32;
		case RX_INT64_TYPE:
			assign_vunion(what, value.extract_static<int64_t>(0));
			return opcid_Int64;
		case RX_UINT64_TYPE:
			assign_vunion(what, value.extract_static<uint64_t>(0));
			return opcid_UInt64;
		case RX_FLOAT_TYPE:
			assign_vunion(what, value.extract_static<float>(0));
			return opcid_Float;
		case RX_DOUBLE_TYPE:
			assign_vunion(what, value.extract_static<double>(0));
			return opcid_Double;
		case RX_COMPLEX_TYPE:
			assign_vunion(what, value.extract_static<double>(0));
			return opcid_Double;
		case RX_STRING_TYPE:
			{
				switch (hint)
				{
				case opcid_XmlElement:
					assign_vunion(what, value.extract_static<string_type>(""));
					return opcid_XmlElement;
				case opcid_LocalizedText:
					assign_vunion(what, localized_text{ value.extract_static<string_type>("") });
					return opcid_LocalizedText;
				case opcid_QualifiedName:
					assign_vunion(what, qualified_name{ ns, value.extract_static<string_type>("") });
					return opcid_LocalizedText;
				default:
					assign_vunion(what, value.extract_static<string_type>(""));
					return opcid_String;
				}
			}
			break;
		case RX_TIME_TYPE:
			assign_vunion(what, value.extract_static<rx_time_struct>(rx_time::null_time().c_data()));
			return opcid_DateTime;
		case RX_UUID_TYPE:
			assign_vunion(what, value.extract_static<rx_uuid>(rx_uuid::null_uuid()));
			return opcid_Guid;
		case RX_BYTES_TYPE:
			assign_vunion(what, value.extract_static<byte_string>(byte_string()));
			return opcid_ByteString;
		case RX_STRUCT_TYPE:
			RX_ASSERT(false);
			return false;
		case RX_TYPE_TYPE:
			RX_ASSERT(false);
			return false;
		case RX_NODE_ID_TYPE:
			assign_vunion(what, value.extract_static<rx_node_id>(rx_node_id()));
			if (hint == opcid_ExpandedNodeId)
				return opcid_ExpandedNodeId;
			else
				return opcid_NodeId;
		case RX_STRING_TYPE | RX_ARRAY_VALUE_MASK:
			assign_vunion(what, value.extract_static<string_array>(string_array()));
			return opcid_String;
		default:
			return opcid_Null;
		}
	}
}

uint8_t assign_vunion(vunion_type& what, const values::rx_simple_value& value, uint8_t hint, uint16_t ns = 0)
{
	if (hint == opcid_BaseDataType)
	{
		variant_type temp;
		temp.from_rx_value(value);
		assign_vunion(what, temp);
		return opcid_BaseDataType;
	}
	else
	{
		switch (value.get_type())
		{
		case RX_BOOL_TYPE:
			assign_vunion(what, value.extract_static<bool>(false));
			return opcid_Boolean;
		case RX_INT8_TYPE:
			assign_vunion(what, value.extract_static<int8_t>(0));
			return opcid_SByte;
		case RX_UINT8_TYPE:
			assign_vunion(what, value.extract_static<uint8_t>(0));
			return opcid_Byte;
		case RX_INT16_TYPE:
			assign_vunion(what, value.extract_static<int16_t>(0));
			return opcid_Int16;
		case RX_UINT16_TYPE:
			assign_vunion(what, value.extract_static<uint16_t>(0));
			return opcid_UInt16;
		case RX_INT32_TYPE:
			assign_vunion(what, value.extract_static<int32_t>(0));
			return opcid_Int32;
		case RX_UINT32_TYPE:
			assign_vunion(what, value.extract_static<uint32_t>(0));
			if (hint == opcid_StatusCode)
				return opcid_StatusCode;
			else
				return opcid_UInt32;
		case RX_INT64_TYPE:
			assign_vunion(what, value.extract_static<int64_t>(0));
			return opcid_Int64;
		case RX_UINT64_TYPE:
			assign_vunion(what, value.extract_static<uint64_t>(0));
			return opcid_UInt64;
		case RX_FLOAT_TYPE:
			assign_vunion(what, value.extract_static<float>(0));
			return opcid_Float;
		case RX_DOUBLE_TYPE:
			assign_vunion(what, value.extract_static<double>(0));
			return opcid_Double;
		case RX_COMPLEX_TYPE:
			assign_vunion(what, value.extract_static<double>(0));
			return opcid_Double;
		case RX_STRING_TYPE:
			{
				switch (hint)
				{
				case opcid_XmlElement:
					assign_vunion(what, value.extract_static<string_type>(""));
					return opcid_XmlElement;
				case opcid_LocalizedText:
					assign_vunion(what, localized_text{ value.extract_static<string_type>("") });
					return opcid_LocalizedText;
				case opcid_QualifiedName:
					assign_vunion(what, qualified_name{ ns, value.extract_static<string_type>("") });
					return opcid_LocalizedText;
				default:
					assign_vunion(what, value.extract_static<string_type>(""));
					return opcid_String;
				}
			}
			break;
		case RX_TIME_TYPE:
			assign_vunion(what, value.extract_static<rx_time_struct>(rx_time::null_time().c_data()));
			return opcid_DateTime;
		case RX_UUID_TYPE:
			assign_vunion(what, value.extract_static<rx_uuid>(rx_uuid::null_uuid()));
			return opcid_Guid;
		case RX_BYTES_TYPE:
			assign_vunion(what, value.extract_static<byte_string>(byte_string()));
			return opcid_ByteString;
		case RX_STRUCT_TYPE:
			RX_ASSERT(false);
			return false;
		case RX_TYPE_TYPE:
			RX_ASSERT(false);
			return false;
		case RX_NODE_ID_TYPE:
			assign_vunion(what, value.extract_static<rx_node_id>(rx_node_id()));
			if (hint == opcid_ExpandedNodeId)
				return opcid_ExpandedNodeId;
			else
				return opcid_NodeId;
		case RX_STRING_TYPE | RX_ARRAY_VALUE_MASK:
			assign_vunion(what, value.extract_static<string_array>(string_array()));
			return opcid_String;
		default:
			return opcid_Null;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


variant_type::variant_type()
	: type_(opcid_Null),
	array_len_(-1)
{
	memzero(&union_, sizeof(union_));
}

variant_type::variant_type(const variant_type& right)
{
	copy_from(right);
}

variant_type::variant_type(variant_type&& right) noexcept
	: union_{ right.union_ }
	, array_len_(right.array_len_)
	, dimensions_{ std::move(right.dimensions_) }
	, type_{ right.type_ }
{
	right.type_ = opcid_Null;
	right.array_len_ = -1;
	memzero(&right.union_, sizeof(right.union_));
}

variant_type::variant_type(bool val)
	: type_(opcid_Boolean),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(char val)
	: type_(opcid_SByte),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(uint8_t val)
	: type_(opcid_Byte),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(int16_t val)
	: type_(opcid_Int16),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(uint16_t val)
	: type_(opcid_UInt16),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(int32_t val)
	: type_(opcid_Int32),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(uint32_t val)
	: type_(opcid_UInt32),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(int64_t val)
	: type_(opcid_Int64),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(uint64_t val)
	: type_(opcid_UInt64),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(float val)
	: type_(opcid_Float),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(double val)
	: type_(opcid_Double),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(const string_array& val)
	: type_(opcid_String),
	array_len_((int)val.size())
{
	if (array_len_)
	{
		union_.array_val = new vunion_type[array_len_];
		for (int i = 0; i < array_len_; i++)
		{
			assign_vunion(union_.array_val[i], val[i]);
		}
	}
	else
	{
		union_.array_val = nullptr;
	}
}
variant_type::variant_type(std::vector<std::unique_ptr<ua_extension> >& val)
	: type_(opcid_Structure),
	array_len_((int)val.size())
{

	if (array_len_)
	{
		union_.array_val = new vunion_type[array_len_];
		for (int i = 0; i < array_len_; i++)
		{
			assign_vunion(union_.array_val[i], val[i].release());
		}
	}
	else
	{
		union_.array_val = nullptr;
	}
}
variant_type::variant_type(const qualified_name& val)
	: type_(opcid_QualifiedName),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(const localized_text& val)
	: type_(opcid_LocalizedText),
	array_len_(-1)
{
	assign_vunion(union_, val);
}


variant_type::variant_type(const rx_node_id& val)
	: type_(opcid_NodeId),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(const data_value& val)
	: type_(opcid_BaseDataType),
	array_len_(-1)
{
	assign_vunion(union_, val);
}

variant_type::variant_type(rx_time val)
	: type_(opcid_DateTime),
	array_len_(-1)
{
	assign_vunion(union_, val.c_data());
}

variant_type::~variant_type()
{
	clear();
}


variant_type& variant_type::operator=(const variant_type& right)
{
	clear();
	copy_from(right);
	return *this;
}
variant_type& variant_type::operator=(variant_type&& right) noexcept
{
	clear();
	union_ = right.union_;
	array_len_ = right.array_len_;
	dimensions_ = std::move(right.dimensions_);
	type_ = right.type_;
	right.type_ = opcid_Null;
	right.array_len_ = -1;
	memzero(&right.union_, sizeof(right.union_));
	return *this;
}


bool variant_type::operator==(const variant_type& right)
{
	// TODO set compare
	return false;
	/*if (right.type_ != type_)
		return false;
		.
		.
		.
		.
	return true;*/
}
bool variant_type::operator!=(const variant_type& right)
{
	return !operator==(right);
}


bool variant_type::is_null() const
{
	return type_ == opcid_Null;

}
void variant_type::set_default(uint8_t type, int value_rank, const const_size_vector<uint32_t>& dimensions)
{
	clear();
	if (type >= opcid_Boolean && type <= opcid_DiagnosticInfo)
		type_ = type;
	switch (type_)
	{
	case opcid_Guid:
		union_.guid_val = new rx_uuid();
		break;
	case opcid_ByteString:
		union_.bstr_val = new byte_string();
		break;
	case opcid_NodeId:
		union_.node_val = new rx_node_id();
		break;
	case opcid_ExpandedNodeId:
		union_.node_val = new rx_node_id();
		break;
	case opcid_QualifiedName:
		union_.qname_val = new qualified_name{};
		break;
	case opcid_LocalizedText:
		union_.ltext_val = new localized_text{};
		break;
	case opcid_BaseDataType:
		union_.var_val = new variant_type();
		break;
	case opcid_DiagnosticInfo:
		union_.di_val = new diagnostic_info();
		break;
	}
}
uint32_t variant_type::get_opc_type_from_rx_type(rx_value_t valType)
{
	switch (valType)
	{
	case RX_BOOL_TYPE:
		return opcid_Boolean;
	case RX_INT8_TYPE:
		return opcid_SByte;
	case RX_UINT8_TYPE:
		return opcid_Byte;
	case RX_INT16_TYPE:
		return opcid_Int16;
	case RX_UINT16_TYPE:
		return opcid_UInt16;
	case RX_INT32_TYPE:
		return opcid_Int32;
	case RX_UINT32_TYPE:
		return opcid_UInt32;
	case RX_INT64_TYPE:
		return opcid_Int64;
	case RX_UINT64_TYPE:
		return opcid_UInt64;
	case RX_FLOAT_TYPE:
		return opcid_Float;
	case RX_DOUBLE_TYPE:
		return opcid_Double;
	case RX_COMPLEX_TYPE:
		return opcid_Double;
	case RX_STRING_TYPE:
		return opcid_String;
	case RX_TIME_TYPE:
		return opcid_DateTime;
	case RX_UUID_TYPE:
		return opcid_Guid;
	case RX_BYTES_TYPE:
		return opcid_ByteString;
	case RX_STRUCT_TYPE:
		RX_ASSERT(false);
		return opcid_Null;
	case RX_TYPE_TYPE:
		RX_ASSERT(false);
		return opcid_Null;
	case RX_NODE_ID_TYPE:
		return opcid_NodeId;
	case RX_STRING_TYPE | RX_ARRAY_VALUE_MASK:
		return opcid_String;
	default:
		return opcid_Null;
	}
}


void variant_type::clear()
{
	if (array_len_ > 0)
	{
		for (int i = 0; i < array_len_; i++)
			clear_union(type_, union_.array_val[i]);
		delete[] union_.array_val;
	}
	else if (array_len_ < 0)
	{
		clear_union(type_, union_);
	}

	memzero(&union_, sizeof(union_));
	type_ = opcid_Null;
	array_len_ = -1;
	dimensions_.clear();

}

bool variant_type::is_array() const
{
	return array_len_ >= 0;

}

void variant_type::copy_from(const variant_type& right)
{
	type_ = right.type_;
	if (right.array_len_ >= 0)
	{
		array_len_ = right.array_len_;
		union_.array_val = new vunion_type[array_len_];
		for (int i = 0; i < array_len_; i++)
		{
			copy_union_from(type_, union_.array_val[i], right.union_.array_val[i]);
		}
		dimensions_ = right.dimensions_;
	}
	else
	{
		copy_union_from(type_, union_, right.union_);
	}

}

void variant_type::clear_union(uint8_t type, vunion_type& vu)
{

	switch (type)
	{
	case opcid_Boolean:
		break;
	case opcid_SByte:
		break;
	case opcid_Byte:
		break;
	case opcid_Int16:
		break;
	case opcid_UInt16:
		break;
	case opcid_Int32:
		break;
	case opcid_UInt32:
		break;
	case opcid_Int64:
		break;
	case opcid_UInt64:
		break;
	case opcid_Float:
		break;
	case opcid_Double:
		break;
	case opcid_String:
		if (vu.str_val)
			delete vu.str_val;
		break;
	case opcid_DateTime:
		break;
	case opcid_Guid:
		delete vu.guid_val;
		break;
	case opcid_ByteString:
		delete vu.bstr_val;
		break;
	case opcid_XmlElement:
		if (vu.str_val)
			delete vu.str_val;
		break;
	case opcid_NodeId:
		delete vu.node_val;
		break;
	case opcid_ExpandedNodeId:
		delete vu.node_val;
		break;
	case opcid_StatusCode:
		break;
	case opcid_QualifiedName:
		delete vu.qname_val;
		break;
	case opcid_LocalizedText:
		delete vu.ltext_val;
		break;
	case opcid_Structure:
		if (vu.ext_val)
			delete vu.ext_val;
		break;
	case opcid_DataValue:
		if (vu.data_val != NULL)
			delete vu.data_val;
		break;
	case opcid_BaseDataType:
		delete vu.var_val;
		break;
	case opcid_DiagnosticInfo:
		delete vu.di_val;
		break;
	}

}

void variant_type::copy_union_from(uint8_t type, vunion_type& to, const vunion_type& from)
{
	switch (type)
	{
	case opcid_Boolean:
		to.b_val = from.b_val;
		break;
	case opcid_SByte:
		to.sc_val = from.sc_val;
		break;
	case opcid_Byte:
		to.c_val = from.c_val;
		break;
	case opcid_Int16:
		to.sw_val = from.sw_val;
		break;
	case opcid_UInt16:
		to.w_val = from.w_val;
		break;
	case opcid_Int32:
		to.sdw_val = from.sdw_val;
		break;
	case opcid_UInt32:
		to.dw_val = from.dw_val;
		break;
	case opcid_Int64:
		to.sqw_val = from.sqw_val;
		break;
	case opcid_UInt64:
		to.qw_val = from.qw_val;
		break;
	case opcid_Float:
		to.f_val = from.f_val;
		break;
	case opcid_Double:
		to.d_val = from.d_val;
		break;
	case opcid_String:
		to.str_val = from.str_val != nullptr ? new string_type(*(from.str_val)) : nullptr;
		break;
	case opcid_DateTime:
		to.ft_val = from.ft_val;
		break;
	case opcid_Guid:
		to.guid_val = new rx_uuid_t(*(from.guid_val));
		break;
	case opcid_ByteString:
		to.bstr_val = new byte_string(*(from.bstr_val));
		break;
	case opcid_XmlElement:
		to.str_val = from.str_val != nullptr ? new string_type(*(from.str_val)) : nullptr;
		break;
	case opcid_NodeId:
		to.node_val = new rx_node_id(*(from.node_val));
		break;
	case opcid_ExpandedNodeId:
		to.node_val = new rx_node_id(*(from.node_val));
		break;
	case opcid_StatusCode:
		to.dw_val = from.dw_val;
		break;
	case opcid_QualifiedName:
		to.qname_val = new qualified_name(*(from.qname_val));
		break;
	case opcid_LocalizedText:
		to.ltext_val = new localized_text(*(from.ltext_val));
		break;
	case opcid_Structure:
		if (from.ext_val == NULL)
			to.ext_val = NULL;
		else
			to.ext_val = from.ext_val->make_copy().release();
		break;
	case opcid_DataValue:
		to.data_val = new data_value(*(from.data_val));
		break;
	case opcid_BaseDataType:
		to.var_val = new variant_type(*(from.var_val));
		break;
	case opcid_DiagnosticInfo:
		to.di_val = from.di_val->make_clone();
		break;
	}

}

void variant_type::to_string(string_type& str)
{
	char buff[0x100];
	switch (type_)
	{
	case opcid_Null:
		str = "Null";
		break;
	case opcid_Boolean:
		str = union_.b_val ? "True" : "False";
		break;
	case opcid_SByte:
		sprintf(buff, "%d", (int)union_.sc_val);
		str = buff;
		break;
	case opcid_Byte:
		sprintf(buff, "%d", (int)union_.c_val);
		str = buff;
		break;
	case opcid_Int16:
		sprintf(buff, "%d", (int)union_.sw_val);
		str = buff;
		break;
	case opcid_UInt16:
		sprintf(buff, "%d", (int)union_.w_val);
		str = buff;
		break;
	case opcid_Int32:
		sprintf(buff, "%d", (int)union_.sdw_val);
		str = buff;
		break;
	case opcid_UInt32:
		sprintf(buff, "%x", (int)union_.dw_val);
		str = buff;
		break;
	case opcid_Int64:
		sprintf(buff, "%d", (int)union_.sqw_val);
		str = buff;
		break;
	case opcid_UInt64:
		sprintf(buff, "%d", (int)union_.qw_val);
		str = buff;
		break;
	case opcid_Float:
		sprintf(buff, "%g", (double)union_.f_val);
		str = buff;
		break;
	case opcid_Double:
		sprintf(buff, "%g", (double)union_.d_val);
		str = buff;
		break;
	case opcid_String:
		str = *union_.str_val;
		break;
	case opcid_DateTime:
		{
			rx_time tm(union_.ft_val);
			str = tm.get_string();
		}
		break;
	case opcid_Guid:
		rx_uuid(*union_.guid_val).to_string(str);
		break;
	case opcid_ByteString:
		str = "Bytes";
		break;
	case opcid_XmlElement:
		str = *union_.str_val;
		break;
	case opcid_NodeId:
		union_.node_val->to_string(str);
		break;
	case opcid_ExpandedNodeId:
		union_.node_val->to_string(str);
		break;
	case opcid_StatusCode:
		sprintf(buff, "%08X", (int)union_.dw_val);
		str = buff;
		break;
	case opcid_QualifiedName:
		str = union_.qname_val->name;
		break;
	case opcid_LocalizedText:
		str = union_.ltext_val->text;
		break;
	case opcid_Structure:
		str = "Struct";
		break;
	case opcid_DataValue:
		union_.data_val->value.to_string(str);
		break;
	case opcid_BaseDataType:
		union_.var_val->to_string(str);
		break;
	case opcid_DiagnosticInfo:
		str = "Diagnostics";
		break;
	}

}
bool variant_type::fill_rx_value(values::rx_value& value) const
{
	switch (type_)
	{
	case opcid_Null:
		value = values::rx_value();
		break;
	case opcid_Boolean:
		value.assign_static(union_.b_val);
		break;
	case opcid_SByte:
		value.assign_static(union_.sc_val);
		break;
	case opcid_Byte:
		value.assign_static(union_.c_val);
		break;
	case opcid_Int16:
		value.assign_static(union_.sw_val);
		break;
	case opcid_UInt16:
		value.assign_static(union_.w_val);
		break;
	case opcid_Int32:
		value.assign_static(union_.sdw_val);
		break;
	case opcid_UInt32:
		value.assign_static(union_.dw_val);
		break;
	case opcid_Int64:
		value.assign_static(union_.sqw_val);
		break;
	case opcid_UInt64:
		value.assign_static(union_.qw_val);
		break;
	case opcid_Float:
		value.assign_static(union_.f_val);
		break;
	case opcid_Double:
		value.assign_static(union_.d_val);
		break;
	case opcid_String:
		value.assign_static(*union_.str_val);
		break;
	case opcid_DateTime:
		value.assign_static(union_.ft_val);
		break;
	case opcid_Guid:
		value.assign_static(*union_.guid_val);
		break;
	case opcid_ByteString:
		value.assign_static(*union_.bstr_val);
		break;
	case opcid_XmlElement:
		value.assign_static(*union_.str_val);
		break;
	case opcid_NodeId:
		{
			string_type temp;
			union_.node_val->to_string(temp);
			value.assign_static(*union_.node_val);
		}
		break;
	case opcid_ExpandedNodeId:
		{
			string_type temp;
			union_.node_val->to_string(temp);
			value.assign_static(*union_.node_val);
		}
		break;
	case opcid_StatusCode:
		value.assign_static(union_.dw_val);
		break;
	case opcid_QualifiedName:
		value.assign_static(union_.qname_val->name);
		break;
	case opcid_LocalizedText:
		value.assign_static(union_.ltext_val->text);
		break;
	case opcid_Structure:
		return false;
		value.assign_static("Struct");
		break;
	case opcid_DataValue:
		union_.data_val->value.fill_rx_value(value);
		break;
	case opcid_BaseDataType:
		union_.var_val->fill_rx_value(value);
		break;
	case opcid_DiagnosticInfo:
		return false;
		value.assign_static("Diagnostics");
		break;
	}
	return true;
}


bool variant_type::fill_rx_value(values::rx_simple_value& value) const
{
	switch (type_)
	{
	case opcid_Null:
		value = values::rx_simple_value();
		break;
	case opcid_Boolean:
		value.assign_static(union_.b_val);
		break;
	case opcid_SByte:
		value.assign_static(union_.sc_val);
		break;
	case opcid_Byte:
		value.assign_static(union_.c_val);
		break;
	case opcid_Int16:
		value.assign_static(union_.sw_val);
		break;
	case opcid_UInt16:
		value.assign_static(union_.w_val);
		break;
	case opcid_Int32:
		value.assign_static(union_.sdw_val);
		break;
	case opcid_UInt32:
		value.assign_static(union_.dw_val);
		break;
	case opcid_Int64:
		value.assign_static(union_.sqw_val);
		break;
	case opcid_UInt64:
		value.assign_static(union_.qw_val);
		break;
	case opcid_Float:
		value.assign_static(union_.f_val);
		break;
	case opcid_Double:
		value.assign_static(union_.d_val);
		break;
	case opcid_String:
		value.assign_static(*union_.str_val);
		break;
	case opcid_DateTime:
		value.assign_static(union_.ft_val);
		break;
	case opcid_Guid:
		value.assign_static(*union_.guid_val);
		break;
	case opcid_ByteString:
		value.assign_static(*union_.bstr_val);
		break;
	case opcid_XmlElement:
		value.assign_static(*union_.str_val);
		break;
	case opcid_NodeId:
		{
			string_type temp;
			union_.node_val->to_string(temp);
			value.assign_static(*union_.node_val);
		}
		break;
	case opcid_ExpandedNodeId:
		{
			string_type temp;
			union_.node_val->to_string(temp);
			value.assign_static(*union_.node_val);
		}
		break;
	case opcid_StatusCode:
		value.assign_static(union_.dw_val);
		break;
	case opcid_QualifiedName:
		value.assign_static(union_.qname_val->name);
		break;
	case opcid_LocalizedText:
		value.assign_static(union_.ltext_val->text);
		break;
	case opcid_Structure:
		return false;
		value.assign_static("Struct");
		break;
	case opcid_DataValue:
		union_.data_val->value.fill_rx_value(value);
		break;
	case opcid_BaseDataType:
		union_.var_val->fill_rx_value(value);
		break;
	case opcid_DiagnosticInfo:
		return false;
		value.assign_static("Diagnostics");
		break;
	}
	return true;
}
/*

#define RX_SIMPLE_VALUE_MASK	0x1f



// value types for object and class types
#define RX_DEFINITION_TYPE_MASK	0x60
#define RX_BINARY_VALUE			0x00
#define RX_JSON_VALUE			0x20
#define RX_SCRIPT_VALUE			0x40


#define RX_STRIP_ARRAY_MASK		0x7f
#define RX_ARRAY_VALUE_MASK		0x80
*/

bool variant_type::from_rx_value(const values::rx_value& value, uint8_t hint, uint16_t ns)
{
	clear();

	if (value.is_array())
	{
		array_len_ = (int)value.array_size();
		type_ = assign_vunion(union_, value, hint, ns);

	}
	else
	{
		type_ = assign_vunion(union_, value, hint, ns);
	}
	return true;
}

bool variant_type::from_rx_value(const values::rx_simple_value& value, uint8_t hint, uint16_t ns)
{
	clear();

	if (value.is_array())
	{
		array_len_ = (int)value.array_size();
		type_ = assign_vunion(union_, value, hint, ns);

	}
	else
	{
		type_ = assign_vunion(union_, value, hint, ns);
	}
	return true;
}


data_value::data_value()
{

}
data_value::~data_value()
{

}
void data_value::set_timestamp(rx_time val)
{
	source_ts = val.c_data();
	source_ps = 0;
	server_ts = val.c_data();
	server_ps = 0;
}


data_value& data_value::operator=(const data_value& right)
{
	server_ps = right.server_ps;
	server_ts = right.server_ts;
	source_ps = right.source_ps;
	source_ts = right.source_ts;
	status_code = right.status_code;
	value = right.value;
	return *this;
}

rx_result data_value::fill_rx_value(values::rx_value& vvalue) const
{
	auto result = value.fill_rx_value(vvalue);
	if (result)
	{
		if (source_ts.t_value)
			vvalue.set_time(source_ts);
		else if (server_ts.t_value)
			vvalue.set_time(server_ts);
		else
			vvalue.set_time(rx_time::now());
		vvalue.set_quality(status_code_to_rx_quality(status_code));

		return true;
	}
	return RX_INVALID_CONVERSION;

}
rx_result data_value::from_rx_value(values::rx_value&& vvalue)
{
	value.from_rx_value(vvalue);
	set_timestamp(vvalue.get_time());
	status_code = rx_quality_to_status_code(vvalue.get_quality());
	return RX_OK;
}
rx_result data_value::from_rx_value(const values::rx_value& vvalue)
{
	value.from_rx_value(vvalue);
	set_timestamp(vvalue.get_time());
	status_code = rx_quality_to_status_code(vvalue.get_quality());
	return RX_OK;
}



} //namespace opcua
} //namespace common
} //namespace protocols
