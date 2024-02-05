

/****************************************************************************
*
*  system\security\rx_security.cpp
*
*  Copyright (c) 2017 Dusan Ciric
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


#include "stdafx.h"


// rx_security
#include "system/security/rx_security.h"

namespace rx
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}


namespace server {

namespace security {

// Class server::security::security_context 

security_context::security_context()
      : _handle(0)
{
}


security_context::~security_context()
{
}



void security_context::logout ()
{
	_handle = security_manager::instance().context_deactivated(smart_this());
}

void security_context::login ()
{
	_handle = security_manager::instance().context_activated(smart_this());
}

bool security_context::has_console () const
{
  return false;

}

bool security_context::is_system () const
{
  return false;

}

bool security_context::is_authenticated () const
{
  return true;

}

bool security_context::is_hosted () const
{
  return false;

}

bool security_context::is_interactive () const
{
  return false;

}


// Class server::security::security_manager 

security_manager::security_manager()
      : _last_id(0)
{
}

security_manager::security_manager(const security_manager &right)
      : _last_id(0)
{
	RX_ASSERT(false);
}


security_manager::~security_manager()
{
}


security_manager & security_manager::operator=(const security_manager &right)
{
	RX_ASSERT(false);
	return *this;
}



security_manager& security_manager::instance ()
{
	static security_manager g_object;
	return g_object;
}

rx_security_handle_t security_manager::context_activated (security_context::smart_ptr who)
{
	char buff[0x100];
	snprintf(buff, sizeof(buff), "User %s, security context created.", who->get_full_name().c_str());
	SECURITY_LOG_INFO("manager", 900, buff);
	locks::auto_lock dummy(&_active_lock);
	_last_id++;
	if (_last_id == 0)
		_last_id++;
	_active_contexts.emplace(_last_id, who);
	return _last_id;
}

rx_security_handle_t security_manager::context_deactivated (security_context::smart_ptr who)
{
	char buff[0x100];
	snprintf(buff, sizeof(buff), "User %s, security context destroying.", who->get_full_name().c_str());
	SECURITY_LOG_INFO("manager", 900, buff);
	locks::auto_lock dummy(&_active_lock);
	auto it = _active_contexts.find(who->get_handle());
	if (it != _active_contexts.end())
	{
		_active_contexts.erase(it);
		return 0;
	}
	else
	{
		SECURITY_LOG_DEBUG("manager", 900, "Shouldnt be here!!!");
		return who->get_handle();
	}
}

void security_manager::get_active_contexts (std::vector<security_context_ptr >& ctxs)
{
	locks::auto_lock dummy(&_active_lock);
	ctxs.reserve(_active_contexts.size());
	for (auto one : _active_contexts)
	{
		ctxs.emplace_back(one.second);
	}
}

security_context_ptr security_manager::get_context (rx_security_handle_t handle)
{
	if(handle==0)
		return security_context_ptr::null_ptr;

	locks::auto_lock dummy(&_active_lock);
	auto it = _active_contexts.find(handle);
	if (it != _active_contexts.end())
	{
		return it->second;
	}
	else
	{
		return security_context_ptr::null_ptr;
	}
}


security_context_ptr active_security()
{
	static unathorized_security_context dummy_ctx;
	rx_security_handle_t handle = rx_security_context();
	if (!handle)
	{// nothing active
		return pointers::interface_reference<unathorized_security_context>::create_from_pointer(&dummy_ctx);
	}
	else
	{// try to ge from handle
		security_context_ptr ret = security_manager::instance().get_context(handle);
		if (!ret)
		{// context not active any more
			ret = pointers::interface_reference<unathorized_security_context>::create_from_pointer(&dummy_ctx);
		}
		return ret;
	}
}
void push_security(security_context_ptr ctx)
{
	rx_push_security_context(ctx->get_handle());
}
void pop_security()
{
	rx_pop_security_context();
}
// Class server::security::secured_object 

secured_object::secured_object()
{
}


secured_object::~secured_object()
{
}



bool secured_object::dword_check_premissions (dword mask, dword extended_mask)
{
	bool ret = false;
	security_context_ptr ctx = basic_check(ret);
	if(!ctx)
		return ret;
	return true;
}

security_context_ptr secured_object::basic_check (bool& ret)
{
	security_context_ptr ctx = active_security();
	if (!ctx)
	{
		ret = false;
		return ctx;
	}
	if (!ctx->is_authenticated())
	{
		ret = false;
		return security_context_ptr::null_ptr;
	}
	if (ctx->is_system())
	{
		ret = true;
		return security_context_ptr::null_ptr;
	}
	return ctx;
}


// Class server::security::security_auto_context 

security_auto_context::security_auto_context (security_context_ptr ctx)
	: _ctx(ctx->get_handle())
{
	if (_ctx)
		rx_push_security_context(_ctx);
}


security_auto_context::~security_auto_context()
{
	if (_ctx)
		rx_pop_security_context();
}



// Class server::security::built_in_security_context 

built_in_security_context::built_in_security_context()
{
	_location = rx_get_server_name();
}


built_in_security_context::~built_in_security_context()
{
}



// Class server::security::unathorized_security_context 

unathorized_security_context::unathorized_security_context()
{
	_user_name = "unathorized";
	_full_name = _user_name + "@";
	_location = rx_get_server_name();
	_full_name += _location;
	_port = "host";
}


unathorized_security_context::~unathorized_security_context()
{
}



void unathorized_security_context::interface_bind ()
{
}

void unathorized_security_context::interface_release ()
{
}

bool unathorized_security_context::is_authenticated () const
{
  return false;

}


} // namespace security
} // namespace server

