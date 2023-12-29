

/****************************************************************************
*
*  sys_internal\rx_security\rx_platform_roles.cpp
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


// rx_platform_roles
#include "sys_internal/rx_security/rx_platform_roles.h"



namespace rx_internal {

namespace rx_security {

// Class rx_internal::rx_security::platform_role 

platform_role::platform_role()
{
}



bool platform_role::is_in_role (security::security_context_ptr& ctx)
{
	for (const auto& one : users_)
	{
		if (rx_match_pattern(ctx->get_full_name().c_str(), one.first.c_str(), true))
			return true;
	}
	return false;
}

std::optional<bool> platform_role::check_permissions (security::security_mask_t mask, const string_type& obj_path, security::security_context_ptr ctx)
{
	bool got_true = false;
	for (auto& one : permissions_)
	{
		if (rx_match_pattern(obj_path.c_str(), one.path.c_str(), true))
		{
			if (mask & one.deny_access)
				return false;// access denied
			else if (mask & one.allow_access)
				got_true = true;// access denied
		}
	}
	if (got_true)
		return true;
	else
		return std::optional<bool>();
}

rx_result platform_role::serialize_role (base_meta_writer& stream)
{
	return RX_NOT_IMPLEMENTED;
}

rx_result platform_role::deserialize_role (base_meta_reader& stream)
{
	if (!stream.start_object("role"))
		return stream.get_error();

	if (!stream.start_array("permissions"))
		return stream.get_error();

	while (!stream.array_end())
	{
		if (!stream.start_object("perm"))
			return stream.get_error();

		role_permission perm;

		if (!stream.read_string("path", perm.path))
			return stream.get_error();
		uint32_t temp;
		if (!stream.read_uint("allow", temp))
			return stream.get_error();
		perm.allow_access = ((security::security_mask_t)temp);
		if (!stream.read_uint("deny", temp))
			return stream.get_error();
		perm.deny_access = ((security::security_mask_t)temp);

		permissions_.push_back(std::move(perm));

		if (!stream.end_object())
			return stream.get_error();
	}

	if (!stream.start_array("users"))
		return stream.get_error();

	while (!stream.array_end())
	{
		if (!stream.start_object("user"))
			return stream.get_error();

		string_type name;
		if (!stream.read_string("name", name))
			return stream.get_error();

		//platform_principal_ref princ;

		//if (!stream.start_object("princ"))
		//	return stream.get_error();

		//if (!stream.read_string("name", princ.name))
		//	return stream.get_error();

		//if (!stream.read_bytes("opaque", princ.opaque))
		//	return stream.get_error();

		//if (!stream.end_object())
		//	return stream.get_error();


		users_.emplace(std::move(name), platform_principal_ref());

		if (!stream.end_object())
			return stream.get_error();
	}

	if (!stream.end_object())
		return stream.get_error();

	return true;
}


// Class rx_internal::rx_security::platform_roles 


rx_result platform_roles::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	platform_role::role_permission perm;
	
	platform_role role_users;

	role_users.users_.emplace("*@*", platform_principal_ref());

	perm.path = "/world*";
	perm.allow_access = security::rx_security_full;
	perm.deny_access = security::rx_security_null;
	role_users.permissions_.push_back(std::move(perm));

	perm.path = "/sys/*";
	perm.allow_access =
		security::rx_security_read_access
		| security::rx_security_poll_access
		| security::rx_security_execute_access;
	perm.deny_access = security::rx_security_null;
	role_users.permissions_.push_back(std::move(perm));

	perm.path = "@*";
	perm.allow_access =
		security::rx_security_read_access
		| security::rx_security_poll_access
		| security::rx_security_execute_access;
	perm.deny_access = security::rx_security_null;
	role_users.permissions_.push_back(std::move(perm));

	roles_.push_back(std::move(role_users));

	string_type node_name = rx_gate::instance().get_node_name();
	
	platform_role role_system;

	role_system.users_.emplace("host@"s + node_name, platform_principal_ref());
	role_system.users_.emplace("system@"s + node_name, platform_principal_ref());

	perm.path = "/*";
	perm.allow_access =	security::rx_security_full;
	perm.deny_access = security::rx_security_null;
	role_system.permissions_.push_back(std::move(perm));

	perm.path = "@*";
	perm.allow_access = security::rx_security_full;
	perm.deny_access = security::rx_security_null;
	role_system.permissions_.push_back(std::move(perm));

	roles_.push_back(std::move(role_system));


	return true;
}

void platform_roles::deinitialize ()
{
}

rx_result platform_roles::initialize_roles (std::vector<rx_roles_storage_item_ptr> storages)
{
	rx_result result;
	for (auto&& one : storages)
	{
		result = one->open_for_read();
		if (result)
		{
			auto& stream = one->read_stream();

			if (!stream.start_array("roles"))
			{
				one->close_read();
				return stream.get_error();
			}

			while (!stream.array_end())
			{
				platform_role role;
				result = role.deserialize_role(stream);
				if (result)
				{
					invalidate_cache();
					roles_.push_back(std::move(role));
				}
			}
			one->close_read();
		}
		if (!result)
			break;
	}
	return result;
}

bool platform_roles::check_permissions (security::security_mask_t mask, const string_type& path, security::security_context_ptr ctx)
{
	std::scoped_lock _(roles_lock_);
	auto& roles = get_roles_for_user(ctx);
	if (roles.empty())
		return false;// no roles
	
	bool ret = false;

	for (auto role_ptr : roles)
	{
		auto chk_result = role_ptr->check_permissions(mask, path, ctx);
		if (chk_result.has_value())
		{
			if (!chk_result.value())
				return false;
			else
				ret = true;
		}
	}

	return ret;	
}

std::vector<platform_role*>& platform_roles::get_roles_for_user (security::security_context_ptr& ctx)
{
	string_type user_name = ctx->get_full_name();
	auto it = user_roles_cache_.find(user_name);
	if (it == user_roles_cache_.end())
	{
		std::vector<platform_role*> temp;
		for (auto& one : roles_)
		{
			if (one.is_in_role(ctx))
			{
				temp.push_back(&one);
			}
		}
		auto ret = user_roles_cache_.emplace(user_name, std::move(temp));
		it = ret.first;
	}
	return it->second;
}

void platform_roles::invalidate_cache ()
{
	user_roles_cache_.clear();
}


// Class rx_internal::rx_security::platform_principal_ref 


} // namespace rx_security
} // namespace rx_internal

