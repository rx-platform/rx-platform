

/****************************************************************************
*
*  protocols\opcua\rx_opcua_session.cpp
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


// rx_opcua_session
#include "protocols/opcua/rx_opcua_session.h"

#include "rx_opcua_identifiers.h"
using namespace protocols::opcua::ids;

#include "rx_opcua_server.h"


namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_session {

// Class protocols::opcua::requests::opcua_session::opcua_create_session 


rx_node_id opcua_create_session::get_binary_request_id ()
{
	return rx_node_id(opcid_CreateSessionRequest_Encoding_DefaultBinary, 0);
}

opcua_request_ptr opcua_create_session::create_empty () const
{
	return std::make_unique<opcua_create_session>();
}

rx_result opcua_create_session::deserialize_binary (binary::ua_binary_istream& stream)
{
	client_description.deserialize(stream);
	stream >> server_uri;
	stream >> endpoint_url;
	stream >> session_name;
	stream >> client_nounce;
	stream >> client_certificate;
	stream >> session_timeout;
	stream >> max_message_size;
	return true;
}

opcua_response_ptr opcua_create_session::do_job (opcua_server_endpoint_ptr ep)
{
	auto ret_ptr = std::make_unique<opcua_create_session_response>(*this);
	rx_node_id session_id = rx_node_id::generate_new(0);
	ret_ptr->session_id = session_id;
	ret_ptr->authentication_token = rx_node_id("ime", 0);
	ret_ptr->session_timeout = session_timeout;
	ret_ptr->server_nounce.clear();
	ret_ptr->server_certificate.clear();
	ret_ptr->max_message_size = max_message_size;
	ret_ptr->endpoints.push_back(ep->get_endpoint_description(endpoint_url, false));

	return ret_ptr;
}


// Class protocols::opcua::requests::opcua_session::opcua_create_session_response 

opcua_create_session_response::opcua_create_session_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_create_session_response::get_binary_response_id ()
{
	return rx_node_id(opcid_CreateSessionResponse_Encoding_DefaultBinary, 0);
}

opcua_response_ptr opcua_create_session_response::create_empty () const
{
	return std::make_unique<opcua_create_session_response>();
}

rx_result opcua_create_session_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream << session_id;
	stream << authentication_token;
	stream << session_timeout;
	stream << server_nounce;
	stream << server_certificate;
	stream.serialize_array(endpoints);
	stream << (int32_t)-1;// server software certificates
	stream << signature_algorithm;
	stream << signature_data;
	stream << max_message_size;

	return true;
}


// Class protocols::opcua::requests::opcua_session::opcua_activate_session 


rx_node_id opcua_activate_session::get_binary_request_id ()
{
	return rx_node_id(opcid_ActivateSessionRequest_Encoding_DefaultBinary, 0);
}

opcua_request_ptr opcua_activate_session::create_empty () const
{
	return std::make_unique<opcua_activate_session>();
}

rx_result opcua_activate_session::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> signature_algorithm;
	stream >> signature_data;
	int certs = -1; // client software certificates
	stream >> certs;
	RX_ASSERT(certs <= 0);// not used, reserved, has to be empty array
	stream >> locale_ids;
	identity_token = stream.deserialize_extension<opcua_identity_token>([](const rx_node_id& id) -> identity_token_ptr {
			static rx_node_id anonymus_id = rx_node_id::opcua_standard_id(opcid_AnonymousIdentityToken_Encoding_DefaultBinary);
			if (id == anonymus_id)
				return std::make_unique<opcua_anonymus_identity_token>();
			else
				return identity_token_ptr();
		});
	stream >> token_signature_algorithm;
	stream >> token_signature_data;
	return true;
}

opcua_response_ptr opcua_activate_session::do_job (opcua_server_endpoint_ptr ep)
{
	auto ret =  std::make_unique<opcua_activate_session_response>(*this);
	return ret;
}

opcua_identity_token::opcua_identity_token(rx_node_id class_id, rx_node_id binary_id, rx_node_id xml_id)
	: common::ua_extension(class_id, binary_id, xml_id)
{
}
opcua_anonymus_identity_token::opcua_anonymus_identity_token()
	: opcua_identity_token(rx_node_id::opcua_standard_id(opcid_AnonymousIdentityToken)
		, rx_node_id::opcua_standard_id(opcid_AnonymousIdentityToken_Encoding_DefaultBinary)
		, rx_node_id::opcua_standard_id(opcid_AnonymousIdentityToken_Encoding_DefaultXml))
{

}
opcua_extension_ptr opcua_anonymus_identity_token::make_copy()
{
	auto ret = std::make_unique< opcua_anonymus_identity_token>();
	return ret;
}
void opcua_anonymus_identity_token::internal_serialize_extension(binary::ua_binary_ostream& stream) const
{
	stream << "Anonymous";
}
void opcua_anonymus_identity_token::internal_deserialize_extension(binary::ua_binary_istream& stream)
{
	string_type policy_id;
	stream >> policy_id;
	RX_ASSERT(policy_id == "Anonymous");
}
// Class protocols::opcua::requests::opcua_session::opcua_close_session 


rx_node_id opcua_close_session::get_binary_request_id ()
{
	return rx_node_id(opcid_CloseSessionRequest_Encoding_DefaultBinary, 0);
}

opcua_request_ptr opcua_close_session::create_empty () const
{
	return std::make_unique<opcua_close_session>();
}

rx_result opcua_close_session::deserialize_binary (binary::ua_binary_istream& stream)
{
	stream >> delete_subscriptions;
	return true;
}

opcua_response_ptr opcua_close_session::do_job (opcua_server_endpoint_ptr ep)
{
	auto ret = std::make_unique<opcua_close_session_response>(*this);
	return ret;
}


// Class protocols::opcua::requests::opcua_session::opcua_activate_session_response 

opcua_activate_session_response::opcua_activate_session_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_activate_session_response::get_binary_response_id ()
{
	return rx_node_id(opcid_ActivateSessionResponse_Encoding_DefaultBinary, 0);
}

opcua_response_ptr opcua_activate_session_response::create_empty () const
{
	return std::make_unique<opcua_activate_session_response>();
}

rx_result opcua_activate_session_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	stream << server_nounce;
	stream << (int32_t)-1;// software certificates results
	stream << (int32_t)-1;// software certificates diagnostics info

	return true;
}


// Class protocols::opcua::requests::opcua_session::opcua_close_session_response 

opcua_close_session_response::opcua_close_session_response (const opcua_request_base& req)
	: opcua_response_base(req)
{
}



rx_node_id opcua_close_session_response::get_binary_response_id ()
{
	return rx_node_id(opcid_CloseSessionResponse_Encoding_DefaultBinary, 0);
}

opcua_response_ptr opcua_close_session_response::create_empty () const
{
	return std::make_unique<opcua_close_session_response>();
}

rx_result opcua_close_session_response::serialize_binary (binary::ua_binary_ostream& stream) const
{
	// empty message
	return true;
}


} // namespace opcua_session
} // namespace requests
} // namespace opcua
} // namespace protocols

