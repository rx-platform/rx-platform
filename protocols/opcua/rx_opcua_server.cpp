

/****************************************************************************
*
*  protocols\opcua\rx_opcua_server.cpp
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


// rx_opcua_server
#include "protocols/opcua/rx_opcua_server.h"

#include "system/server/rx_server.h"

using namespace protocols::opcua::common;


namespace protocols {

namespace opcua {

namespace opcua_server {

// Class protocols::opcua::opcua_server::opcua_server_endpoint_base 

opcua_server_endpoint_base::opcua_server_endpoint_base (const string_type& endpoint_url, const string_type& app_name, const string_type& app_uri, opcua_addr_space::opcua_address_space_base* addr_space)
      : app_name_(app_name),
        app_uri_(app_uri),
        endpoint_url_(endpoint_url),
        address_space_(addr_space)
{
}



common::endpoint_description opcua_server_endpoint_base::get_endpoint_description (const string_type& ep_url)
{
	endpoint_description ep_descr;
	ep_descr.url = ep_url;
	ep_descr.application = get_application_description(ep_url);

	ep_descr.security_mode = security_mode_t::none;
	ep_descr.policy_uri = "http://opcfoundation.org/UA/SecurityPolicy#None";
	ep_descr.security_level = 0;
	ep_descr.transport_profile_uri = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";

	user_token_policy pol;
	pol.policy_id = "Anonymous";
	pol.token_type = 0;

	ep_descr.user_tokens.push_back(std::move(pol));

	return ep_descr;
}

common::application_description opcua_server_endpoint_base::get_application_description (const string_type& ep_url)
{
	application_description app_descr;
	app_descr.application_uri = app_uri_;
	app_descr.application_name.text = app_name_;
	app_descr.product_uri = "rx-platform.org/"s + rx_gate::instance().get_rx_version();
	app_descr.application_type = application_type_t::server_application_type;
	app_descr.discovery_urls.push_back(ep_url);

	return app_descr;
}

opcua_addr_space::opcua_address_space_base* opcua_server_endpoint_base::get_address_space ()
{
	return address_space_;
}


} // namespace opcua_server
} // namespace opcua
} // namespace protocols

