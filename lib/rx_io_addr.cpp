

/****************************************************************************
*
*  lib\rx_io_addr.cpp
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


// rx_io_addr
#include "lib/rx_io_addr.h"



namespace rx {

namespace io {
rx_result fill_ip4_addr(const string_type& addr, uint16_t port, sockaddr_in* addr_struct)
{
	memzero(addr_struct, sizeof(*addr_struct));
    addr_struct->sin_family = AF_INET;
	addr_struct->sin_port = htons(port);
	if (addr.empty())
	{
		addr_struct->sin_addr.s_addr = INADDR_ANY;
		return true;
	}
	else
	{
		auto ret = inet_pton(AF_INET, addr.c_str(), &addr_struct->sin_addr);
		if (ret == 1)
			return true;
		else
			return addr + " is not valid IP4 address";
	}
}

string_type get_ip4_addr_string(const sockaddr_in* addr_struct)
{
    if (addr_struct == NULL)
		 return "ERROR, invalid pointer.";
	if (addr_struct->sin_family != AF_INET)
		return "ERROR, wrong address type.";
	char buff[INET_ADDRSTRLEN];
	const char* addr = inet_ntop(AF_INET, &addr_struct->sin_addr, buff, sizeof(buff) / sizeof(buff[0]));
	if (addr)
	{
		char buff_end[INET_ADDRSTRLEN + 0x10];
		sprintf(buff_end, "%s:%d", addr, (int)htons(addr_struct->sin_port));
		return buff_end;
	}
	return "ERROR parsing IP4 address.";
}
string_type get_ip4_addr_string(const string_type& addr, uint16_t port)
{
    std::ostringstream ss;
    ss << addr << ':' << port;
    return ss.str();
}

template<typename T>
void create_numeric_address(protocol_address* def, T addr);
template<typename T>
rx_protocol_result_t extract_numeric_address(const protocol_address* def, T& addr);

template<>
void create_numeric_address(protocol_address* def, uint8_t addr)
{
    rx_create_uint8_address(def, addr);
}
template<>
void create_numeric_address(protocol_address* def, uint16_t addr)
{
    rx_create_uint16_address(def, addr);
}
template<>
void create_numeric_address(protocol_address* def, uint32_t addr)
{
    rx_create_uint32_address(def, addr);
}
template<>
void create_numeric_address(protocol_address* def, uint64_t addr)
{
    rx_create_uint64_address(def, addr);
}
template<>
rx_protocol_result_t extract_numeric_address(const protocol_address* def, uint8_t& addr)
{
    return rx_extract_uint8_address(def, &addr);
}
template<>
rx_protocol_result_t extract_numeric_address(const protocol_address* def, uint16_t& addr)
{
    return rx_extract_uint16_address(def, &addr);
}
template<>
rx_protocol_result_t extract_numeric_address(const protocol_address* def, uint32_t& addr)
{
    return rx_extract_uint32_address(def, &addr);
}
template<>
rx_protocol_result_t extract_numeric_address(const protocol_address* def, uint64_t& addr)
{
    return rx_extract_uint64_address(def, &addr);
}



// Class rx::io::ip4_address 

ip4_address::ip4_address()
{
    rx_create_null_address(this);
}

ip4_address::ip4_address(const ip4_address &right)
{
    rx_create_null_address(this);
    rx_copy_address(this, &right);
}

ip4_address::ip4_address (const sockaddr_in* addr)
{
    rx_create_ip4_address(this, addr);
}

ip4_address::ip4_address (const sockaddr* addr)
{
    if (addr->sa_family != AF_INET)
    {
        rx_create_null_address(this);
    }
    else
    {
        rx_create_ip4_address(this, (const sockaddr_in*)addr);
    }
}

ip4_address::ip4_address (const string_type& addr, uint16_t port)
{
    sockaddr_in storage;
    unsigned long num_addr = INADDR_ANY;
    if(!addr.empty())
        num_addr = inet_addr(addr.c_str());
    memzero(&storage, sizeof(storage));
    storage.sin_family = AF_INET;
    storage.sin_port = htons(port);
    storage.sin_addr.s_addr = num_addr;
    rx_create_ip4_address(this, &storage);
}


bool ip4_address::operator==(const ip4_address &right) const
{
    if (is_null() && right.is_null())
    {
        return true;
    }
    else if (!is_null() && !right.is_null())
    {
        const sockaddr_in* me = NULL;
        const sockaddr_in* other = NULL;

        auto result = rx_extract_ip4_address(this, &me);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        result = rx_extract_ip4_address(&right, &other);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        return me->sin_port == other->sin_port && GET_IP4_ADDR(me) == GET_IP4_ADDR(other);
    }
    else
    {
        return false;
    }
}

bool ip4_address::operator!=(const ip4_address &right) const
{
    return !operator==(right);
}


bool ip4_address::operator<(const ip4_address &right) const
{
    if (is_null() && !right.is_null())
    {
        return true;
    }
    else if (!is_null() && !right.is_null())
    {
        const sockaddr_in* me = NULL;
        const sockaddr_in* other = NULL;

        auto result = rx_extract_ip4_address(this, &me);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        result = rx_extract_ip4_address(&right, &other);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        return me->sin_port < other->sin_port
            || (me->sin_port == other->sin_port && GET_IP4_ADDR(me) < GET_IP4_ADDR(other));
    }
    else
    {
        return false;
    }
}

bool ip4_address::operator>(const ip4_address &right) const
{
    if (!is_null() && right.is_null())
    {
        return true;
    }
    else if (!is_null() && !right.is_null())
    {
        const sockaddr_in* me = NULL;
        const sockaddr_in* other = NULL;

        auto result = rx_extract_ip4_address(this, &me);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        result = rx_extract_ip4_address(&right, &other);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        return me->sin_port > other->sin_port
            || (me->sin_port == other->sin_port && GET_IP4_ADDR(me) > GET_IP4_ADDR(other));
    }
    else
    {
        return false;
    }
}

bool ip4_address::operator<=(const ip4_address &right) const
{
    return operator==(right) && operator<(right);
}

bool ip4_address::operator>=(const ip4_address &right) const
{
    return operator==(right) && operator>(right);
}



rx_result ip4_address::parse (const protocol_address* ep)
{
    rx_free_address(this);
    if (rx_is_null_address(ep))
        return true;// null address is always valid

    if (ep->type == protocol_address_ip4)
    {
        rx_copy_address(this, ep);
        return true;
    }
    else
    {
        return "Invalid address expected IP4 format.";
    }
}

const sockaddr_in* ip4_address::get_ip4_address () const
{
    if (rx_is_null_address(this))
        return nullptr;
    else
        return &this->value.ip4_address;
}

bool ip4_address::is_null () const
{
    return rx_is_null_address(this) != 0 ? true : false;
}

bool ip4_address::is_valid () const
{
    return !is_null() && this->value.ip4_address.sin_family == AF_INET;
}

string_type ip4_address::to_string () const
{
    if (is_valid())
        return get_ip4_addr_string(&this->value.ip4_address);
    else
        return "<null>";
}

rx_result ip4_address::parse (const string_type& what)
{
	auto idx = what.rfind(':');
	uint16_t port = 0;
	if (idx != string_type::npos && idx != what.size())
	{
		auto result = std::from_chars(&what[idx + 1], &what[what.size() - 1], port);
		if (result.ec != std::errc())
		{
			std::ostringstream ss;
			ss
				<< "Error parsing port at char "
				<< idx
				<< ","
				<< std::make_error_code(result.ec).message()
				<< "!";
			return ss.str();
		}
	}
	sockaddr_in temp;
	auto ret = fill_ip4_addr(what.substr(0, idx), port, &temp);
	if (!ret)
	{
		std::ostringstream ss;
		ss
			<< "Error parsing ip address "
			<< idx
			<< ","
			<< ret.errors()[0]
			<< "!";
		return ss.str();
	}
    if (!rx_is_null_address(this))
        rx_free_address(this);
    rx_create_ip4_address(this, &temp);
	return true;
}

rx_result ip4_address::parse (const string_type& addr, uint16_t port)
{
    sockaddr_in temp;
    auto ret = fill_ip4_addr(addr, port, &temp);
    if (!ret)
    {
        std::ostringstream ss;
        ss
            << "Error parsing ip address "
            << addr
            << ","
            << ret.errors()[0]
            << "!";
        return ss.str();
    }
    if (!rx_is_null_address(this))
        rx_free_address(this);
    rx_create_ip4_address(this, &temp);
    return true;
}

const sockaddr* ip4_address::get_address () const
{
    if (rx_is_null_address(this))
        return nullptr;
    else
        return (const sockaddr*)&this->value.ip4_address;
}

ip4_address::ip4_address(ip4_address&& right) noexcept
{
    rx_create_null_address(this);
    rx_move_address(this, &right);
}
ip4_address& ip4_address::operator=(const ip4_address& right)
{
    rx_free_address(this);
    rx_copy_address(this, &right);
    return *this;
}
ip4_address& ip4_address::operator=(ip4_address&& right) noexcept
{
    rx_free_address(this);
    rx_move_address(this, &right);
    return *this;
}
// Parameterized Class rx::io::numeric_address 

template <typename defT>
numeric_address<defT>::numeric_address()
{
    rx_create_null_address(this);
}

template <typename defT>
numeric_address<defT>::numeric_address(const numeric_address<defT> &right)
{
    rx_create_null_address(this);
    rx_copy_address(this, &right);
}

template <typename defT>
numeric_address<defT>::numeric_address (defT val)
{
    create_numeric_address<defT>(this, val);
}


template <typename defT>
bool numeric_address<defT>::operator==(const numeric_address<defT> &right) const
{
    if (is_null() && right.is_null())
    {
        return true;
    }
    else if (!is_null() && !right.is_null())
    {
        defT me = 0;
        defT other = 0;

        auto result = extract_numeric_address<defT>(this, me);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        result = extract_numeric_address<defT>(&right, other);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        return me==other;
    }
    else
    {
        return false;
    }
}

template <typename defT>
bool numeric_address<defT>::operator!=(const numeric_address<defT> &right) const
{
    return !operator==(right);
}


template <typename defT>
bool numeric_address<defT>::operator<(const numeric_address<defT> &right) const
{
    if (is_null() && !right.is_null())
    {
        return true;
    }
    else if (!is_null() && !right.is_null())
    {
        defT me = 0;
        defT other = 0;

        auto result = extract_numeric_address<defT>(this, me);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        result = extract_numeric_address<defT>(&right, other);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        return me < other;
    }
    else
    {
        return false;
    }
}

template <typename defT>
bool numeric_address<defT>::operator>(const numeric_address<defT> &right) const
{
    if (!is_null() && right.is_null())
    {
        return true;
    }
    else if (!is_null() && !right.is_null())
    {
        defT me = 0;
        defT other = 0;

        auto result = extract_numeric_address<defT>(this, me);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        result = extract_numeric_address<defT>(&right, other);
        if (result != RX_PROTOCOL_OK)
        {
            RX_ASSERT(false);
            return false;
        }
        return me > other;
    }
    else
    {
        return false;
    }
}

template <typename defT>
bool numeric_address<defT>::operator<=(const numeric_address<defT> &right) const
{
    return operator==(right) && operator<(right);
}

template <typename defT>
bool numeric_address<defT>::operator>=(const numeric_address<defT> &right) const
{
    return operator==(right) && operator>(right);
}



template <typename defT>
rx_result numeric_address<defT>::parse (const protocol_address* ep)
{
    rx_free_address(this);
    if (rx_is_null_address(ep))
        return true;// null address is always valid
    defT addr;
    auto result = extract_numeric_address(ep, addr);
    if (result!=RX_PROTOCOL_OK)
    {
        rx_create_null_address(this);
        return rx_protocol_error_message(result);
    }
    else
    {
        create_numeric_address(this, addr);
        return true;
    }
}

template <typename defT>
defT numeric_address<defT>::get_address () const
{
    if (is_null())
    {
        return 0;
    }
    else
    {
        defT addr = 0;
        if (extract_numeric_address(this, addr) == RX_PROTOCOL_OK)
            return addr;
        else
            return 0;
    }
}

template <typename defT>
bool numeric_address<defT>::is_null () const
{
    return this->type == protocol_address_none;
}

template <typename defT>
bool numeric_address<defT>::is_valid () const
{
  // numeric addr is always valid
  return true;

}

template <typename defT>
string_type numeric_address<defT>::to_string () const
{
    defT addr = 0;
    if (is_null() || extract_numeric_address(this, addr) != RX_PROTOCOL_OK)
        return "<null>";

    std::ostringstream ss;
    ss << (uint64_t)addr;
    return ss.str();
}

template <typename defT>
rx_result numeric_address<defT>::parse (const string_type& what)
{
    rx_free_address(this);
    if (what.empty())
    {
        return "Null argument";
    }
    else
    {
        defT addr;
        auto res = std::from_chars(what.data(), what.data() + what.size(), addr);
        if (res.ec == std::errc())
        {
            create_numeric_address(this, addr);
            return true;
        }
        else if (res.ec == std::errc::invalid_argument)
        {
            return "Invalid argument.";
        }
        else if (res.ec == std::errc::result_out_of_range)
        {
            return "Out of range.";
        }
        else
        {
            return "Undefined error.";
        }
    }
}


template <typename defT>
numeric_address<defT>::numeric_address(numeric_address<defT>&& right) noexcept
{
    rx_create_null_address(this);
    rx_move_address(this, &right);
}
template <typename defT>
numeric_address<defT>& numeric_address<defT>::operator=(const numeric_address<defT>& right)
{
    rx_free_address(this);
    rx_copy_address(this, &right);
    return *this;
}
template <typename defT>
numeric_address<defT>& numeric_address<defT>::operator=(numeric_address<defT>&& right) noexcept
{
    rx_free_address(this);
    rx_move_address(this, &right);
    return *this;
}
// explicit template instantiation
template class numeric_address<uint8_t>;
template class numeric_address<uint16_t>;
template class numeric_address<uint32_t>;
template class numeric_address<uint64_t>;
// Class rx::io::any_address 

any_address::any_address()
{
    rx_create_null_address(this);
}

any_address::any_address (const protocol_address* ep)
{
    rx_create_null_address(this);
    if(ep)
        rx_copy_address(this, ep);
}


any_address::~any_address()
{
    rx_free_address(this);
}



bool any_address::is_null () const
{
    return this->type == protocol_address_none;
}

string_type any_address::to_string () const
{
    switch (this->type)
    {
    case protocol_address_none:
        return "null";
    case protocol_address_uint8:
        {
            numeric_address<uint8_t> temp;
            temp.parse(this);
            return temp.to_string();
        }
    case protocol_address_uint16:
        {
            numeric_address<uint16_t> temp;
            temp.parse(this);
            return temp.to_string();
        }
    case protocol_address_uint32:
        {
            numeric_address<uint32_t> temp;
            temp.parse(this);
            return temp.to_string();
        }
    case protocol_address_uint64:
        {
            numeric_address<uint64_t> temp;
            temp.parse(this);
            return temp.to_string();
        }
    case protocol_address_mac:
        return "nemamac";
    case protocol_address_ip4:
        {
            ip4_address temp;
            temp.parse(this);
            return temp.to_string();
        }
    case protocol_address_ip6:
        return "nemaip6";
    case protocol_address_string:
        return this->value.string_address;
    case protocol_address_bytes:
        return "nemabytes";
    default:
        RX_ASSERT(false);
        return "err";
    }
}


any_address::any_address(any_address&& right) noexcept
{
    rx_create_null_address(this);
    rx_move_address(this, &right);
}
any_address::any_address(const any_address& right)
{
    rx_create_null_address(this);
    rx_copy_address(this, &right);
}
any_address& any_address::operator=(const any_address& right)
{
    rx_free_address(this);
    rx_copy_address(this, &right);
    return *this;
}
any_address& any_address::operator=(any_address&& right) noexcept
{
    rx_free_address(this);
    rx_move_address(this, &right);
    return *this;
}

bool any_address::operator<(const any_address& right) const
{
    if (type == right.type)
    {
        switch (type)
        {
        case protocol_address_none:
            return false;
        case protocol_address_uint8:
            return value.uint8_address < right.value.uint8_address;
        case protocol_address_uint16:
            return value.uint16_address < right.value.uint16_address;
        case protocol_address_uint32:
            return value.uint32_address < right.value.uint32_address;
        case protocol_address_uint64:
            return value.uint64_address < right.value.uint64_address;
        case protocol_address_mac:
            return memcmp(value.mac_address, &right.value.mac_address, MAC_ADDR_SIZE) < 0;
        case protocol_address_ip4:
            if (value.ip4_address.sin_port == right.value.ip4_address.sin_port)
            {
                auto ip1= GET_IP4_ADDR((&value.ip4_address));
                auto ip2 = GET_IP4_ADDR((&right.value.ip4_address));
                return ip1 < ip2;
            }
            else
            {
                return value.ip4_address.sin_port < right.value.ip4_address.sin_port;
            }
        case protocol_address_ip6:
            RX_ASSERT(false);
            return false;
        case protocol_address_string:
            if (value.string_address == nullptr && right.value.string_address == nullptr)
                return false;
            if (value.string_address == nullptr && right.value.string_address != nullptr)
                return true;
            if (value.string_address != nullptr && right.value.string_address == nullptr)
                return false;
            else
                return strcmp(value.string_address, right.value.string_address) < 0;
        case protocol_address_bytes:
            if (value.bytes_address.size == right.value.bytes_address.size)
            {
                if (value.bytes_address.size == 0)
                    return true;
                else
                    return memcmp(value.bytes_address.address, right.value.bytes_address.address, value.bytes_address.size) < 0;
            }
            else
            {
                return value.bytes_address.size < right.value.bytes_address.size;
            }
        default:
            RX_ASSERT(false);
            return false;
        }
    }
    else
    {
        return type < right.type;
    }
}
} // namespace io
} // namespace rx

