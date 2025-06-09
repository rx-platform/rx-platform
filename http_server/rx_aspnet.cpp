

/****************************************************************************
*
*  http_server\rx_aspnet.cpp
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


// rx_aspnet
#include "http_server/rx_aspnet.h"

#include "sys_internal/rx_security/rx_aspnet_security.h"
#include "protocols/http/rx_http_mapping.h"

#include "third-party/rapidjson/include/rapidjson/document.h"
#include "third-party/rapidjson/include/rapidjson/writer.h"


namespace rx_internal {

namespace rx_http_server {

namespace aspnet {
namespace
{
string_type get_service_endpoint()
{
#ifndef _WIN32
    return "";
#else
	string_type static_ret;
	if (!static_ret.empty())
		return static_ret;
	string_type auth_svc_url = "unknown";
	string_type role_svc_url = "unknown";
	static char buff[MAX_PATH + 1];
	HKEY hkey = NULL;
	if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Informatika\\WebSec", 0, KEY_READ, &hkey))
	{
		DWORD type = 0;
		DWORD size = (DWORD)sizeof(buff);
		if (ERROR_SUCCESS == RegQueryValueExA(hkey, "AuthSvcUrl", NULL, &type, (BYTE*)buff, &size) && type == REG_SZ)
		{
			auth_svc_url =buff;
		}
		type = 0;
		size = (DWORD)sizeof(buff);
		if (ERROR_SUCCESS == RegQueryValueExA(hkey, "RolSvcUrl", NULL, &type, (BYTE*)buff, &size) && type == REG_SZ)
		{
			role_svc_url = buff;
		}

		RegCloseKey(hkey);
	}
	size_t idx = auth_svc_url.find("://");
	if (idx != string_type::npos)
	{
		auto idx1 = auth_svc_url.find('/', idx + 3);
		if (idx1 != string_type::npos)
			auth_svc_url = auth_svc_url.substr(idx + 3, idx1 - idx - 3);
		else
			auth_svc_url = auth_svc_url.substr(idx + 3);
	}
	static_ret = auth_svc_url;
	return static_ret;
#endif
}

string_type get_logon_service_endpoint()
{
#ifndef _WIN32
	return "";
#else
	string_type static_ret;
	if (!static_ret.empty())
		return static_ret;
	string_type auth_svc_url = "unknown";
	string_type role_svc_url = "unknown";
	static char buff[MAX_PATH + 1];
	HKEY hkey = NULL;
	if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Informatika\\WebSec", 0, KEY_READ, &hkey))
	{
		DWORD type = 0;
		DWORD size = (DWORD)sizeof(buff);
		if (ERROR_SUCCESS == RegQueryValueExA(hkey, "AuthSvcUrl", NULL, &type, (BYTE*)buff, &size) && type == REG_SZ)
		{
			auth_svc_url = buff;
		}
		type = 0;
		size = (DWORD)sizeof(buff);
		if (ERROR_SUCCESS == RegQueryValueExA(hkey, "RolSvcUrl", NULL, &type, (BYTE*)buff, &size) && type == REG_SZ)
		{
			role_svc_url = buff;
		}

		RegCloseKey(hkey);
	}
	static_ret = auth_svc_url;
	return static_ret;
#endif
}
} // namespace

// Class rx_internal::rx_http_server::aspnet::aspnet_authorizer 


rx_result aspnet_authorizer::handle_request_after (http_request& req, http_response& resp)
{

	return true;
}

rx_result aspnet_authorizer::handle_request_before(http_request& req, http_response& resp)
{
	auto it = req.headers.find("Cookie");
	if (it != req.headers.end())
	{
		string_type session;
		string_type cookie;
		string_type token;
		const string_type asp_session = "ASP.NET_SessionId";
		const string_type auth = ".PLANTARCHITECT";
		const string_type auth_svc = ".ASPXAUTH";
		const string_type rx_rnp = ".rnpmes";

		string_type rxcookie;
		auto idx = it->second.find(rx_rnp);
		if (idx != string_type::npos)
		{

			idx = it->second.find('=', idx + rx_rnp.length());
			if (idx == string_type::npos)
				return true;
			auto idx2 = it->second.find(';', idx + 1);
			if (idx2 == string_type::npos)
				rxcookie = it->second.substr(idx + 1);
			else
				rxcookie = it->second.substr(idx + 1, idx2 - idx - 1);

			if (!rxcookie.empty())
			{
				bytes_value_struct data;
				if (RX_OK == rx_base64_get_data(&data, rxcookie.c_str()))
				{
					string_type user;
					string_view_type cookie_str((const char*)data.value, data.size);
					string_type host = get_service_endpoint();
					idx = cookie_str.find('\\');
					if (idx != string_view_type::npos)
					{
						user = cookie_str.substr(0, idx);
						token = cookie_str.substr(idx + 1);
						auto sec = rx_create_reference<rx_security::aspnet_forms::aspnet_security_context>(user, host, token, false);
						req.request_identity = sec;
					}
				}
			}

		}
		else
		{
			idx = it->second.find(asp_session);
			if (idx != string_type::npos)
			{
				idx = it->second.find('=', idx + asp_session.length());
				if (idx == string_type::npos)
					return true;
				auto idx2 = it->second.find(';', idx + 1);
				if (idx2 == string_type::npos)
					session = it->second.substr(idx);
				else
					session = it->second.substr(idx, idx2 - idx);
			}
			idx = it->second.find(auth);
			if (idx == string_type::npos)
				return true;
			idx = it->second.find('=', idx + auth.length());
			if (idx == string_type::npos)
				return true;
			auto idx2 = it->second.find(';', idx + 1);
			cookie = auth_svc + "=";
			if (idx2 == string_type::npos)
				cookie += it->second.substr(idx + 1);
			else
				cookie += it->second.substr(idx + 1, idx2 - idx);

			//	cookie = replace_in_string(cookie, ".PLANTARCHITECT", ".ASPXAUTH");
			auto it_param = req.params.find("__reguser");
			if (it_param != req.params.end())
			{
				string_type user = it_param->second;
				string_type host = get_service_endpoint();
				req.params.erase(it_param);

				auto sec = rx_create_reference<rx_security::aspnet_forms::aspnet_security_context>(user, host, cookie, false);
				req.request_identity = sec;
			}

		}	

	}
	return true;

}


// Class rx_internal::rx_http_server::aspnet::aspnet_logon_display 

aspnet_logon_display::aspnet_logon_display()
{
}


aspnet_logon_display::~aspnet_logon_display()
{
}



rx_result aspnet_logon_display::initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result aspnet_logon_display::deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path)
{
	return true;
}

rx_result aspnet_logon_display::handle_request (rx_platform::http::http_request& req, rx_platform::http::http_response& resp)
{
	if (req.method == rx_http_method::post)
	{
		resp.headers["Set-Cookie"] = ".rnpmes=;";
		string_type str(req.get_content_as_string());
		if (str.size() < 2)
		{
			bool ret = rx_internal::rx_security::aspnet_forms::aspnet_security_provider::logoff();
			if (ret)
			{
				resp.set_string_content(rx_create_string("{ \"success\" : true }"));
			}
			else
			{
				resp.set_string_content(rx_create_string("{ \"success\" : false, \"message\" : \"", RX_ACCESS_DENIED, "\" }"));
			}
			resp.headers["Content-Type"] = "application/json";
			resp.result = 200;

			return true;
		}
		else
		{
			rapidjson::Document doc;
			doc.Parse(str.c_str());
			if (doc.IsObject())
			{
				int cnt = doc.MemberCount();
				if (cnt == 2)
				{
					std::map<string_type, string_type> params;
					auto it = doc.MemberBegin();
					for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); it++)
					{
						if (it->value.GetType() != rapidjson::kStringType)
							return "Invalid parameters";
						params[it->name.GetString()] = it->value.GetString();
					}

					string_type name;
					string_type val;
					auto it_name = params.find("user");
					if (it_name != params.end())
					{
						name = it_name->second;
						if (name.empty())
							return "Invalid parameters";
					}
					else
						return "Invalid parameters";

					auto it_val = params.find("pass");
					if (it_val != params.end())
					{
						val = it_val->second;
						if (val.empty())
							return "Invalid parameters";
					}
					else
						return "Invalid parameters";

					string_type ret = rx_internal::rx_security::aspnet_forms::aspnet_security_provider::logon(name, val);
					if (!ret.empty())
					{
						resp.set_string_content(rx_create_string("{ \"success\" : true }"));
						string_value_struct str;
						if (RX_OK == rx_base64_get_string(&str, (uint8_t*)&ret[0], ret.size()))
						{
							resp.headers["Set-Cookie"] = ".rnpmes="s + rx_c_str(&str);
						}
					}
					else
					{
						resp.headers["Set-Cookie"] = ".rnpmes=;";
						resp.set_string_content(rx_create_string("{ \"success\" : false, \"message\" : \"", RX_ACCESS_DENIED, "\" }"));
					}
					resp.headers["Content-Type"] = "application/json";
					resp.result = 200;

					return true;

				}
			}
			return "Invalid parameters";
		}
	}
	return "Invalid Verb";
}


} // namespace aspnet
} // namespace rx_http_server
} // namespace rx_internal

