

/****************************************************************************
*
*  protocols\opcua\rx_opcua_params.cpp
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


// rx_opcua_params
#include "protocols/opcua/rx_opcua_params.h"

//#include "rx_opcua_build_nodes2Types.h"
#include "protocols/opcua/rx_opcua_binary.h"
#include "protocols/opcua/rx_opcua_identifiers.h"
#include "lib/rx_values.h"
#include "rx_opcua_addr_space.h"
using namespace protocols::opcua::ids;


namespace protocols {
namespace opcua {
namespace common {



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

ua_argument::ua_argument()
	: ua_extension(rx_node_id::opcua_standard_id(opcid_Argument)
		, rx_node_id::opcua_standard_id(opcid_Argument_Encoding_DefaultBinary)
		, rx_node_id::opcua_standard_id(opcid_Argument_Encoding_DefaultXml))
		, value_rank(-1)
{
}

opcua_extension_ptr ua_argument::make_copy()
{
	auto ret = std::make_unique<ua_argument>();

	ret->name = name;
	ret->data_type = data_type;
	ret->value_rank = value_rank;
	ret->array_dimensions = array_dimensions;
	ret->description = description;

	return ret;
}


void ua_argument::internal_serialize_extension(binary::ua_binary_ostream& stream) const
{
	stream << name;
	stream << data_type;
	stream << value_rank;
	stream << array_dimensions;
	stream << description;
}
void ua_argument::internal_deserialize_extension(binary::ua_binary_istream& stream)
{
	stream >> name;
	stream >> data_type;
	stream >> value_rank;
	stream >> array_dimensions;
	stream >> description;
}


void opcua_view_description::serialize(binary::ua_binary_ostream& stream) const
{
	stream << view_id;
	stream << timestamp;
	stream << view_version;
}
void opcua_view_description::deserialize(binary::ua_binary_istream& stream)
{
	stream >> view_id;
	stream >> timestamp;
	stream >> view_version;
}


void opcua_browse_description::serialize(binary::ua_binary_ostream& stream) const
{
	stream << node_id;
	stream << (int)direction;
	stream << reference_type_id;
	stream << sub_types;
	stream << node_class_mask;
	stream << result_mask;
}
void opcua_browse_description::deserialize(binary::ua_binary_istream& stream)
{
	stream >> node_id;
	int temp = 0;
	stream >> temp;
	direction = (browse_direction_type)temp;
	stream >> reference_type_id;
	stream >> sub_types;
	stream >> node_class_mask;
	stream >> result_mask;
}

void reference_description::serialize(binary::ua_binary_ostream& stream) const
{
	stream << reference_id;
	stream << is_forward;
	stream << target_id;
	stream << browse_name;
	stream << display_name;
	stream << node_class;
	stream << type_id;
}
void reference_description::deserialize(binary::ua_binary_istream& stream)
{
	stream >> reference_id;
	stream >> is_forward;
	stream >> target_id;
	stream >> browse_name;
	stream >> display_name;
	stream >> node_class;
	stream >> type_id;
}


void browse_result_internal::add_reference_data(bool forward, const opcua_addr_space::reference_data& data, const opcua_browse_description& to_browse)
{
	auto node_ptr = data.resolved_node.lock();
	reference_description temp;
	temp.target_id = data.target_id;
	if (to_browse.result_mask & 0x01)// reference type
		temp.reference_id = data.reference_id;
	if (to_browse.result_mask & 0x02)// is forward
		temp.is_forward = forward;
	if ((to_browse.result_mask & 0x04) && node_ptr)// node class
		temp.node_class = node_ptr->get_node_class();
	if ((to_browse.result_mask & 0x08) && node_ptr)// browse name
		temp.browse_name = node_ptr->get_browse_name();
	if ((to_browse.result_mask & 0x10) && node_ptr)// display name
		temp.display_name = localized_text{ node_ptr->get_display_name() };
	if ((to_browse.result_mask & 0x20) && node_ptr)// type def
		temp.type_id = node_ptr->get_type_id();
	references.push_back(std::move(temp));
}


void opcua_browse_result::serialize(binary::ua_binary_ostream& stream) const
{
	stream << status_code;
	stream << continuation_point;
	stream.serialize_array(references);
}
void opcua_browse_result::deserialize(binary::ua_binary_istream& stream)
{
	stream >> status_code;
	stream >> continuation_point;
	stream.deserialize_array(references);
}

opcua_monitoring_filter::opcua_monitoring_filter(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id)
	: common::ua_extension(class_id, binary_id, xml_id)
{
}
opcua_data_change_filter::opcua_data_change_filter()
	: opcua_monitoring_filter(rx_node_id::opcua_standard_id(opcid_DataChangeFilter)
	, rx_node_id::opcua_standard_id(opcid_DataChangeFilter_Encoding_DefaultBinary)
	, rx_node_id::opcua_standard_id(opcid_DataChangeFilter_Encoding_DefaultXml))
	, data_change_trigger(data_change_trigger_t::STATUS)
	, deadband_type(0)
	, deadband_value(0.0)
{
}
bool opcua_data_change_filter::filter_item()
{
	return true;
}
std::unique_ptr<opcua_monitoring_filter> opcua_data_change_filter::make_filter_copy()
{
	auto ret = std::make_unique<opcua_data_change_filter>();
	ret->data_change_trigger = data_change_trigger;
	ret->deadband_type = deadband_type;
	ret->deadband_value = deadband_value;
	return ret;
}
void opcua_data_change_filter::internal_serialize_extension(binary::ua_binary_ostream& stream) const
{
	stream << (int)data_change_trigger;
	stream << deadband_type;
	stream << deadband_value;
}
void opcua_data_change_filter::internal_deserialize_extension(binary::ua_binary_istream& stream)
{
	int temp;
	stream >> temp;
	data_change_trigger = (data_change_trigger_t)temp;
	stream >> deadband_type;
	stream >> deadband_value;
}

void monitored_parameters::serialize(binary::ua_binary_ostream& stream) const
{
	stream << client_handle;
	stream << interval;
	stream.serialize_extension(filter_ptr.get());
	stream << queue_size;
	stream << discard_oldest;
}
void monitored_parameters::deserialize(binary::ua_binary_istream& stream)
{
	stream >> client_handle;
	stream >> interval;
	filter_ptr = stream.deserialize_extension<opcua_monitoring_filter>([](const rx_node_id& id) -> monitoring_filter_ptr {
		static rx_node_id data_change_id = rx_node_id::opcua_standard_id(opcid_DataChangeFilter_Encoding_DefaultBinary);

		if (id == data_change_id)
			return std::make_unique<opcua_data_change_filter>();
		else
			return monitoring_filter_ptr();
		});
	stream >> queue_size;
	stream >> discard_oldest;
}


void create_monitored_item_data::serialize(binary::ua_binary_ostream& stream) const
{
	to_monitor.serialize(stream);
	stream << mode;
	parameters.serialize(stream);
}
void create_monitored_item_data::deserialize(binary::ua_binary_istream& stream)
{
	to_monitor.deserialize(stream);
	stream >> mode;
	parameters.deserialize(stream);
}
void create_monitored_item_result::serialize(binary::ua_binary_ostream& stream) const
{
	stream << status;
	stream << server_handle;
	stream << interval;
	stream << queue_size;
	stream.serialize_extension(filter_ptr.get());
}
void create_monitored_item_result::deserialize(binary::ua_binary_istream& stream)
{
	stream >> status;
	stream >> server_handle;
	stream >> interval;
	stream >> queue_size;
	filter_ptr = stream.deserialize_extension<opcua_monitoring_filter>([](const rx_node_id& id) -> monitoring_filter_ptr {
		static rx_node_id data_change_id = rx_node_id::opcua_standard_id(opcid_DataChangeFilter_Encoding_DefaultBinary);

		if (id == data_change_id)
			return std::make_unique<opcua_data_change_filter>();
		else
			return monitoring_filter_ptr();
		});
}

void relative_path_element::serialize(binary::ua_binary_ostream& stream) const
{
	stream << reference_id;
	stream << is_inverse;
	stream << sub_types;
	stream << target_name;
}
void relative_path_element::deserialize(binary::ua_binary_istream& stream)
{
	stream >> reference_id;
	stream >> is_inverse;
	stream >> sub_types;
	stream >> target_name;
}

void relative_path::serialize(binary::ua_binary_ostream& stream) const
{
	stream.serialize_array(elements);
}
void relative_path::deserialize(binary::ua_binary_istream& stream)
{
	stream.deserialize_array(elements);
}

void browse_path::serialize(binary::ua_binary_ostream& stream) const
{
	stream << starting_node;
	path.serialize(stream);
}
void browse_path::deserialize(binary::ua_binary_istream& stream)
{
	stream >> starting_node;
	path.deserialize(stream);
}

void browse_path_target::serialize(binary::ua_binary_ostream& stream) const
{
	stream << target_id;
	stream << remaining_index;
}
void browse_path_target::deserialize(binary::ua_binary_istream& stream)
{
	stream >> target_id;
	stream >> remaining_index;
}

void browse_path_result::serialize(binary::ua_binary_ostream& stream) const
{
	stream << status_code;
	stream.serialize_array(targets);
}
void browse_path_result::deserialize(binary::ua_binary_istream& stream)
{
	stream >> status_code;
	stream.deserialize_array(targets);
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

void write_value::serialize(binary::ua_binary_ostream& stream) const
{
	stream << node_id;
	stream << attr_id;
	stream << range;
	stream << value;
}
void write_value::deserialize(binary::ua_binary_istream& stream)
{
	stream >> node_id;
	stream >> attr_id;
	stream >> range;
	stream >> value;
}



void execute_value::serialize(binary::ua_binary_ostream& stream) const
{
	stream << node_id;
	stream << method_id;
	stream << arguments;
}
void execute_value::deserialize(binary::ua_binary_istream& stream)
{
	stream >> node_id;
	stream >> method_id;
	stream >> arguments;
}


void execute_result::serialize(binary::ua_binary_ostream& stream) const
{
	stream << status_code;
	stream << input_arguments_result;
	stream<< inputs_diagnostics_info;
	stream << out_arguments;
}
void execute_result::deserialize(binary::ua_binary_istream& stream)
{
	stream >> status_code;
	stream >> input_arguments_result;
	stream >> inputs_diagnostics_info;
	stream >> out_arguments;
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




} //namespace opcua
} //namespace common
} //namespace protocols

