

/****************************************************************************
*
*  sys_internal\rx_security\rx_x509_security.cpp
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


// rx_x509_security
#include "sys_internal/rx_security/rx_x509_security.h"


/////////////////////////////////////////////////////////////
// logging macros for console library
#define X509_LOG_INFO(src,lvl,msg) RX_LOG_INFO("X509",src,lvl,msg)
#define X509_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("X509",src,lvl,msg)
#define X509_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("X509",src,lvl,msg)
#define X509_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("X509",src,lvl,msg)
#define X509_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("X509",src,lvl,msg)
#define X509_LOG_TRACE(src,lvl,msg) RX_TRACE("X509",src,lvl,msg)

#define RX_CERT_SECURITY_INFO "{rx-platform} X509 Security Provider"


namespace rx_internal {

namespace rx_security {
namespace
{
string_type g_cert_security_name = RX_CERT_SECURITY_NAME;
}

// Class rx_internal::rx_security::certificate_security_provider


const string_type& certificate_security_provider::get_name ()
{
    return g_cert_security_name;
}

string_type certificate_security_provider::get_info ()
{
    return RX_CERT_SECURITY_INFO;
}

rx_result certificate_security_provider::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
    auto result = x509_certificates::instance().initialize(host, data);
    if (!result)
        return result;

    return true;
}

void certificate_security_provider::deinitialize ()
{
    x509_certificates::instance().deinitialize();
}

rx_result_with<security::security_context_ptr> certificate_security_provider::create_host_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    auto result = x509_certificates::instance().get_certificate("host");
    if (result)
    {
        x509_security_context::smart_ptr ret_ptr = rx_create_reference< x509_security_context>("host", rx_gate::instance().get_node_name());
        ret_ptr->set_certificate(result.move_value());
        return security::security_context_ptr(ret_ptr);
    }
    else
    {
        return result.errors();
    }
}

rx_result_with<security::security_context_ptr> certificate_security_provider::create_system_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    auto result = x509_certificates::instance().get_certificate("system");
    if (result)
    {
        auto ret_ptr = rx_create_reference< x509_security_context>("system", rx_gate::instance().get_node_name());
        ret_ptr->set_certificate(result.move_value());
        return security::security_context_ptr(ret_ptr);
    }
    else
    {
        return result.errors();
    }
}

rx_result_with<security::security_context_ptr> certificate_security_provider::create_world_context (hosting::rx_platform_host* host, configuration_data_t& data)
{
    auto result = x509_certificates::instance().get_certificate("world");
    if (result)
    {
        auto ret_ptr = rx_create_reference<x509_security_context>(rx_gate::instance().get_instance_name(), rx_gate::instance().get_node_name());
        ret_ptr->set_certificate(result.move_value());
        return security::security_context_ptr(ret_ptr);
    }
    else
    {
        return result.errors();
    }
}


// Class rx_internal::rx_security::x509_security_context

x509_security_context::x509_security_context()
{
}

x509_security_context::x509_security_context (const string_view_type name, const string_view_type location)
{
    user_name_ = name;
    location_ = location;
    full_name_ = user_name_ + '@' + location_;
}


x509_security_context::~x509_security_context()
{
}



void x509_security_context::set_certificate (x509_certificate_ptr cert)
{
    certificate_ = std::move(cert);
    if (is_authenticated())
    {
        if (full_name_.empty())
        {
            string_type princ = certificate_->get_principal_name();
            auto idx = princ.find('@');
            if (idx == string_type::npos)
            {
                user_name_ = princ;
                location_ = rx_gate::instance().get_node_name();
                full_name_ = user_name_ + "@";
                full_name_ += location_;
            }
            else
            {
                user_name_ = princ.substr(0, idx);
                location_ = princ.substr(idx + 1);
                full_name_ = princ;
            }
        }
    }
}

void x509_security_context::set_certificate (rx_certificate_t* cert)
{
    x509_certificate_ptr ptr = rx_create_reference<x509_certificate>();
    if (ptr->attach(cert))
    {
        set_certificate(ptr);
    }
}

bool x509_security_context::is_authenticated () const
{
    return certificate_ && certificate_->is_valid();
}

rx_result x509_security_context::serialize (base_meta_writer& stream) const
{
    byte_string data;
    if (is_authenticated())
    {
        data = certificate_->get_bytes();
    }
    if (!stream.write_bytes("der", &data[0], data.size()))
        return stream.get_error();

    return true;
}

rx_result x509_security_context::deserialize (base_meta_reader& stream)
{
    byte_string data;
    if (!stream.read_bytes("der", data))
        return stream.get_error();
    if (data.empty())
        return "Empty Certificate fingerprint.";

    x509_certificate_ptr cert = rx_create_reference<x509_certificate>();
    auto result = cert->open_from_bytes(data);
    if (!result)
        return result;
    byte_string thumb = cert->get_thumbprint();
    result = cert->open_from_thumb(thumb);
    if (!result)
        return result;

    set_certificate(cert);

    return true;
}


// Class rx_internal::rx_security::x509_certificates

x509_certificates::x509_certificates()
{
}


x509_certificates::~x509_certificates()
{
}



x509_certificates& x509_certificates::instance ()
{
    static x509_certificates g_obj;
    return g_obj;
}

rx_result x509_certificates::initialize (hosting::rx_platform_host* host, configuration_data_t& data)
{
    string_type host_name = host->get_host_name();
    for (const auto& one : data.other.certificates)
    {
        if (one.first.size() > 5)
        {
            string_type cert_name = one.first.substr(5);
            x509_certificate_ptr cert_ptr = rx_create_reference<x509_certificate>();
            byte_string thumb = rx_hexstr_to_bytes(one.second);
            if (!thumb.empty())
            {
                auto result = cert_ptr->open_from_thumb(thumb);
                if (result)
                {
                    certificates_.emplace(std::move(cert_name), std::move(cert_ptr));
                }
                else
                {
                    std::ostringstream ss;
                    ss << "Error opening certificate "
                        << cert_name
                        << " with thumbprint "
                        << one.second
                        << " :"
                        << result.errors_line();

                    X509_LOG_WARNING("x509_certificates", 999, ss.str());
                }
            }
            else
            {
                std::ostringstream ss;
                ss << "Error opening certificate "
                    << cert_name
                    << " : thumbprint not a hex string";

                X509_LOG_WARNING("x509_certificates", 999, ss.str());
            }
        }
    }
    return true;
}

void x509_certificates::deinitialize ()
{
}

rx_result_with<x509_certificate_ptr> x509_certificates::get_certificate (const string_type& name)
{
    std::scoped_lock _(lock_);
    auto it = certificates_.find(name);
    if (it != certificates_.end())
    {
        return it->second;
    }
    else
    {
        return RX_INVALID_ARGUMENT;
    }
}


// Class rx_internal::rx_security::x509_certificate

x509_certificate::x509_certificate()
{
    rx_init_certificate_struct(&cert_);
}


x509_certificate::~x509_certificate()
{
    if (rx_is_valid_certificate(&cert_))
    {
        rx_close_certificate(&cert_);
    }
}



rx_certificate_t* x509_certificate::get_certificate_object ()
{
    if (rx_is_valid_certificate(&cert_))
        return &cert_;
    else
        return nullptr;
}

rx_result x509_certificate::open_from_thumb (const byte_string& thumb)
{
    if (rx_is_valid_certificate(&cert_))
    {
        rx_close_certificate(&cert_);
        rx_init_certificate_struct(&cert_);
    }
    if (RX_OK == rx_open_certificate_from_thumb(&cert_, nullptr, (const uint8_t*)&thumb[0], thumb.size()))
    {
        return true;
    }
    else
    {
        return rx_result::create_from_last_os_error("Unable to open certificate.");
    }
}

rx_result x509_certificate::open_from_bytes (const byte_string& data)
{
    if (rx_is_valid_certificate(&cert_))
    {
        rx_close_certificate(&cert_);
        rx_init_certificate_struct(&cert_);
    }
    if (RX_OK == rx_open_certificate_from_bytes(&cert_, (const uint8_t*)&data[0], data.size()))
    {
        return true;
    }
    else
    {
        return rx_result::create_from_last_os_error("Unable to open certificate.");
    }
}

rx_result x509_certificate::attach (rx_certificate_t* cert)
{
    cert_ = *cert;
    rx_init_certificate_struct(cert);
    return true;
}

bool x509_certificate::is_valid () const
{
    return rx_is_valid_certificate(&cert_);
}

string_type x509_certificate::get_principal_name () const
{
    string_value_struct data;
    if (RX_OK == rx_certificate_get_principal_name(&cert_, &data))
    {
        string_type ret = rx_c_str(&data);
        rx_destory_string_value_struct(&data);
        return ret;
    }
    else
    {
        return "";
    }
}

byte_string x509_certificate::get_thumbprint () const
{
    byte_string ret;
    bytes_value_struct data;
    if (RX_OK == rx_certificate_get_thumbprint(&cert_, &data))
    {
        ret = byte_string((std::byte*)data.value, (std::byte*)data.value + data.size);
    }
    return ret;
}

byte_string x509_certificate::get_bytes () const
{
    byte_string ret;
    bytes_value_struct data;
    if (RX_OK == rx_certificate_get_bytes(&cert_, &data))
    {
        ret = byte_string((std::byte*)data.value, (std::byte*)data.value + data.size);
    }
    return ret;
}

x509_certificate::x509_certificate(x509_certificate&& right) noexcept
{
    cert_ = right.cert_;
    rx_init_certificate_struct(&right.cert_);
}
x509_certificate& x509_certificate::operator=(x509_certificate&& right)noexcept
{
    cert_ = right.cert_;
    rx_init_certificate_struct(&right.cert_);
    return *this;
}
} // namespace rx_security
} // namespace rx_internal

