

/****************************************************************************
*
*  protocols\opcua\rx_opcua_binary.cpp
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


// rx_opcua_binary
#include "protocols/opcua/rx_opcua_binary.h"


#include "protocols/opcua/rx_opcua_identifiers.h"

using namespace protocols::opcua::ids;


namespace protocols {
namespace opcua {
namespace binary {

ua_binary_istream::ua_binary_istream(rx::io::rx_const_io_buffer* buffer)
	: buffer_(buffer)
{
}
ua_binary_istream::~ua_binary_istream()
{
}
ua_binary_istream& ua_binary_istream::operator>> (bool& val)
{
	uint8_t temp;
	auto result = buffer_->read_from_buffer(temp);
	val = temp != 0;
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (uint16_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (int16_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (uint32_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (int32_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (float& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (double& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (string_type& val)
{
	int length = 0;
	auto result = buffer_->read_from_buffer(length);
	if (result)
	{
		if (length > 0)
		{
			rx_protocol_result_t ret;
			char* temp = (char*)rx_get_from_packet(buffer_->c_buffer(), length, &ret);
			if (temp)
			{
				val.assign(temp, length);
			}
			else
				result = rx_protocol_error_message(ret);
		}
		else
		{
			val.clear();
		}
	}
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (rx_time_struct& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (uint8_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (byte_string& val)
{
	int length = 0;
	auto result = buffer_->read_from_buffer(length);
	if (result)
	{
		if (length > 0)
		{
			rx_protocol_result_t ret;
			std::byte* temp = (std::byte*)rx_get_from_packet(buffer_->c_buffer(), length, &ret);
			if (temp)
			{
				val.assign(temp, temp + length);
			}
			else
				result = rx_protocol_error_message(ret);
		}
		else
		{
			val.clear();
		}
	}
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (diagnostic_info& val)
{
	uint32_t result;

	int namespace_uri = -1;
	int symbolic_id = -1;
	int locale = -1;
	int localized_text = -1;
	uint32_t inner;
	string_type additional;
	std::unique_ptr<diagnostic_info> inner_obj;

	uint8_t encoding = 0;
	*this >> encoding;

	if (encoding & 0x2)
		*this >> namespace_uri;
	if (encoding & 0x1)
		*this >> symbolic_id;
	if (encoding & 0x8)
		*this >> locale;
	if (encoding & 0x4)
		*this >> localized_text;
	if (encoding & 0x10)
		*this >> additional;
	if (encoding & 0x20)
		*this >> result;
	if (encoding & 0x40)
	{
		inner_obj = std::make_unique<diagnostic_info>();
		*this >> inner;
	}
	if ((encoding & 0x1) && symbolic_id >= 0)
		val.symbolic_id_idx = symbolic_id;
	if ((encoding & 0x2) && namespace_uri >= 0)
		val.namespace_uri_idx = namespace_uri;
	if ((encoding & 0x4) && localized_text >= 0)
		val.localized_text_idx = localized_text;
	if ((encoding & 0x8) && locale >= 0)
		val.locale_idx = locale;
	if ((encoding & 0x10))
		val.additional_info = additional;
	if (encoding & 0x20)
		val.internal_result = result;
	if (inner_obj)
		val.internal_diagnostic_info = std::move(inner_obj);

	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (localized_text& val)
{
	uint8_t encoding = 0;
	(*this) >> encoding;
	if (!val.localeid.empty())
		val.localeid.clear();
	if (!val.text.empty())
		val.text.clear();
	if (encoding & 0x1)
		(*this) >> val.localeid;
	if (encoding & 0x2)
		(*this) >> val.text;
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (rx_uuid_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}

std::unique_ptr<ua_extension> ua_binary_istream::deserialize_extension(std::function<std::unique_ptr<ua_extension>(const rx_node_id& id)> creator_func)
{
	rx_node_id id;
	(*this) >> id;
	std::unique_ptr<ua_extension> val = creator_func(id);
	uint8_t encoding;
	(*this) >> encoding;
	if (val != NULL)
	{
		if (encoding == 1)
		{
			int len;
			(*this) >> len;
			val->internal_deserialize_extension(*this);
		}

	}
	else
	{
		if (encoding != 0)
		{// do the dummy read for this stuff
			byte_string dummy;
			(*this) >> dummy;
		}
	}
	return val;
}
ua_binary_istream& ua_binary_istream::operator>> (node_class_type& val)
{
	int temp;
	(*this) >> temp;
	val = (node_class_type)temp;
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (browse_direction_type& val)
{
	int temp;
	(*this) >> temp;
	val = (browse_direction_type)temp;
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (timestamps_return_type& val)
{
	int temp;
	(*this) >> temp;
	val = (timestamps_return_type)temp;
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (attribute_id& val)
{
	uint32_t temp;
	(*this) >> temp;
	val = (attribute_id)temp;
	return *this;
}

ua_binary_istream& ua_binary_istream::operator >> (qualified_name& val)
{
	(*this) >> val.namesp;
	(*this) >> val.name;
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (data_value& val)
{
	uint8_t encoding;
	(*this) >> encoding;
	if (encoding & 0x1)
	{
		(*this) >> val.value;
	}
	if (encoding & 0x2)
	{
		(*this) >> val.status_code;
	}
	if (encoding & 0x4)
	{
		(*this) >> val.source_ts;
	}
	if (encoding & 0x8)
	{
		(*this) >> val.server_ts;
	}
	if (encoding & 0x10)
	{
		(*this) >> val.source_ps;
	}
	if (encoding & 0x20)
	{
		(*this) >> val.server_ps;
	}
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (variant_type& val)
{
	val.clear();
	uint8_t encoding;
	(*this) >> encoding;
	uint8_t type = (encoding & 0x1f);
	val.type_ = type;
	if (encoding & 0x80)
	{// array stuff
		int length;
		(*this) >> length;
		val.union_.array_val = new vunion_type[length];
		for (int i = 0; i < length; i++)
			deserialize_vunion(type, val.union_.array_val[i]);
		val.array_len_ = length;
		if (encoding & 0x40)
		{// we have dimensions
			int current = 0;
			while (current < length)
			{
				int dim;
				(*this) >> dim;
				current += dim;
				val.dimensions_.push_back(dim);
			}
		}
	}
	else
	{ // no array
		deserialize_vunion(type, val.union_);
	}

	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (char& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (int64_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (uint64_t& val)
{
	auto result = buffer_->read_from_buffer(val);
	return *this;
}
ua_binary_istream& ua_binary_istream::operator>> (change_trigger_type& val)
{
	int temp;
	(*this) >> temp;
	val = (change_trigger_type)temp;
	return *this;
}
void ua_binary_istream::deserialize_vunion(uint8_t type, vunion_type& vu)
{
	switch (type)
	{
	case opcid_Boolean:
		(*this) >> vu.b_val;
		break;
	case opcid_SByte:
		(*this) >> vu.sc_val;
		break;
	case opcid_Byte:
		(*this) >> vu.c_val;
		break;
	case opcid_Int16:
		(*this) >> vu.sw_val;
		break;
	case opcid_UInt16:
		(*this) >> vu.w_val;
		break;
	case opcid_Int32:
		(*this) >> vu.sdw_val;
		break;
	case opcid_UInt32:
		(*this) >> vu.dw_val;
		break;
	case opcid_Int64:
		(*this) >> vu.sqw_val;
		break;
	case opcid_UInt64:
		(*this) >> vu.qw_val;
		break;
	case opcid_Float:
		(*this) >> vu.f_val;
		break;
	case opcid_Double:
		(*this) >> vu.d_val;
		break;
	case opcid_String:
		vu.str_val = new string_type;
		(*this) >> (*vu.str_val);
		break;
	case opcid_DateTime:
		(*this) >> vu.ft_val;
		break;
	case opcid_Guid:
		vu.guid_val = new rx_uuid_t;
		(*this) >> (*vu.guid_val);
		break;
	case opcid_ByteString:
		vu.bstr_val = new byte_string;
		(*this) >> (*vu.bstr_val);
		break;
	case opcid_XmlElement:
		vu.str_val = new string_type;
		(*this) >> (*vu.str_val);
		break;
	case opcid_NodeId:
		vu.node_val = new rx_node_id;
		(*this) >> (*vu.node_val);
		break;
	case opcid_ExpandedNodeId:
		vu.node_val = new rx_node_id;
		(*this) >> (*vu.node_val);
		break;
	case opcid_StatusCode:
		(*this) >> vu.dw_val;
		break;
	case opcid_QualifiedName:
		vu.qname_val = new qualified_name;
		(*this) >> (*vu.qname_val);
		break;
	case opcid_LocalizedText:
		vu.ltext_val = new localized_text;
		(*this) >> (*vu.ltext_val);
		break;
	case opcid_Structure:
		{
			auto ret_ptr = deserialize_extension([](const rx_node_id& id) {
				if (id.is_null())
					return std::make_unique<ua_extension>();
				else
					return opcua_extension_ptr();
				});
			vu.ext_val = ret_ptr.release();
		}
		break;
	case opcid_DataValue:
		vu.data_val = new data_value;
		(*this) >> (*vu.data_val);
		break;
	case opcid_BaseDataType:
		vu.var_val = new variant_type;
		(*this) >> (*vu.var_val);
		break;
	case opcid_DiagnosticInfo:
		vu.di_val = new diagnostic_info;
		(*this) >> (*vu.di_val);
		break;
	}
}
ua_binary_istream& ua_binary_istream::operator>> (rx_node_id& val)
{
	uint8_t encoding;
	(*this) >> encoding;
	switch (encoding & 0xf)
	{
	case 0:// two bytes
		uint8_t id;
		(*this) >> id;
		val = rx_node_id(id, 0);
		break;
	case 1:// for bytes
		{
			uint8_t nmsp;
			uint16_t id;
			(*this) >> nmsp;
			(*this) >> id;
			val = rx_node_id(id, nmsp);
		}
		break;
	case 2:
		{
			uint16_t namesp;
			uint32_t id;
			(*this) >> namesp;
			(*this) >> id;
			val = rx_node_id(id, namesp);
		}
		break;
	case 3:
		{
			uint16_t namesp;
			string_type id;
			(*this) >> namesp;
			(*this) >> id;
			val = rx_node_id(id.c_str(), namesp);
		}
		break;
	case 4:
		{
			uint16_t namesp;
			rx_uuid_t id;
			(*this) >> namesp;
			(*this) >> id;
			val = rx_node_id(id, namesp);
		}
		break;
	case 5:
		{
			uint16_t namesp;
			byte_string id;
			(*this) >> namesp;
			(*this) >> id;
			val = rx_node_id(id, namesp);
		}
		break;
	}
	if (encoding & 0x80)
	{
		RX_ASSERT(false);
		/*String uri;
		(*this) >> uri;
		val.set_namespace_uri(uri);
		*/
	}
	if (encoding & 0x40)
	{
		RX_ASSERT(false);
		/*dword svrindex;
		(*this) >> svrindex;
		val.set_server_index(svrindex);
		*/
	}
	return *this;
}


