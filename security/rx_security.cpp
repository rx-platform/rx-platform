

/****************************************************************************
*
*  security\rx_security.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


// rx_security
#include "security/rx_security.h"

namespace rx_platform
{
rx_security_handle_t rx_security_context();
bool rx_push_security_context(rx_security_handle_t obj);
bool rx_pop_security_context();
}
#include "sys_internal/rx_security/rx_platform_security.h"


namespace rx_platform {

namespace security {
namespace {
security_manager* g_object = nullptr;
security_context_ptr dummy_ctx;
}

// Class rx_platform::security::security_context 

security_context::security_context()
      : handle_(0)
{
}


security_context::~security_context()
{
	if (handle_)
	{
		char buff[0x100];
		snprintf(buff, sizeof(buff), "User %s, security context deleted without deactivation.", get_full_name().c_str());
		SECURITY_LOG_CRITICAL("manager", 900, buff);
	}
}



void security_context::logout ()
{
	if (handle_)
	{
		handle_ = security_manager::instance().context_deactivated(smart_this());
	}
}

rx_result security_context::login ()
{
	if(is_authenticated())
		handle_ = security_manager::instance().context_activated(smart_this());
	return true;
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


// Class rx_platform::security::security_manager 

security_manager::security_manager()
{
}

security_manager::security_manager(const security_manager &right)
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
	if (g_object == nullptr)
	{
		g_object = new security_manager();

		dummy_ctx = rx_create_reference<unathorized_security_context>();
	}
	return *g_object;
}

rx_security_handle_t security_manager::context_activated (security_context::smart_ptr who)
{
	char buff[0x100];

	if (who->get_handle())
	{
		snprintf(buff, sizeof(buff), "User %s already logged in.", who->get_full_name().c_str());
		SECURITY_LOG_INFO("manager", 900, buff);
		return 0;
	}

	snprintf(buff, sizeof(buff), "User %s logged in.", who->get_full_name().c_str());
	SECURITY_LOG_INFO("manager", 900, buff);

	locks::auto_lock_t dummy(&active_lock_);
	
	intptr_t new_id;
	do
	{
		if (RX_OK == rx_crypt_gen_random(&new_id, sizeof(new_id)))
		{
			if(new_id!=0 && active_handles_.find(new_id)==active_handles_.end())
				break;
		}
	} while(true);
	active_contexts_.emplace(new_id, who);
	return new_id;
}

rx_security_handle_t security_manager::context_deactivated (security_context::smart_ptr who)
{
	char buff[0x100];
	locks::auto_lock_t dummy(&active_lock_);
	auto it = active_contexts_.find(who->get_handle());
	if (it != active_contexts_.end())
	{
		active_contexts_.erase(it);
		snprintf(buff, sizeof(buff), "User %s logged off.", who->get_full_name().c_str());
		SECURITY_LOG_INFO("manager", 900, buff);
		return 0;
	}
	else
	{
		SECURITY_LOG_DEBUG("manager", 900, "Shouldn't be here!!!");
		return who->get_handle();
	}
}

void security_manager::get_active_contexts (std::vector<security_context_ptr >& ctxs)
{
	locks::auto_lock_t dummy(&active_lock_);
	ctxs.reserve(active_contexts_.size());
	for (auto one : active_contexts_)
	{
		ctxs.push_back(one.second);
	}
}

security_context_ptr security_manager::get_context (rx_security_handle_t handle)
{
	if(handle==0)
		return security_context_ptr::null_ptr;

	locks::auto_lock_t dummy(&active_lock_);
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

void security_manager::deinitialize ()
{
	dummy_ctx = security_context_ptr::null_ptr;
	delete this;
}

// whole purpose of this function is to enable lazy initialization of dummy_ctx
security_context_ptr security_context_helper(bool get_unathorized)
{
	if (get_unathorized)
		return dummy_ctx;

	rx_security_handle_t handle = rx_security_context();
	if (!handle)
	{// nothing active
		return dummy_ctx;
	}
	else
	{// try to get from handle
		security_context_ptr ret = security_manager::instance().get_context(handle);
		if (!ret)
		{// context not active any more
			return dummy_ctx;
		}
		return ret;
	}
}
security_context_ptr unauthorized_context()
{
	return security_context_helper(true);
}
security_context_ptr active_security()
{
	return security_context_helper(false);
}
void push_security(security_context_ptr ctx)
{
	rx_push_security_context(ctx->get_handle());
}
void pop_security()
{
	rx_pop_security_context();
}
// Class rx_platform::security::security_guard 

security_guard::security_guard (const meta_data& data, security_mask_t access)
	: access_mask_((security_mask_t)(data.attributes&namespace_item_full_access))
	, extended_mask_(rx_security_ext_null)
	, path_base_(data.get_full_path())
{
	/*if (data.attributes | namespace_item_internal_access)
		extended_mask_ = extended_mask_ | rx_security_requires_internal;
	if (data.attributes | namespace_item_system_access)
		extended_mask_ = extended_mask_ | rx_security_requires_system;*/
	if (access)
		access_mask_ = access_mask_ | access;
}

security_guard::security_guard (security_mask_t access, const string_type& path, extended_security_mask_t extended)
	: access_mask_(access)
	, extended_mask_(extended)
	, path_base_(path)
{
}


security_guard::~security_guard()
{
}



bool security_guard::check_permission (security_mask_t access)
{
	bool ret = false;
	security_context_ptr ctx = active_security();
	if (ctx)
	{
		ret = check_permission(access, ctx);
	}
	if (!ret)
	{
		SECURITY_LOG_TRACE("Guard",800,"Access denied!!");
	}
	return ret;
}

bool security_guard::check_permission (security_mask_t mask, security_context_ptr ctx)
{
	if (!ctx->is_authenticated())
	{
		return false;
	}
	else
	{
		if ((mask & access_mask_) == rx_security_null)
		{
			return false;
		}
		if ((extended_mask_ & rx_security_requires_system) && !ctx->is_system())
		{
			return false;
		}
		else if ((extended_mask_ & rx_security_requires_internal) && !ctx->is_hosted())
		{
			return false;
		}

		return rx_internal::rx_security::platform_security::instance().check_permissions(
			mask, path_base_, ctx);
	}
}


// Class rx_platform::security::secured_scope 

secured_scope::secured_scope (security_context_ptr ctx)
	: ctx_(ctx ? ctx->get_handle() : 0)
{
	if (ctx_)
		rx_push_security_context(ctx_);
}

secured_scope::secured_scope (rx_security_handle_t ctx)
    : ctx_(ctx)
{
	if (ctx_)
		rx_push_security_context(ctx_);
}


secured_scope::~secured_scope()
{
	if (ctx_)
		rx_pop_security_context();
}



// Class rx_platform::security::unathorized_security_context 

unathorized_security_context::unathorized_security_context (bool has_console)
      : has_console_(has_console)
{
	user_name_ = "unauthorized";
	full_name_ = user_name_ + "@";
	location_ = rx_get_node_name();
	full_name_ += location_;
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

rx_result unathorized_security_context::serialize (base_meta_writer& stream) const
{
	return RX_NOT_IMPLEMENTED;
}

rx_result unathorized_security_context::deserialize (base_meta_reader& stream)
{
	return RX_NOT_IMPLEMENTED;
}


bool unathorized_security_context::has_console () const
{
  return has_console_;
}


} // namespace security
} // namespace rx_platform

