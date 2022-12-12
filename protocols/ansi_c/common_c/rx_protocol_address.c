

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_address.c
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


// rx_protocol_address
#include "protocols/ansi_c/common_c/rx_protocol_address.h"


RX_COMMON_API void rx_create_null_address(struct protocol_address_def* def)
{
	def->type = protocol_address_none;
}
RX_COMMON_API void rx_create_uint8_address(struct protocol_address_def* def, uint8_t addr)
{
	def->type = protocol_address_uint8;
	def->value.uint8_address = addr;
}
RX_COMMON_API void rx_create_uint16_address(struct protocol_address_def* def, uint16_t addr)
{
	def->type = protocol_address_uint16;
	def->value.uint16_address = addr;
}
RX_COMMON_API void rx_create_uint32_address(struct protocol_address_def* def, uint32_t addr)
{
	def->type = protocol_address_uint32;
	def->value.uint32_address = addr;
}
RX_COMMON_API void rx_create_uint64_address(struct protocol_address_def* def, uint64_t addr)
{
	def->type = protocol_address_uint64;
	def->value.uint64_address = addr;
}
RX_COMMON_API void rx_create_mac_address(struct protocol_address_def* def, const uint8_t* addr)
{
	def->type = protocol_address_mac;
	memcpy(def->value.mac_address, addr, MAC_ADDR_SIZE);
}
RX_COMMON_API void rx_create_ip4_address(struct protocol_address_def* def, const struct sockaddr_in* addr)
{
	def->type = protocol_address_ip4;
	memcpy(&def->value.ip4_address, addr, sizeof(struct sockaddr_in));
}
RX_COMMON_API void rx_create_ip6_address(struct protocol_address_def* def, const struct sockaddr_in6* addr)
{
	def->type = protocol_address_ip6;
	memcpy(&def->value.ip6_address, addr, sizeof(struct sockaddr_in6));
}
RX_COMMON_API void rx_create_string_address(struct protocol_address_def* def, const char* addr)
{
	size_t size = 0;
	def->type = protocol_address_string;
	if (addr)
	{
		size = strlen(addr) + 1;
		def->value.string_address = (char*)malloc(size);
		if(def->value.string_address)
			strcpy(def->value.string_address, addr);
	}
	else
	{
		def->value.string_address = NULL;
	}
}
RX_COMMON_API void rx_create_bytes_address(struct protocol_address_def* def, const uint8_t* addr, size_t size)
{
	def->type = protocol_address_bytes;
	if (addr && size > 0)
	{
		def->value.bytes_address.size = size;
		def->value.bytes_address.address = (uint8_t*)malloc(size);
		memcpy(def->value.bytes_address.address, addr, size);
	}
	else
	{
		def->value.bytes_address.size = 0;
		def->value.bytes_address.address = NULL;
	}
}

RX_COMMON_API int rx_is_null_address(const struct protocol_address_def* def)
{
	return def == NULL || def->type == protocol_address_none;
}
RX_COMMON_API rx_protocol_result_t rx_extract_uint8_address(const struct protocol_address_def* def, uint8_t* addr)
{
	if (def->type != protocol_address_uint8)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.uint8_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_uint16_address(const struct protocol_address_def* def, uint16_t* addr)
{
	if (def->type != protocol_address_uint16)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.uint16_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_uint32_address(const struct protocol_address_def* def, uint32_t* addr)
{
	if (def->type != protocol_address_uint32)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.uint32_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_uint64_address(const struct protocol_address_def* def, uint64_t* addr)
{
	if (def->type != protocol_address_uint64)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.uint64_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_mac_address(const struct protocol_address_def* def, const uint8_t** addr)
{
	if (def->type != protocol_address_mac)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.mac_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_ip4_address(const struct protocol_address_def* def, const struct sockaddr_in** addr)
{
	if (def->type != protocol_address_ip4 || def->value.ip4_address.sin_family != AF_INET)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = &def->value.ip4_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_ip6_address(const struct protocol_address_def* def, const struct sockaddr_in6** addr)
{
	if (def->type != protocol_address_ip6 || def->value.ip4_address.sin_family != AF_INET6)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = &def->value.ip6_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_string_address(const struct protocol_address_def* def, const char** addr)
{
	if (def->type != protocol_address_string)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.string_address;
	return RX_PROTOCOL_OK;
}
RX_COMMON_API rx_protocol_result_t rx_extract_bytes_address(const struct protocol_address_def* def, const uint8_t** addr, size_t* size)
{
	if (def->type != protocol_address_bytes)
		return RX_PROTOCOL_INVALID_ADDR;
	*addr = def->value.bytes_address.address;
	*size = def->value.bytes_address.size;
	return RX_PROTOCOL_OK;
}

RX_COMMON_API void rx_free_address(struct protocol_address_def* def)
{
	switch (def->type)
	{
	case protocol_address_none:
		return;
	case protocol_address_uint8:
	case protocol_address_uint16:
	case protocol_address_uint32:
	case protocol_address_uint64:
	case protocol_address_mac:
	case protocol_address_ip4:
	case protocol_address_ip6:
		break;
	case protocol_address_string:
		if (def->value.string_address != NULL)
			free(def->value.string_address);
		break;
	case protocol_address_bytes:
		if (def->value.bytes_address.address != NULL)
			free(def->value.bytes_address.address);
		break;
	}
	def->type = protocol_address_none;
}
RX_COMMON_API void rx_copy_address(struct protocol_address_def* dest, const struct protocol_address_def* src)
{
	if (!rx_is_null_address(dest))
		rx_free_address(dest);
	memcpy(dest, src, sizeof(struct protocol_address_def));
	switch (src->type)
	{
	case protocol_address_none:
	case protocol_address_uint8:
	case protocol_address_uint16:
	case protocol_address_uint32:
	case protocol_address_uint64:
	case protocol_address_mac:
	case protocol_address_ip4:
	case protocol_address_ip6:
		return;
	case protocol_address_string:
		rx_create_string_address(dest, src->value.string_address);
		return;
	case protocol_address_bytes:
		rx_create_bytes_address(dest, src->value.bytes_address.address, src->value.bytes_address.size);
		return;
	}
}
RX_COMMON_API void rx_move_address(struct protocol_address_def* dest, struct protocol_address_def* src)
{
	if (!rx_is_null_address(dest))
		rx_free_address(dest);
	memcpy(dest, src, sizeof(struct protocol_address_def));
	// just null source address
	src->type = protocol_address_none;
}