ua_binary_ostream::ua_binary_ostream(rx::io::rx_io_buffer* buffer)
	: buffer_(buffer)
{
}
ua_binary_ostream::~ua_binary_ostream()
{
}
ua_binary_ostream& ua_binary_ostream::operator << (bool val)
{
	uint8_t temp = val ? 1 : 0;
	auto result = buffer_->write_to_buffer(temp);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (uint16_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (int16_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (uint32_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (int32_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (float val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (double val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const string_type& val)
{
	if (val.empty())
	{
		(*this) << -1;
	}
	else
	{
		auto result = buffer_->write_string(val);
	}
	return *this;

}
ua_binary_ostream& ua_binary_ostream::operator << (const rx_time_struct& val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const byte_string& val)
{
	if (val.empty())
	{
		(*this) << -1;
	}
	else
	{
		int len = (int)val.size();
		auto result = buffer_->write_to_buffer(len);
		if (result && len > 0)
			buffer_->write(&val[0], len);
	}
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (uint8_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const diagnostic_info& val)
{
	uint8_t encoding = 0;

	if (val.symbolic_id_idx >= 0)
		encoding |= 0x1;
	if (val.namespace_uri_idx >= 0)
		encoding |= 0x2;
	if (val.localized_text_idx >= 0)
		encoding |= 0x4;
	if (val.locale_idx >= 0)
		encoding |= 0x8;
	if(!val.additional_info.empty())
		encoding |= 0x10;

	*this << encoding;

	if (encoding & 0x1)
		*this << val.symbolic_id_idx;
	if (encoding & 0x2)
		*this << val.namespace_uri_idx;
	if (encoding & 0x4)
		*this << val.localized_text_idx;
	if (encoding & 0x8)
		*this << val.locale_idx;
	if (encoding & 0x10)
		*this << val.additional_info;
	if (encoding & 0x20)
		*this << val.internal_result;
	if (encoding & 0x40)
		*this << *val.internal_diagnostic_info;

	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const localized_text& val)
{
	uint8_t encoding = 0;
	if (!val.localeid.empty())
		encoding = 0x1;
	if (!val.text.empty())
		encoding |= 0x2;
	(*this) << encoding;
	if (encoding & 0x1)
		(*this) << val.localeid;
	if (encoding & 0x2)
		(*this) << val.text;
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const rx_uuid_t& val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const rx_node_id& val)
{
	uint8_t encoding = 0;
	//if (val.get_server_index() != 0)
	//	encoding = encoding | 0x40;
	//if (!val.get_namespace_uri().empty())
	//	encoding = encoding | 0x50;
	switch (val.get_node_type())
	{
	case rx_node_id_bytes:
		encoding = encoding | 5;
		(*this) << encoding;
		(*this) << val.get_namespace();
		{
			byte_string temp;
			val.get_bytes(temp);
			(*this) << temp;
		}
		break;
	case rx_node_id_uuid:
		encoding = encoding | 4;
		(*this) << encoding;
		(*this) << val.get_namespace();
		(*this) << val.get_uuid();
		break;
	case rx_node_id_string:
		encoding = encoding | 3;
		(*this) << encoding;
		(*this) << val.get_namespace();
		{
			string_type temp;
			val.get_string(temp);
			(*this) << temp;
		}
		break;
	case rx_node_id_numeric:
		{
			uint16_t nspace = val.get_namespace();
			uint32_t num_id = val.get_numeric();
			if (nspace == 0 && (num_id & 0xffffff00) == 0)
			{// two bytes
				(*this) << encoding;
				(*this) << (uint8_t)num_id;
			}
			else if ((nspace & 0xff00) == 0 && (num_id & 0xffff0000) == 0)
			{// four bytes
				encoding = encoding | 1;
				(*this) << encoding;
				(*this) << (uint8_t)nspace;
				(*this) << (uint16_t)num_id;
			}
			else
			{// regular numeric
				encoding = encoding | 2;
				(*this) << encoding;
				(*this) << nspace;
				(*this) << num_id;
			}
		}
		break;
	}
	/*if (!val.get_namespace_uri().empty())
		(*this) << val.get_namespace_uri();
	if (val.get_server_index() != 0)
		(*this) << val.get_server_index();*/
	return *this;
}
void ua_binary_ostream::serialize_extension(const ua_extension* val)
{
	if (val == nullptr)
	{
		uint16_t node_null = 0;
		uint8_t encoding = 0;
		(*this) << node_null;
		(*this) << encoding;
	}
	else
	{
		(*this) << val->binary_id;
		uint8_t encoding = 1;
		(*this) << encoding;
		val->internal_serialize_extension(*this);
	}
}
ua_binary_ostream& ua_binary_ostream::operator << (const node_class_type val)
{
	(*this) << (int)val;
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const browse_direction_type val)
{
	(*this) << (int)val;
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const timestamps_return_type val)
{
	(*this) << (int)val;
	return *this;
}

ua_binary_ostream& ua_binary_ostream::operator << (const attribute_id val)
{
	(*this) << (uint32_t)val;
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const variant_type& val)
{
	uint8_t encoding = 0;
	if (val.array_len_ >= 0)
	{// we have arrays
		encoding |= 0x80;
		if(!val.dimensions_.empty())
			encoding |= 0x40;
	}
	encoding |= val.type_;
	(*this) << encoding;
	if (encoding & 0x80)
	{// array stuff
		(*this) << val.array_len_;
		for (int i = 0; i < val.array_len_; i++)
			serialize_vunion(val.type_, val.union_.array_val[i]);
		if (encoding & 0x40)
		{// we have dimensions
			int current = 0;
			for (size_t d = 0; d < val.dimensions_.size(); d++)
				(*this) << val.dimensions_[d];
		}
	}
	else
	{ // no array
		serialize_vunion(val.type_, val.union_);
	}
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const data_value& val)
{
	uint8_t encoding = 0;
	if (!val.value.is_null())
	{
		encoding |= 0x1;
	}
	if (val.status_code != 0)
	{
		encoding |= 0x2;
	}
	if (val.source_ts.t_value != 0)
	{
		encoding |= 0x4;
	}
	if (val.server_ts.t_value != 0)
	{
		encoding |= 0x8;
	}
	if (val.source_ps != 0)
	{
		encoding |= 0x10;
	}
	if (val.server_ps != 0)
	{
		encoding |= 0x20;
	}
	(*this) << encoding;
	if (encoding & 0x1)
	{
		(*this) << val.value;
	}
	if (encoding & 0x2)
	{
		(*this) << val.status_code;
	}
	if (encoding & 0x4)
	{
		(*this) << val.source_ts;
	}
	if (encoding & 0x8)
	{
		(*this) << val.server_ts;
	}
	if (encoding & 0x10)
	{
		(*this) << val.server_ps;
	}
	if (encoding & 0x20)
	{
		(*this) << val.server_ps;
	}
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (const qualified_name& val)
{
	(*this) << val.namesp;
	(*this) << val.name;
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (char val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (int64_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (uint64_t val)
{
	auto result = buffer_->write_to_buffer(val);
	return *this;
}
ua_binary_ostream& ua_binary_ostream::operator << (change_trigger_type val)
{
	(*this) << (int)val;
	return *this;
}
void ua_binary_ostream::serialize_vunion(uint8_t type, const vunion_type& vu)
{
	switch (type)
	{
	case opcid_Boolean:
		(*this) << vu.b_val;
		break;
	case opcid_SByte:
		(*this) << vu.sc_val;
		break;
	case opcid_Byte:
		(*this) << vu.b_val;
		break;
	case opcid_Int16:
		(*this) << vu.sw_val;
		break;
	case opcid_UInt16:
		(*this) << vu.w_val;
		break;
	case opcid_Int32:
		(*this) << vu.sdw_val;
		break;
	case opcid_UInt32:
		(*this) << vu.dw_val;
		break;
	case opcid_Int64:
		(*this) << vu.sqw_val;
		break;
	case opcid_UInt64:
		(*this) << vu.qw_val;
		break;
	case opcid_Float:
		(*this) << vu.f_val;
		break;
	case opcid_Double:
		(*this) << vu.d_val;
		break;
	case opcid_String:
		(*this) << (*vu.str_val);
		break;
	case opcid_DateTime:
		(*this) << vu.ft_val;
		break;
	case opcid_Guid:
		(*this) << (*vu.guid_val);
		break;
	case opcid_ByteString:
		(*this) << (*vu.bstr_val);
		break;
	case opcid_XmlElement:
		(*this) << (*vu.str_val);
		break;
	case opcid_NodeId:
		(*this) << (*vu.node_val);
		break;
	case opcid_ExpandedNodeId:
		(*this) << (*vu.node_val);
		break;
	case opcid_StatusCode:
		(*this) << vu.dw_val;
		break;
	case opcid_QualifiedName:
		(*this) << (*vu.qname_val);
		break;
	case opcid_LocalizedText:
		(*this) << (*vu.ltext_val);
		break;
	case opcid_Structure:
		serialize_extension(vu.ext_val);
		break;
	case opcid_DataValue:
		(*this) << (*vu.data_val);
		break;
	case opcid_BaseDataType:
		(*this) << (*vu.var_val);
		break;
	case opcid_DiagnosticInfo:
		(*this) << (*vu.di_val);
		break;
	}
}





} //namespace opcua
} //namespace binary
} //namespace protocols
