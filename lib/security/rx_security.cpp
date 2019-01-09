

/****************************************************************************
*
*  lib\security\rx_security.cpp
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_security
#include "lib/security/rx_security.h"

namespace rx
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}


namespace rx {

namespace security {

// Class rx::security::security_context 

security_context::security_context()
      : handle_(0)
{
}


security_context::~security_context()
{
}



void security_context::logout ()
{
	handle_ = security_manager::instance().context_deactivated(smart_this());
}

void security_context::login ()
{
	handle_ = security_manager::instance().context_activated(smart_this());
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


// Class rx::security::security_manager 

security_manager::security_manager()
      : last_id_(0)
{
}

security_manager::security_manager(const security_manager &right)
      : last_id_(0)
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
	locks::auto_lock dummy(&active_lock_);
	last_id_++;
	if (last_id_ == 0)
		last_id_++;
	active_contexts_.emplace(last_id_, who);
	return last_id_;
}

rx_security_handle_t security_manager::context_deactivated (security_context::smart_ptr who)
{
	char buff[0x100];
	snprintf(buff, sizeof(buff), "User %s, security context destroying...", who->get_full_name().c_str());
	SECURITY_LOG_INFO("manager", 900, buff);
	locks::auto_lock dummy(&active_lock_);
	auto it = active_contexts_.find(who->get_handle());
	if (it != active_contexts_.end())
	{
		active_contexts_.erase(it);
		snprintf(buff, sizeof(buff), "User %s, security context destroyed.", who->get_full_name().c_str());
		SECURITY_LOG_INFO("manager", 900, buff);
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
	locks::auto_lock dummy(&active_lock_);
	ctxs.reserve(active_contexts_.size());
	for (auto one : active_contexts_)
	{
		ctxs.emplace_back(one.second);
	}
}

security_context_ptr security_manager::get_context (rx_security_handle_t handle)
{
	if(handle==0)
		return security_context_ptr::null_ptr;

	locks::auto_lock dummy(&active_lock_);
	auto it = active_contexts_.find(handle);
	if (it != active_contexts_.end())
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
	static unathorized_security_context::smart_ptr dummy_ctx(pointers::_create_new);
	rx_security_handle_t handle = rx_security_context();
	if (!handle)
	{// nothing active
		return dummy_ctx;
	}
	else
	{// try to ge from handle
		security_context_ptr ret = security_manager::instance().get_context(handle);
		if (!ret)
		{// context not active any more
			return dummy_ctx;
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
// Class rx::security::security_guard 

security_guard::security_guard()
{
}


security_guard::~security_guard()
{
}



bool security_guard::check_premissions (security_mask_t mask, extended_security_mask_t extended_mask)
{
	bool ret = false;
	security_context_ptr ctx = active_security();
	if (ctx)
	{
		ret = check_premissions(mask, extended_mask, ctx);
	}
	if (!ret)
	{
		SECURITY_LOG_TRACE("Guard",800,"Access denied!!");
	}
	return ret;
}

bool security_guard::check_premissions (security_mask_t mask, extended_security_mask_t extended_mask, security_context_ptr ctx)
{
	if (!ctx->is_system())
	{
		return false;
	}
	return true;
}


// Class rx::security::security_auto_context 

security_auto_context::security_auto_context (security_context_ptr ctx)
	: ctx_(ctx->get_handle())
{
	if (ctx_)
		rx_push_security_context(ctx_);
}


security_auto_context::~security_auto_context()
{
	if (ctx_)
		rx_pop_security_context();
}



// Class rx::security::built_in_security_context 

built_in_security_context::built_in_security_context()
{
	location_ = rx_get_server_name();
}


built_in_security_context::~built_in_security_context()
{
}



// Class rx::security::unathorized_security_context 

unathorized_security_context::unathorized_security_context()
{
	user_name_ = "unathorized";
	full_name_ = user_name_ + "@";
	location_ = rx_get_server_name();
	full_name_ += location_;
	port_ = "host";
}


unathorized_security_context::~unathorized_security_context()
{
}



bool unathorized_security_context::is_authenticated () const
{
  return false;

}

void unathorized_security_context::interface_bind ()
{
}

void unathorized_security_context::interface_release ()
{
}


// Class rx::security::loose_security_guard 

loose_security_guard::loose_security_guard()
{
}


loose_security_guard::~loose_security_guard()
{
}



bool loose_security_guard::check_premissions (security_mask_t mask, extended_security_mask_t extended_mask, security_context_ptr ctx)
{
	if (!ctx->is_authenticated())
	{
		return false;
	}
	return true;
}


} // namespace security
} // namespace rx

