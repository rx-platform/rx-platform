

/****************************************************************************
*
*  protocols\opcua\rx_opcua_params.cpp
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


#include "pch.h"


// rx_opcua_params
#include "protocols/opcua/rx_opcua_params.h"

#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "lib/rx_values.h"
using namespace protocols::opcua::ids;


namespace protocols {
namespace opcua {
namespace common {


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
void assign_vunion(vunion_type& what, float val)
{
	what.f_val = val;
}
void assign_vunion(vunion_type& what, double val)
{
	what.d_val = val;
}

void assign_vunion(vunion_type& what, const string_type& val)
{
	what.str_val = new string_type(val);
}
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
		union_.array_val=new vunion_type[array_len_];
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



bool variant_type::is_null() const
{
	return type_ == opcid_Null;

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
		to.str_val = new string_type(*(from.str_val));
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
		to.str_val = new string_type(*(from.str_val));
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

void variant_type::fill_variable_value(values::rx_value& value) const
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
		value.assign_static("Struct");
		break;
	case opcid_DataValue:
		union_.data_val->value.fill_variable_value(value);
		break;
	case opcid_BaseDataType:
		union_.var_val->fill_variable_value(value);
		break;
	case opcid_DiagnosticInfo:
		value.assign_static("Diagnostics");
		break;
	}
}

bool variant_type::from_rx_value(const values::rx_value& value, uint8_t hint)
{
	clear();
	if (value.is_numeric())
	{
		type_ = opcid_UInt32;
		union_.dw_val = (uint32_t)value.get_float();
		/*type_=opcid_Double;
		union_.d_val=value.to_double();*/
		return true;
	}
	return false;

}


ua_extension::ua_extension(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id)
	: class_id(class_id), binary_id(binary_id), xml_id(xml_id)
{

}

opcua_extension_ptr ua_extension::make_copy()
{
	return std::make_unique<ua_extension>();
}

void ua_extension::internal_serialize_extension(binary::ua_binary_ostream& stream) const
{

}
void ua_extension::internal_deserialize_extension(binary::ua_binary_istream& stream)
{

}



void user_token_policy::deserialize(binary::ua_binary_istream& stream)
{
	stream >> policy_id;
	stream >> token_type;
	stream >> issued_token_type;
	stream >> issuer_endpoint_url;
	stream >> policy_uri;
}
void user_token_policy::serialize(binary::ua_binary_ostream& stream) const
{
	stream << policy_id;
	stream << token_type;
	stream << issued_token_type;
	stream << issuer_endpoint_url;
	stream << policy_uri;
}

void application_description::deserialize(binary::ua_binary_istream& stream)
{
	stream >> application_uri;
	stream >> product_uri;
	stream >> application_name;
	int temp;
	stream >> temp;
	application_type = (application_type_t)temp;
	stream >> gateway_uri;
	stream >> discovery_profile_uri;
	stream >> discovery_urls;
}
void application_description::serialize(binary::ua_binary_ostream& stream) const
{
	stream << application_uri;
	stream << product_uri;
	stream << application_name;
	stream << (int)application_type;
	stream << gateway_uri;
	stream << discovery_profile_uri;
	stream << discovery_urls;
}

void endpoint_description::deserialize(binary::ua_binary_istream& stream)
{
	stream >> url;
	application.deserialize(stream);
	stream >> certificate;
	int temp;
	stream >> temp;
	security_mode = (security_mode_t)temp;
	stream >> policy_uri;
	stream.deserialize_array(user_tokens);
	stream >> transport_profile_uri;
	stream >> security_level;
}
void endpoint_description::serialize(binary::ua_binary_ostream& stream) const
{
	stream << url;
	application.serialize(stream);
	stream << certificate;
	stream << (int)security_mode;
	stream << policy_uri;
	stream.serialize_array(user_tokens);
	stream << transport_profile_uri;
	stream << security_level;
}


void read_value_id::serialize(binary::ua_binary_ostream& stream) const
{
	stream << node_id;
	stream << attr_id;
	stream << range;
	stream << data_encoding;
}
void read_value_id::deserialize(binary::ua_binary_istream& stream)
{
	stream >> node_id;
	stream >> attr_id;
	stream >> range;
	stream >> data_encoding;
}



void diagnostic_info::fill_diagnostics_strings(string_array& strings, uint32_t mask) const
{
	int current_idx = (int)strings.size();
	if (!namespace_uri.empty())
	{
		namespace_uri_idx = current_idx;
		current_idx++;
		strings.push_back(namespace_uri);
	}
	if (!symbolic_id.empty())
	{
		symbolic_id_idx = current_idx;
		current_idx++;
		strings.push_back(symbolic_id);
	}
	if (!locale.empty())
	{
		locale_idx = current_idx;
		current_idx++;
		strings.push_back(locale);
	}
	if (!localized_text.empty())
	{
		localized_text_idx = current_idx;
		current_idx++;
		strings.push_back(localized_text);
	}
	if (internal_diagnostic_info)
	{
		internal_diagnostic_info->fill_diagnostics_strings(strings, mask);
	}
}
void diagnostic_info::retrive_diagnostics_strings(const string_array& strings, uint32_t result)
{
	if (namespace_uri_idx >= 0)
	{
		namespace_uri = strings[namespace_uri_idx];
	}
	if (symbolic_id_idx >= 0)
	{
		symbolic_id = strings[symbolic_id_idx];
	}
	/*else
	{
		symbolic_id = standard_opc_codes[OPC_STATUS_CODE(result)];
	}*/
	if (locale_idx >= 0)
	{
		locale = strings[locale_idx];
	}
	if (localized_text_idx >= 0)
	{
		localized_text = strings[localized_text_idx];
	}
	if (internal_diagnostic_info)
	{
		internal_diagnostic_info->retrive_diagnostics_strings(strings, result);
	}
}

diagnostic_info* diagnostic_info::make_clone()
{
	diagnostic_info* ret = new diagnostic_info();
	ret->additional_info = additional_info;
	ret->symbolic_id = symbolic_id;
	ret->localized_text = localized_text;
	ret->locale = locale;
	ret->namespace_uri = namespace_uri;
	ret->internal_result = internal_result;
	if (internal_diagnostic_info)
		ret->internal_diagnostic_info = std::unique_ptr<diagnostic_info>(internal_diagnostic_info->make_clone());

	return ret;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




} //namespace opcua
} //namespace common
} //namespace protocols

