

/****************************************************************************
*
*  sys_internal\rx_set_messages.cpp
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_set_messages
#include "sys_internal/rx_set_messages.h"

#include "sys_internal/rx_internal_protocol.h"
#include "system/serialization/rx_ser.h"
#include "api/rx_namespace_api.h"
#include "api/rx_meta_api.h"
#include "system/server/rx_server.h"


namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace set_messages {

// Parameterized Class sys_internal::rx_protocol::messages::set_messages::protocol_simple_type_creator 


template <class itemT>
message_ptr protocol_simple_type_creator<itemT>::do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create)
{
	auto ret_value = std::make_unique<error_message>("Not implemented"s, 21, request);
	return ret_value;
}

template <class itemT>
rx_result protocol_simple_type_creator<itemT>::serialize (base_meta_writer& stream) const
{
	auto result = item->serialize_definition(stream, STREAMING_TYPE_TYPE);
	if (!result)
		return result;
	return true;
}

template <class itemT>
rx_result protocol_simple_type_creator<itemT>::deserialize (base_meta_reader& stream, const meta::meta_data& meta)
{
	auto result = item->deserialize_definition(stream, STREAMING_TYPE_TYPE);
	if (!result)
		return result;
	item->meta_info() = meta;
	return result;
}


// Parameterized Class sys_internal::rx_protocol::messages::set_messages::protocol_type_creator 


template <class itemT>
message_ptr protocol_type_creator<itemT>::do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create)
{
	rx_node_id id = rx_node_id::null_id;

	auto callback = [create, request, port](rx_result_with<typename itemT::smart_ptr>&& result) mutable
	{
		if (result)
		{
			if (create)
			{
				auto response = std::make_unique<set_type_response<itemT> >();
				response->item = result.value();
				response->request_id = request;
				port->data_processed(std::move(response));
			}
			else
			{
				auto response = std::make_unique<update_type_response<itemT> >();
				response->item = result.value();
				response->request_id = request;
				port->data_processed(std::move(response));
			}
		}
		else
		{
			auto ret_value = std::make_unique<error_message>(result, 14, request);
			port->data_processed(std::move(ret_value));
		}

	};
	rx_result result;
	if(create)
		result = api::meta::rx_create_type<itemT>("", "", item, namespace_item_attributes::namespace_item_null, callback, ctx);
	else
		result = api::meta::rx_update_type<itemT>(item, callback, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request);
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}

template <class itemT>
rx_result protocol_type_creator<itemT>::serialize (base_meta_writer& stream) const
{
	auto result = item->serialize_definition(stream, STREAMING_TYPE_TYPE);
	if (!result)
		return result;
	return true;
}

template <class itemT>
rx_result protocol_type_creator<itemT>::deserialize (base_meta_reader& stream, const meta::meta_data& meta)
{
	item = rx_create_reference<itemT>();
	auto result = item->deserialize_definition(stream, STREAMING_TYPE_TYPE);
	if (!result)
		return result;
	item->meta_info() = meta;
	return result;
}


// Class sys_internal::rx_protocol::messages::set_messages::protocol_type_creator_base 

protocol_type_creator_base::~protocol_type_creator_base()
{
}



// Class sys_internal::rx_protocol::messages::set_messages::set_type_request 

string_type set_type_request::type_name = "setTypeReq";

uint16_t set_type_request::type_id = rx_set_type_request_id;


rx_result set_type_request::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	if (!creator_)
		return "Message not defined!";
	auto result = creator_->serialize(stream);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

rx_result set_type_request::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_TYPE, target_type);
	if (!result)
		return result;
	switch (target_type)
	{
		// object types
	case rx_item_type::rx_object_type:
		creator_ = std::make_unique<protocol_type_creator<object_types::object_type> >();
		break;
	case rx_item_type::rx_port_type:
		creator_ = std::make_unique<protocol_type_creator<object_types::port_type> >();
		break;
	case rx_item_type::rx_application_type:
		creator_ = std::make_unique<protocol_type_creator<object_types::application_type> >();
		break;
	case rx_item_type::rx_domain_type:
		creator_ = std::make_unique<protocol_type_creator<object_types::domain_type> >();
		break;
		// simple types
	case rx_item_type::rx_struct_type:
		creator_ = std::make_unique<protocol_simple_type_creator<basic_types::struct_type> >();
		break;
	case rx_item_type::rx_variable_type:
		creator_ = std::make_unique<protocol_simple_type_creator<basic_types::variable_type> >();
		break;
		// variable sub-types
	case rx_item_type::rx_source_type:
		creator_ = std::make_unique<protocol_simple_type_creator<basic_types::source_type> >();
		break;
	case rx_item_type::rx_filter_type:
		creator_ = std::make_unique<protocol_simple_type_creator<basic_types::filter_type> >();
		break;
	case rx_item_type::rx_event_type:
		creator_ = std::make_unique<protocol_simple_type_creator<basic_types::event_type> >();
		break;
		// mappings
	case rx_item_type::rx_mapper_type:
		creator_ = std::make_unique<protocol_simple_type_creator<basic_types::mapper_type> >();
		break;
	default:
		return "Unknown type: "s + rx_item_type_name(target_type);
	}
	RX_ASSERT(creator_);
	result = creator_->deserialize(stream, meta);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

message_ptr set_type_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	if (creator_)
	{
		return creator_->do_job(ctx, port, request_id, true);
	}
	else
	{
		auto ret_value = std::make_unique<error_message>("Message not defined!"s, 13, request_id);
		return ret_value;
	}
}

const string_type& set_type_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t set_type_request::get_type_id ()
{
  return type_id;

}


// Parameterized Class sys_internal::rx_protocol::messages::set_messages::set_type_response 

template <class itemT>
string_type set_type_response<itemT>::type_name = "setTypeResp";

template <class itemT>
uint16_t set_type_response<itemT>::type_id = rx_set_type_response_id;


template <class itemT>
const string_type& set_type_response<itemT>::get_type_name ()
{
  return type_name;

}

template <class itemT>
rx_message_type_t set_type_response<itemT>::get_type_id ()
{
  return type_id;

}


// Parameterized Class sys_internal::rx_protocol::messages::set_messages::update_type_response 

template <class itemT>
string_type update_type_response<itemT>::type_name = "updateTypeResp";

template <class itemT>
uint16_t update_type_response<itemT>::type_id = rx_update_type_response_id;


template <class itemT>
const string_type& update_type_response<itemT>::get_type_name ()
{
  return type_name;

}

template <class itemT>
rx_message_type_t update_type_response<itemT>::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::set_messages::update_type_request 

string_type update_type_request::type_name = "updateTypeReq";

uint16_t update_type_request::type_id = rx_update_type_request_id;


rx_result update_type_request::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	if (!updater_)
		return "Message not defined!";
	auto result = updater_->serialize(stream);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

rx_result update_type_request::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_TYPE, target_type);
	if (!result)
		return result;
	switch (target_type)
	{
		// object types
	case rx_item_type::rx_object_type:
		updater_ = std::make_unique<protocol_type_creator<object_types::object_type> >();
		break;
	case rx_item_type::rx_port_type:
		updater_ = std::make_unique<protocol_type_creator<object_types::port_type> >();
		break;
	case rx_item_type::rx_application_type:
		updater_ = std::make_unique<protocol_type_creator<object_types::application_type> >();
		break;
	case rx_item_type::rx_domain_type:
		updater_ = std::make_unique<protocol_type_creator<object_types::domain_type> >();
		break;
		// simple types
	case rx_item_type::rx_struct_type:
		updater_ = std::make_unique<protocol_simple_type_creator<basic_types::struct_type> >();
		break;
	case rx_item_type::rx_variable_type:
		updater_ = std::make_unique<protocol_simple_type_creator<basic_types::variable_type> >();
		break;
		// variable sub-types
	case rx_item_type::rx_source_type:
		updater_ = std::make_unique<protocol_simple_type_creator<basic_types::source_type> >();
		break;
	case rx_item_type::rx_filter_type:
		updater_ = std::make_unique<protocol_simple_type_creator<basic_types::filter_type> >();
		break;
	case rx_item_type::rx_event_type:
		updater_ = std::make_unique<protocol_simple_type_creator<basic_types::event_type> >();
		break;
		// mappings
	case rx_item_type::rx_mapper_type:
		updater_ = std::make_unique<protocol_simple_type_creator<basic_types::mapper_type> >();
		break;
	default:
		return "Unknown type: "s + rx_item_type_name(target_type);
	}
	RX_ASSERT(updater_);
	result = updater_->deserialize(stream, meta);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

message_ptr update_type_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	if (updater_)
	{
		return updater_->do_job(ctx, port, request_id, false);
	}
	else
	{
		auto ret_value = std::make_unique<error_message>("Message not defined!"s, 13, request_id);
		return ret_value;
	}
}

const string_type& update_type_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t update_type_request::get_type_id ()
{
  return type_id;

}


// Parameterized Class sys_internal::rx_protocol::messages::set_messages::set_runtime_response 

template <class itemT>
string_type set_runtime_response<itemT>::type_name = "updateRuntimeResp";

template <class itemT>
uint16_t set_runtime_response<itemT>::type_id = rx_update_runtime_response_id;


template <class itemT>
const string_type& set_runtime_response<itemT>::get_type_name ()
{
  return type_name;

}

template <class itemT>
rx_message_type_t set_runtime_response<itemT>::get_type_id ()
{
  return type_id;

}


// Parameterized Class sys_internal::rx_protocol::messages::set_messages::update_runtime_response 

template <class itemT>
string_type update_runtime_response<itemT>::type_name = "updateRuntimeResp";

template <class itemT>
uint16_t update_runtime_response<itemT>::type_id = rx_set_runtime_response_id;


template <class itemT>
const string_type& update_runtime_response<itemT>::get_type_name ()
{
  return type_name;

}

template <class itemT>
rx_message_type_t update_runtime_response<itemT>::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::set_messages::set_runtime_request 

string_type set_runtime_request::type_name = "setRuntimeReq";

uint16_t set_runtime_request::type_id = rx_set_runtime_request_id;


rx_result set_runtime_request::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	if (!creator_)
		return "Message not defined!";
	auto result = creator_->serialize(stream);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

rx_result set_runtime_request::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_OBJECT, target_type);
	if (!result)
		return result;
	switch (target_type)
	{
		// object types
	case rx_item_type::rx_object:
		creator_ = std::make_unique<protocol_runtime_creator<object_types::object_type> >();
		break;
	case rx_item_type::rx_port:
		creator_ = std::make_unique<protocol_runtime_creator<object_types::port_type> >();
		break;
	case rx_item_type::rx_application:
		creator_ = std::make_unique<protocol_runtime_creator<object_types::application_type> >();
		break;
	case rx_item_type::rx_domain:
		creator_ = std::make_unique<protocol_runtime_creator<object_types::domain_type> >();
		break;
	default:
		return "Unknown type: "s + rx_item_type_name(target_type);
	}
	RX_ASSERT(creator_);
	result = creator_->deserialize(stream, meta);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

message_ptr set_runtime_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	if (creator_)
	{
		return creator_->do_job(ctx, port, request_id, true);
	}
	else
	{
		auto ret_value = std::make_unique<error_message>("Message not defined!"s, 13, request_id);
		return ret_value;
	}
}

const string_type& set_runtime_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t set_runtime_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::set_messages::update_runtime_request 

string_type update_runtime_request::type_name = "updateRuntimeReq";

uint16_t update_runtime_request::type_id = rx_update_runtime_request_id;


rx_result update_runtime_request::serialize (base_meta_writer& stream) const
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	if (!updater_)
		return "Message not defined!";
	auto result = updater_->serialize(stream);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

rx_result update_runtime_request::deserialize (base_meta_reader& stream)
{
	if (!stream.start_object("item"))
		return "Error starting item object";

	meta::meta_data meta;
	rx_item_type target_type;
	auto result = meta.deserialize_meta_data(stream, STREAMING_TYPE_OBJECT, target_type);
	if (!result)
		return result;
	switch (target_type)
	{
		// object types
	case rx_item_type::rx_object:
		updater_ = std::make_unique<protocol_runtime_creator<object_types::object_type> >();
		break;
	case rx_item_type::rx_port:
		updater_ = std::make_unique<protocol_runtime_creator<object_types::port_type> >();
		break;
	case rx_item_type::rx_application:
		updater_ = std::make_unique<protocol_runtime_creator<object_types::application_type> >();
		break;
	case rx_item_type::rx_domain:
		updater_ = std::make_unique<protocol_runtime_creator<object_types::domain_type> >();
		break;
	default:
		return "Unknown type: "s + rx_item_type_name(target_type);
	}
	RX_ASSERT(updater_);
	result = updater_->deserialize(stream, meta);
	if (!result)
		return result;

	if (!stream.end_object())
		return "Error ending item object";

	return true;
}

message_ptr update_runtime_request::do_job (api::rx_context ctx, rx_protocol_port_ptr port)
{
	if (updater_)
	{
		return updater_->do_job(ctx, port, request_id, false);
	}
	else
	{
		auto ret_value = std::make_unique<error_message>("Message not defined!"s, 13, request_id);
		return ret_value;
	}
}

const string_type& update_runtime_request::get_type_name ()
{
  return type_name;

}

rx_message_type_t update_runtime_request::get_type_id ()
{
  return type_id;

}


// Class sys_internal::rx_protocol::messages::set_messages::protocol_runtime_creator_base 


// Parameterized Class sys_internal::rx_protocol::messages::set_messages::protocol_runtime_creator 


template <class itemT>
message_ptr protocol_runtime_creator<itemT>::do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create)
{
	rx_node_id id = rx_node_id::null_id;

	auto callback = [create, request, port](rx_result_with<typename itemT::RTypePtr>&& result) mutable
	{
		if (result)
		{
			if (create)
			{
				auto response = std::make_unique<set_runtime_response<itemT> >();
				response->item = result.value();
				response->request_id = request;
				port->data_processed(std::move(response));
			}
			else
			{
				auto response = std::make_unique<update_runtime_response<itemT> >();
				response->item = result.value();
				response->request_id = request;
				port->data_processed(std::move(response));
			}
		}
		else
		{
			auto ret_value = std::make_unique<error_message>(result, 14, request);
			port->data_processed(std::move(ret_value));
		}

	};
	rx_result result;

	meta_.increment_version(false);
	if (create)
		result = api::meta::rx_create_runtime<itemT>(meta_, &values_, instance_data_, callback, ctx);
	else
		result = api::meta::rx_update_runtime<itemT>(meta_, &values_, instance_data_, callback, ctx);

	if (!result)
	{
		auto ret_value = std::make_unique<error_message>(result, 13, request);
		return ret_value;
	}
	else
	{
		// just return we send callback
		return message_ptr();
	}
}

template <class itemT>
rx_result protocol_runtime_creator<itemT>::serialize (base_meta_writer& stream) const
{
	return "Jbg nema jos!!!";
}

template <class itemT>
rx_result protocol_runtime_creator<itemT>::deserialize (base_meta_reader& stream, const meta::meta_data& meta)
{
	bool ret = false;
	if (stream.start_object("def"))
	{
		if (stream.read_init_values("values", values_))
		{
			if (instance_data_.deserialize(stream, STREAMING_TYPE_OBJECT))
			{
				meta_ = meta;
				ret = true;
			}
		}
	}
	return ret;
}


} // namespace set_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal

