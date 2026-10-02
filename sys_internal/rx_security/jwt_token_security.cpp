

/****************************************************************************
*
*  sys_internal\rx_security\jwt_token_security.cpp
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


// jwt_token_security
#include "sys_internal/rx_security/jwt_token_security.h"

#include "sys_internal/rx_inf.h"



#define RX_JWT_SECURITY_INFO "{rx-platform} JWT Token Security Provider"

#include "common/base64.h"
#include "lib/rx_ser_json.h"


namespace rx_internal {

namespace rx_security {

namespace jwt_token {
namespace
{
const string_type g_jwt_security_name = RX_JWT_SECURITY_NAME;

struct jwt_header
{
    string_type alg;
    string_type kid;
    string_type typ;


    static rx_result deserialize(base_meta_reader& stream, jwt_header& header)
    {
        if (!stream.read_string("alg", header.alg))
            return stream.get_error();
        if (!stream.read_string("kid", header.kid))
            return stream.get_error();
        if (!stream.read_string("typ", header.typ))
            return stream.get_error();
        return true;
    }
};
struct jwt_permission
{
    string_type type;
    int action;
    int control_type;


    static rx_result deserialize(base_meta_reader& stream, jwt_permission& header)
    {
        if (!stream.read_string("type", header.type))
            return stream.get_error();
        if (!stream.read_int("action", header.action))
            return stream.get_error();
        if (!stream.read_int("controlType", header.control_type))
            return stream.get_error();
        return true;
    }
};
struct jwt_payload
{
    string_type name;
    bool is_system_user;
    string_type iss;
    uint64_t iat;
    uint64_t nbf;
    uint64_t exp;
    std::vector<jwt_permission> permissions;


    static rx_result deserialize(base_meta_reader& stream, jwt_payload& header)
    {

        if (!stream.read_string("name", header.name))
            return stream.get_error();

        if (!stream.read_bool("is_system_user", header.is_system_user))
            return stream.get_error();

        if (!stream.read_string("iss", header.iss))
            return stream.get_error();

        if (!stream.read_uint64("iat", header.iat))
            return stream.get_error();

        if (!stream.read_uint64("nbf", header.nbf))
            return stream.get_error();

        if (!stream.read_uint64("exp", header.exp))
            return stream.get_error();

        if (!header.is_system_user)
        {
            if (!stream.start_array("permissions"))
                return stream.get_error();

            while (!stream.array_end())
            {

                if (!stream.start_object("permission"))
                    return stream.get_error();
                jwt_permission perm;
                auto result = perm.deserialize(stream, perm);
                if (!result)
                    return result;


                header.permissions.push_back(perm);

                if (!stream.end_object())
                    return stream.get_error();


            }
        }

        return true;
    }
};

void base64_url2base64(string_type& base64url)
{
    // 1. Replace URL-safe characters
    for (char& c : base64url) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }

    // 2. Restore padding '=' characters
    size_t mod = base64url.length() % 4;
    if (mod == 2) {
        base64url.append("==");
    }
    else if (mod == 3) {
        base64url.append("=");
    }
}
security_mask_t jwt_action_to_security_mask(int action)
{
    security_mask_t ret = rx_security_null;
    if (action == 0)
    {
        ret = rx_security_read_access | rx_security_write_access | rx_security_delete_access | rx_security_poll_access | rx_security_execute_access;
    }
    else
    {
        if (action & 1)
            ret = ret | (rx_security_read_access | rx_security_poll_access);
        if (action & 2)
            ret = ret | rx_security_delete_access;
        if (action & 4)
            ret = ret | rx_security_write_access;
        if (action & 16)
            ret = ret | rx_security_execute_access;
    }
    return ret;
}
}

// Class rx_internal::rx_security::jwt_token::jwt_token_security_provider 


const string_type& jwt_token_security_provider::get_name ()
{
    return g_jwt_security_name;
}

string_type jwt_token_security_provider::get_info ()
{
    return RX_JWT_SECURITY_INFO;
}

rx_result jwt_token_security_provider::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
	JWT_LOG_INFO("jwt_token_security_provider", 900, "Initializing JWT Token Security Provider");
	auto result = jwt_tokens_repository::instance().initialize(host, data);
    if (result)
    {
        JWT_LOG_INFO("jwt_token_security_provider", 900, "JWT Token Security Provider initialized");
    }
    else
    {
		JWT_LOG_ERROR("jwt_token_security_provider", 900, "JWT Token Security Provider initialization failed: " + result.errors_line());
    }
    return result;
}

void jwt_token_security_provider::deinitialize ()
{
    JWT_LOG_INFO("jwt_token_security_provider", 900, "De-initializing JWT Token Security Provider");

    jwt_tokens_repository::instance().deinitialize();
    
    JWT_LOG_INFO("jwt_token_security_provider", 900, "JWT Token Security Provider de-initialized");
}

rx_result_with<security::security_context_ptr> jwt_token_security_provider::create_host_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    return RX_NOT_SUPPORTED;
}

rx_result_with<security::security_context_ptr> jwt_token_security_provider::create_system_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    return RX_NOT_SUPPORTED;
}

rx_result_with<security::security_context_ptr> jwt_token_security_provider::create_world_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    return RX_NOT_SUPPORTED;
}

rx_result jwt_token_security_provider::start (hosting::rx_platform_host* host)
{
    JWT_LOG_INFO("jwt_token_security_provider", 900, "Starting JWT Token Security Provider");
    auto result = jwt_tokens_repository::instance().start(host);
    if (result)
    {
        JWT_LOG_INFO("jwt_token_security_provider", 900, "JWT Token Security Provider started");
    }
    else
    {
        JWT_LOG_ERROR("jwt_token_security_provider", 900, "JWT Token Security Provider start failed: " + result.errors_line());
    }
    return result;
}

void jwt_token_security_provider::stop ()
{
    JWT_LOG_INFO("jwt_token_security_provider", 900, "Stopping JWT Token Security Provider");

    jwt_tokens_repository::instance().stop();

    JWT_LOG_INFO("jwt_token_security_provider", 900, "JWT Token Security Provider stopped");
}


// Class rx_internal::rx_security::jwt_token::jwt_token_security_context 

jwt_token_security_context::jwt_token_security_context (std::weak_ptr<jwt_token_data> data)
      : my_data_(std::move(data))
{
	auto my_data = my_data_.lock();
    if(my_data)
    {
        location_ = my_data->get_provider();
        user_name_ = my_data->get_user_name();
		full_name_ = user_name_ + "@" + location_;
    }
}


jwt_token_security_context::~jwt_token_security_context()
{
}



bool jwt_token_security_context::is_authenticated () const
{
	auto my_data = my_data_.lock();
    if(!my_data)
        return false;
    else
        return my_data->is_valid();
}

rx_result jwt_token_security_context::serialize (base_meta_writer& stream) const
{
    return RX_NOT_SUPPORTED;
}

rx_result jwt_token_security_context::deserialize (base_meta_reader& stream)
{
    return RX_NOT_SUPPORTED;
}

bool jwt_token_security_context::has_console () const
{
    return false;
}

bool jwt_token_security_context::is_in_role (string_view_type role, security_mask_t access) const
{

    auto my_data = my_data_.lock();
    if (!my_data)
        return false;
    else
        return my_data->is_in_role(role, access);
}


// Class rx_internal::rx_security::jwt_token::jwt_token_data 

jwt_token_data::jwt_token_data()
      : is_system_(false),
        valid_(false)
{
}


jwt_token_data::~jwt_token_data()
{
}



rx_result jwt_token_data::parse (string_type header, string_type payload, string_type signature)
{

    base64_url2base64(header);
    base64_url2base64(payload);
    base64_url2base64(signature);


    bytes_value_struct header_decoded;
    if (RX_OK == rx_base64_get_data(&header_decoded, header.c_str()))
    {


        bytes_value_struct payload_decoded;
        if (RX_OK == rx_base64_get_data(&payload_decoded, payload.c_str()))
        {
            bytes_value_struct signature_decoded;
            if (RX_OK == rx_base64_get_data(&signature_decoded, signature.c_str()))
            {

                rx::serialization::json_reader reader_header;
                size_t temp_size = 0;
                const char* temp_data = (const char*)rx_c_ptr(&header_decoded, &temp_size);
                auto bresult = reader_header.parse_data(string_type(temp_data, temp_size));
                if (bresult)
                {

                    jwt_header jwt_header;
                    auto result = jwt_header.deserialize(reader_header, jwt_header);
                    if (result)
                    {

                        if (jwt_header.typ == "JWT")
                        {
                            rx::serialization::json_reader reader_payload;
                            temp_data = (const char*)rx_c_ptr(&payload_decoded, &temp_size);
                            bresult = reader_payload.parse_data(string_type(temp_data, temp_size));
                            if (bresult)
                            {
                                jwt_payload jwt_payload;
                                result = jwt_payload.deserialize(reader_payload, jwt_payload);
                                if (result)
                                {
                                    for (const auto& perm : jwt_payload.permissions)
                                    {
                                        permission_data data;
                                        if (perm.control_type == 1)
                                        {
                                            data.allowed = jwt_action_to_security_mask(perm.action);
                                            data.denied = rx_security_null;
                                        }
                                        else
                                        {
                                            data.allowed = rx_security_null;
                                            data.denied = jwt_action_to_security_mask(perm.action);
                                        }
                                        permissions_[perm.type] = data;
                                    }
                                    timeval tval;
                                    tval.tv_sec = jwt_payload.exp;
                                    tval.tv_usec = 0;
                                    expires_ = rx_time(tval);
                                    is_system_ = jwt_payload.is_system_user;
                                    user_name_ = jwt_payload.name;
                                    string_type host;
                                    auto idx = jwt_payload.iss.find("://");
                                    if (idx != string_type::npos)
                                    {
                                        auto idx2 = jwt_payload.iss.find('/', idx + 3);
                                        if (idx2 != string_type::npos)
                                        {
                                            host = jwt_payload.iss.substr(idx + 3, idx2 - idx - 3);
                                        }
                                        else
                                        {
                                            host = jwt_payload.iss.substr(idx + 3);
                                        }
                                    }
                                    else
                                    {
                                        host = jwt_payload.iss;
                                    }
                                    user_name_ = jwt_payload.name;
                                    provider_ = host;
                                    valid_ = true;
									size_t temp_size = 0;
									std::byte* sig_ptr = (std::byte*)rx_c_ptr(&signature_decoded, &temp_size);
									if (sig_ptr && temp_size > 0)
									    signature_ = byte_string(sig_ptr, sig_ptr + temp_size);
                                    else
										signature_.clear();
									key_id_ = jwt_header.kid;
                }
                            }
                        }
                    }
                }
                else
                {
                    JWT_LOG_ERROR("jwt_token_data", 200, "Unable to parse JSON header"s + reader_header.get_error());
                }
                rx_destory_bytes_value_struct(&signature_decoded);
            }
            rx_destory_bytes_value_struct(&payload_decoded);
        }
        rx_destory_bytes_value_struct(&header_decoded);
    }
    return true;
}

bool jwt_token_data::is_in_role (string_view_type role, security_mask_t access) const
{

	std::scoped_lock _(const_cast<jwt_token_data*>(this)->lock_);
    if (!valid_ || expires_ < rx_time::now())
    {
        return false;
    }
    if (is_system_)
    {
        return true;
    }

    auto it = permissions_.find(string_type(role));
    if (it != permissions_.end())
    {
        if (it->second.denied & access)// explicit denial of access
            return false;

		bool ret = (it->second.allowed & access) == access;

        return ret;// check for all requested access
    }
    return false;
}

bool jwt_token_data::is_valid () const
{
    std::scoped_lock _(const_cast<jwt_token_data*>(this)->lock_);
    return valid_ && expires_ >= rx_time::now();
}


// Class rx_internal::rx_security::jwt_token::jwt_tokens_repository 

jwt_tokens_repository* jwt_tokens_repository::instance_ = NULL;

jwt_tokens_repository::jwt_tokens_repository()
{
}


jwt_tokens_repository::~jwt_tokens_repository()
{
}



jwt_tokens_repository& jwt_tokens_repository::instance ()
{
    if (!instance_)
    {
        instance_ = new jwt_tokens_repository();
    }
	return *instance_;
}

rx_result jwt_tokens_repository::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hard coded public key for JWT Token Security Provider
// should be replaced with loading public keys from configuration or from a secure location
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* temp_key =
"-----BEGIN RSA PUBLIC KEY-----\r\n"
"MIIBCgKCAQEAptwv8+H4WMXaIgFJfFJDjNZJr6BR/GsgvXFD3E+xTyOQC0ieO82z\r\n"
"q4RRNqXEhZww/6t7Xu9sEPBdrvVtCj8t6LAfv//EJmbYBkl9zn4YFUaMQKX8XFpr\r\n"
"zJZatDZU8WdOyeMJeRS1zYNtQp8QIZjyTN4oND+Mcy6ZXvbOR4Cf7/ZqKgBq/A7m\r\n"
"rEO6qwTVFhZqki09ZmI4e2/wUhFlOF9B8qsK+ME4nb56EAiINt4dcvMnMuNxXsf1\r\n"
"AA87XTqMBb+QvBfU3X0j5AJ9LfpxkkP90evb0GK1BvMhiddrzU8m/4NbfOJwT8Tu\r\n"
"N0IHDvbVVD8D9v6nAL4fomwLTC7fnzBERwIDAQAB\r\n"
"-----END RSA PUBLIC KEY-----\r\n";
	auto key = rx_crypt_open_public_from_pem(temp_key);
    if(key == 0)
    {
        JWT_LOG_ERROR("jwt_tokens_repository", 900, "Failed to load public key for JWT Token Security Provider");
        return "Error loading public key";
	}
	public_keys_["ensaco-rsa-key-v1"] = key;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////


    return true;
}

void jwt_tokens_repository::deinitialize ()
{
    instance_ = nullptr;
    delete this;
}

rx_result jwt_tokens_repository::start (hosting::rx_platform_host* host)
{
    std::function<void()> func = [this]()
    {

        std::scoped_lock _(tokens_lock_);
        for (auto it = active_tokens_.begin(); it != active_tokens_.end(); )
        {
            if (!it->second->is_valid())
            {
                JWT_LOG_TRACE("jwt_tokens_repository", 900, "Removing expired JWT Token for user "s + it->second->get_user_name() + " from repository");
                it = active_tokens_.erase(it);
            }
            else
            {
                ++it;
            }
        }
	};
	timer_ = rx_create_timer_job<std::function<void()>>()(rx_reference_ptr(), func);
	infrastructure::server_runtime::instance().append_calculation_job(timer_
        , infrastructure::server_runtime::instance().get_executer(RX_DOMAIN_SLOW));
    timer_->start(60000);// 1 minute
    return true;
}

void jwt_tokens_repository::stop ()
{
    if (timer_)
    {
        timer_->cancel();
        timer_ = periodic_job::smart_ptr::null_ptr;
    }
}

rx_result_with<jwt_token_security_context::smart_ptr> jwt_tokens_repository::get_context (string_view_type token)
{
    auto idx = token.find('.');


    if (idx != string_type::npos)
    {
        string_type header(token.substr(0, idx));
        auto idx2 = token.find('.', idx + 1);
        if (idx2 != string_type::npos)
        {
            string_type payload(token.substr(idx + 1, idx2 - idx - 1));
            string_type signature(token.substr(idx2 + 1));
            {
                std::scoped_lock _(tokens_lock_);
                auto it = active_tokens_.find(signature);
                if (it != active_tokens_.end())
                {
                    if (it->second->is_valid())
                    {
                        jwt_token_security_context::smart_ptr context = rx_create_reference<jwt_token_security_context>(it->second);
                        return context;
                    }
                    else
                    {
                        JWT_LOG_TRACE("jwt_tokens_repository", 900, "Removing expired JWT Token for user "s +it->second->get_user_name() + " from repository");
                        active_tokens_.erase(it);
                        return RX_INVALID_ARGUMENT;
                    }
                }
            }

            std::shared_ptr<jwt_token_data> data = std::make_shared<jwt_token_data>();
            auto result = data->parse(header, payload, signature);
            if (result)
            {

                if (data->is_valid())
                {

                    crypt_key_t key = 0;
                    auto key_it = public_keys_.find(data->get_key_id());
                    if (key_it == public_keys_.end())
                    {
                        JWT_LOG_ERROR("jwt_tokens_repository", 900, "Public key not found for JWT Token Security Provider: " + data->get_key_id());
                        return RX_INVALID_ARGUMENT;
                    }
                    key = key_it->second;


                    if (!check_signature(token.substr(0, idx2), data->get_signature(), key))
                    {
                        JWT_LOG_ERROR("jwt_tokens_repository", 900, "Invalid signature for JWT Token Security Provider: " + data->get_key_id());
						return RX_INVALID_ARGUMENT;
                    }


                    std::scoped_lock _(tokens_lock_);
                    auto it = active_tokens_.find(signature);
                    if (it != active_tokens_.end())
                    {
                        // someone else did it already, return that one
                        jwt_token_security_context::smart_ptr context = rx_create_reference<jwt_token_security_context>(it->second);
                        return context;
                    }
                    else
                    {
                        JWT_LOG_TRACE("jwt_tokens_repository", 900, "Adding JWT Token for user "s + data->get_user_name() + " to repository");

                        active_tokens_[signature] = data;
                        jwt_token_security_context::smart_ptr context = rx_create_reference<jwt_token_security_context>(data);
                        return context;
                    }
                }
                else
                {
                    return RX_INVALID_ARGUMENT;
                }
            }
            else
            {
                return rx_result_with<jwt_token_security_context::smart_ptr>(result.errors());
            }
        }
    }
    return RX_INVALID_ARGUMENT;
}

bool jwt_tokens_repository::check_signature (string_view_type content, const byte_string& sig, crypt_key_t key)
{
    return rx_crypt_rsa_verify_sha256(
        key,
        content.data(), content.size(),
        sig.data(), sig.size()) == RX_OK;
}


} // namespace jwt_token
} // namespace rx_security
} // namespace rx_internal

