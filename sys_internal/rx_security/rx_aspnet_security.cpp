

/****************************************************************************
*
*  sys_internal\rx_security\rx_aspnet_security.cpp
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

/////////////////////////////////////////////////////////////
// logging macros for console library
#define ASPNET_LOG_INFO(src,lvl,msg) RX_LOG_INFO("ASPNET",src,lvl,msg)
#define ASPNET_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("ASPNET",src,lvl,msg)
#define ASPNET_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("ASPNET",src,lvl,msg)
#define ASPNET_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("ASPNET",src,lvl,msg)
#define ASPNET_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("ASPNET",src,lvl,msg)
#define ASPNET_LOG_TRACE(src,lvl,msg) RX_TRACE("ASPNET",src,lvl,msg)

#define RX_ASPFORM_SECURITY_INFO "{rx-platform} ASPNET Cookie Security Provider"


// rx_aspnet_security
#include "sys_internal/rx_security/rx_aspnet_security.h"

#include "sys_internal/rx_async_functions.h"


namespace rx_internal {

namespace rx_security {
namespace
{
const string_type g_asp_security_name = RX_ASP_SECURITY_NAME;
}

namespace aspnet_forms {
namespace
{

#ifdef _WIN32

std::wstring convert_to_unicode(const std::string& what)
{
	std::wstring ret;
	if (what.empty())
		return ret;

	wchar_t stack_buff[0x20];
	wchar_t* buff = stack_buff;
	size_t str_size = what.size();
	if (str_size + 1 > sizeof(stack_buff) / sizeof(stack_buff[0]))
		buff = new wchar_t[str_size + 1];
	if (str_size > 0)
	{
		MultiByteToWideChar(CP_UTF8, 0, what.c_str(), -1, buff, (int)str_size + 1);
		ret = buff;
	}
	if (str_size + 1 > sizeof(stack_buff) / sizeof(stack_buff[0]))
		delete[] buff;

	return ret;
}

std::string convert_from_unicode(const std::wstring& what)
{
	std::string ret;
	if (what.empty())
		return ret;

	char stack_buff[0x20];
	char* buff = stack_buff;
	size_t str_size = what.size();
	if (str_size + 1 > sizeof(stack_buff) / sizeof(stack_buff[0]))
		buff = new char[str_size + 1];
	if (str_size > 0)
	{
		WideCharToMultiByte(CP_UTF8, 0, what.c_str(), -1, buff, (int)str_size + 1, NULL, NULL);
		ret = buff;
	}
	if (str_size + 1 > sizeof(stack_buff) / sizeof(stack_buff[0]))
		delete[] buff;

	return ret;
}


HRESULT typedef(STDAPICALLTYPE* InitSecExLib_t)();
HRESULT typedef(STDAPICALLTYPE* DeinitSecExLib_t)();
HRESULT typedef(STDAPICALLTYPE* LogOnEx_t)(const wchar_t*, const wchar_t*, wchar_t*);
HRESULT typedef(STDAPICALLTYPE* LogOff_t)();
HRESULT typedef(STDAPICALLTYPE* IsInRole_t)(const wchar_t*, const wchar_t*, unsigned __int32*);
HRESULT typedef(STDAPICALLTYPE* GetTokenUserName_t)(const wchar_t*, wchar_t*);


class sec_ex_proxy
{
	HMODULE module;
	InitSecExLib_t pInitSecExLib;
	DeinitSecExLib_t pDeinitSecExLib;
	LogOnEx_t pLogOnEx;
	LogOff_t pLogOff;
	IsInRole_t pIsInRole;
	GetTokenUserName_t pGetTokenUserName;
public:
	sec_ex_proxy();
	~sec_ex_proxy();
	void initialize();
	bool log_on(const string_type& user, const string_type& pass, string_type& token);
	bool log_off();
	bool is_in_role(const string_type& role, const string_type& token);
	bool get_user_name(const string_type& token, string_type& user);
};


sec_ex_proxy::sec_ex_proxy() :
	module(NULL),
	pInitSecExLib(NULL),
	pDeinitSecExLib(NULL),
	pLogOnEx(NULL),
	pLogOff(NULL),
	pIsInRole(NULL),
	pGetTokenUserName(NULL)
{
	UINT err = SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);
	module = LoadLibrary(L"websec.dll");
	if (module == NULL)
	{// we didnt made it try blank one
		module = LoadLibrary(L"blanksec.dll");
	}
	SetErrorMode(err);
	if (module != NULL)
	{// we made it
		pInitSecExLib = (InitSecExLib_t)GetProcAddress(module, "InitSecExLib");
		pDeinitSecExLib = (DeinitSecExLib_t)GetProcAddress(module, "DeinitSecExLib");
		pLogOnEx = (LogOnEx_t)GetProcAddress(module, "LogOnEx");
		pLogOff = (LogOff_t)GetProcAddress(module, "LogOff");
		pIsInRole = (IsInRole_t)GetProcAddress(module, "IsInRole");
		pGetTokenUserName = (GetTokenUserName_t)GetProcAddress(module, "GetTokenUserName");
	}
}
sec_ex_proxy::~sec_ex_proxy()
{
	if (module != NULL)
		FreeLibrary(module);
}

void sec_ex_proxy::initialize()
{
	if (module && pInitSecExLib)
	{
		HRESULT hr = (pInitSecExLib)();
		if (FAILED(hr))
		{
			ASPNET_LOG_ERROR("sec_ex_proxy", 999, "Failed to initialize security library: " + std::to_string(hr));
		}
	}
}
bool sec_ex_proxy::log_on(const string_type& user, const string_type& pass, string_type& token)
{
	if (module && pLogOnEx)
	{
		wchar_t buff[0x200];
		if (SUCCEEDED((pLogOnEx)(convert_to_unicode(user).c_str(), convert_to_unicode(pass).c_str(), buff)))
		{
			token = convert_from_unicode(buff);
			return true;
		}
	}
	return false;
}
bool sec_ex_proxy::log_off()
{
	if (module && pLogOff)
	{
		if (SUCCEEDED((pLogOff)()))
		{
			return true;
		}
	}
	return false;
}
bool sec_ex_proxy::is_in_role(const string_type& role, const string_type& token)
{
	if (module && pIsInRole)
	{
		unsigned __int32 result = 0;
		std::wstring wrole = convert_to_unicode(role);
		std::wstring wtoken = convert_to_unicode(token);
		if (SUCCEEDED((pIsInRole)(wrole.c_str(), wtoken.c_str(), &result)))
		{
			return result != 0;
		}
	}
	return false;
}
bool sec_ex_proxy::get_user_name(const string_type& token, string_type& user)
{
	if (module && pGetTokenUserName)
	{
		wchar_t buff[0x200];
		if (SUCCEEDED((pGetTokenUserName)(convert_to_unicode(token).c_str(), buff)))
		{
			user = convert_from_unicode(buff);
			return true;
		}
	}
	return false;
}


std::unique_ptr<sec_ex_proxy> g_sec_proxy;

#endif
}

// Class rx_internal::rx_security::aspnet_forms::aspnet_security_provider

std::atomic<bool> aspnet_security_provider::initialized_ = false;


const string_type& aspnet_security_provider::get_name ()
{
	return g_asp_security_name;
}

string_type aspnet_security_provider::get_info ()
{
	return RX_ASPFORM_SECURITY_INFO;
}

rx_result aspnet_security_provider::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
#ifdef _WIN32
	g_sec_proxy = std::make_unique<sec_ex_proxy>();
#endif
	return true;
}

void aspnet_security_provider::deinitialize ()
{
#ifdef _WIN32
	g_sec_proxy.reset();
#endif
}

rx_result_with<security::security_context_ptr> aspnet_security_provider::create_host_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result_with<security::security_context_ptr> aspnet_security_provider::create_system_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result_with<security::security_context_ptr> aspnet_security_provider::create_world_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
	return RX_NOT_IMPLEMENTED;
}

string_type aspnet_security_provider::logon (const string_type& user, const string_type& pass)
{
#ifdef _WIN32
	if (initialized_)
	{
		string_type token;
		if (g_sec_proxy->log_on(user, pass, token))
			return token;
	}
#endif
	return "";
}

bool aspnet_security_provider::logoff ()
{
#ifdef _WIN32
	if (initialized_)
	{
		string_type token;
		if (g_sec_proxy->log_off())
			return true;
	}
#endif
	return false;
}

rx_result aspnet_security_provider::start (hosting::rx_platform_host* host)
{
#ifdef _WIN32
	rx_post_function_to(RX_DOMAIN_SLOW, rx_reference_ptr()
		, []() mutable
		{
			g_sec_proxy->initialize();
			if (g_sec_proxy->log_off())
			{
				ASPNET_LOG_INFO("aspnet_security_provider", 999, "Initialized ASPNET security provider");
				initialized_ = true;
			}
			else
			{
				ASPNET_LOG_ERROR("aspnet_security_provider", 999, "Failed to initialize ASPNET security provider");
			}
		});
#endif
	return true;
}

void aspnet_security_provider::stop ()
{
	initialized_ = false;
}


// Class rx_internal::rx_security::aspnet_forms::aspnet_security_context

aspnet_security_context::aspnet_security_context()
{
}

aspnet_security_context::aspnet_security_context (const string_view_type name, const string_view_type host, const string_view_type cookie, bool system)
{
	user_name_ = name;
	location_ = host;
	full_name_ = user_name_ + "@" + string_type(host);
	token_ = user_name_ + '\\' + string_type(cookie);
}


aspnet_security_context::~aspnet_security_context()
{
}



bool aspnet_security_context::is_authenticated () const
{
	return !token_.empty();
}

rx_result aspnet_security_context::serialize (base_meta_writer& stream) const
{
	if (!stream.write_string("name", user_name_.c_str()))
		return stream.get_error();
	if (!stream.write_string("token", token_.c_str()))
		return stream.get_error();
	if (!stream.write_string("host", token_.c_str()))
		return stream.get_error();
	return true;
}

rx_result aspnet_security_context::deserialize (base_meta_reader& stream)
{
	if (!stream.read_string("name", user_name_))
		return stream.get_error();
	if (!stream.read_string("token", token_))
		return stream.get_error();
	if (!stream.read_string("host", location_))
		return stream.get_error();
	full_name_ = user_name_ + '@' + location_;
	return true;
}

rx_result aspnet_security_context::initialize (const string_view_type name, const string_view_type cookie, bool system)
{
	user_name_ = name;
	token_ = user_name_ + '\\' + string_type(cookie);
	return true;
}

bool aspnet_security_context::has_console () const
{
	return false;
}

bool aspnet_security_context::is_in_role (string_view_type role, security_mask_t access) const
{

#ifdef _WIN32
	return g_sec_proxy->is_in_role(string_type(role), token_);
#else
    return false;
#endif
}


} // namespace aspnet_forms
} // namespace rx_security
} // namespace rx_internal

