

/****************************************************************************
*
*  lib\rx_io_addr.cpp
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
*  along with rx-platform. It is also available in any rx-platform console
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
	addr_struct->sin_family = AF_INET;
	memzero(addr_struct, sizeof(*addr_struct));
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

// Class rx::io::ip4_address 

ip4_address::ip4_address()
{
    memzero(&storage_, sizeof(storage_));
}

ip4_address::ip4_address(const ip4_address &right)
{
    memcpy(&storage_, &right.storage_, sizeof(storage_));
}

ip4_address::ip4_address (const sockaddr_in* addr)
{
    storage_.size = sizeof(addr);
    storage_.addr = *addr;
}


ip4_address::~ip4_address()
{
}


bool ip4_address::operator==(const ip4_address &right) const
{
    if (storage_.size == right.storage_.size)
        return memcmp(&storage_.addr, &right.storage_.addr, sizeof(storage_.addr)) == 0;
    else
        return false;
}

bool ip4_address::operator!=(const ip4_address &right) const
{
    return !operator==(right);
}


bool ip4_address::operator<(const ip4_address &right) const
{
    if (storage_.size < right.storage_.size)
        return true;
    else if (storage_.size > right.storage_.size)
        return false;
    else
        return memcmp(&storage_.addr, &right.storage_.addr, sizeof(storage_.addr)) < 0;
}

bool ip4_address::operator>(const ip4_address &right) const
{
    if (storage_.size > right.storage_.size)
        return true;
    else if (storage_.size < right.storage_.size)
        return false;
    else
        return memcmp(&storage_.addr, &right.storage_.addr, sizeof(storage_.addr)) > 0;
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
    if (ep == nullptr)
    {
        memzero(&storage_, sizeof(storage_));
    }
    else
    {
        if (ep->data_length != sizeof(sockaddr_in))
            return "Invalid argument size.";
        else
        {
            sockaddr_in temp = *((sockaddr_in*)(ep->data));
            if(temp.sin_family!=AF_INET)
                return "Invalid socket address type.";
            storage_.size = sizeof(sockaddr_in);
            storage_.addr = temp;
        }
    }
    return true;
}

const sockaddr_in* ip4_address::get_address () const
{
    if (storage_.size == 0)
        return nullptr;
    else
        return &storage_.addr;
}

bool ip4_address::is_null () const
{
    return storage_.size == 0;
}

bool ip4_address::is_valid () const
{
    return storage_.addr.sin_family != AF_INET;
}

string_type ip4_address::to_string () const
{
    if (is_valid())
        return get_ip4_addr_string(&storage_.addr);
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
	return false;
}

const protocol_address* ip4_address::to_protocol_address () const
{
	return reinterpret_cast<const protocol_address*>(&storage_.addr);
}


// Parameterized Class rx::io::numeric_address 

template <typename defT>
numeric_address<defT>::numeric_address()
    : storage_({ 0,0 })
{
}

template <typename defT>
numeric_address<defT>::numeric_address(const numeric_address<defT> &right)
    : storage_(right.storage_)
{
}

template <typename defT>
numeric_address<defT>::numeric_address (defT val)
    : storage_({ sizeof(defT), val })
{
}


template <typename defT>
numeric_address<defT>::~numeric_address()
{
}


template <typename defT>
bool numeric_address<defT>::operator==(const numeric_address<defT> &right) const
{
    if (storage_.size == right.storage_.size)
        return storage_.value == right.storage_.value;
    else
        return false;
}

template <typename defT>
bool numeric_address<defT>::operator!=(const numeric_address<defT> &right) const
{
    return !operator==(right);
}


template <typename defT>
bool numeric_address<defT>::operator<(const numeric_address<defT> &right) const
{
    if (storage_.size < right.storage_.size)
        return true; 
    else if (storage_.size > right.storage_.size)
        return false;
    else
        return storage_.value < right.storage_.value;
}

template <typename defT>
bool numeric_address<defT>::operator>(const numeric_address<defT> &right) const
{
    if (storage_.size > right.storage_.size)
        return true;
    else if (storage_.size < right.storage_.size)
        return false;
    else
        return storage_.value > right.storage_.value;
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
    if (ep == nullptr || ep->data_length == 0)
    {
        storage_ = endpoint_storage{ 0,0 };
    }
    else
    {
        if (ep->data_length != sizeof(defT))
            return "Invalid endpoint format, wrong data length.";
        storage_.size = sizeof(defT);
        storage_.value = *((defT*)ep->data);
    }
    return true;
}

template <typename defT>
defT numeric_address<defT>::get_address () const
{
    if (is_null())
        return 0;
    else
        return storage_.value;
}

template <typename defT>
bool numeric_address<defT>::is_null () const
{
    return storage_.size == 0;
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
    if (is_null())
        return "<null>";
    
    std::ostringstream ss;
    ss << storage_.value;
    return ss.str();
}

template <typename defT>
rx_result numeric_address<defT>::parse (const string_type& what)
{
    if (what.empty())
    {
        // null value
        storage_ = endpoint_storage{ 0,0 };
        return true;
    }
    else
    {
        storage_.size = sizeof(defT);
        auto res = std::from_chars(what.data(), what.data() + what.size(), storage_.value);
        if (res.ec == std::errc())
            return true;
        else if (res.ec == std::errc::invalid_argument)
            return "Invalid argument.";
        else if (res.ec == std::errc::result_out_of_range)
            return "Out of range.";
        else
            return "Undefined error.";
    }
}

template <typename defT>
const protocol_address* numeric_address<defT>::to_protocol_address () const
{
    if (storage_.size == 0)
        return nullptr;
    else
        return reinterpret_cast<const protocol_address*>(&storage_);
}

// explicit template instanation
template class numeric_address<int8_t>;
template class numeric_address<uint8_t>;
template class numeric_address<int16_t>;
template class numeric_address<uint16_t>;
template class numeric_address<int32_t>;
template class numeric_address<uint32_t>;
template class numeric_address<int64_t>;
template class numeric_address<uint64_t>;
} // namespace io
} // namespace rx

