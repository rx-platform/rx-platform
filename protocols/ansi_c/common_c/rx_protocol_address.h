

/****************************************************************************
*
*  protocols\ansi_c\common_c\rx_protocol_address.h
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


#ifndef rx_protocol_address_h
#define rx_protocol_address_h 1


#include "rx_protocol_errors.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDR_SIZE 6

enum protocol_address_type
{
	protocol_address_none = 0,
	protocol_address_uint8 = 1,
	protocol_address_uint16 = 2,
	protocol_address_uint32 = 3,
	protocol_address_uint64 = 4,
	protocol_address_mac = 5,
	protocol_address_ip4 = 6,
	protocol_address_ip6 = 7,
	protocol_address_string = 8,
	protocol_address_bytes = 9
};
struct bytes_address
{
	uint8_t* address;
	size_t size;
};
union protocol_address_value
{
	uint8_t uint8_address;
	uint16_t uint16_address;
	uint32_t uint32_address;
	uint64_t uint64_address;
	uint8_t mac_address[MAC_ADDR_SIZE];
	struct sockaddr_in ip4_address;
	struct sockaddr_in6 ip6_address;
	char* string_address;
	struct bytes_address bytes_address;
};

typedef struct protocol_address_def
{
	enum protocol_address_type type;
	union protocol_address_value value;

} protocol_address;


void rx_create_null_address(struct protocol_address_def* def);
void rx_create_uint8_address(struct protocol_address_def* def, uint8_t addr);
void rx_create_uint16_address(struct protocol_address_def* def, uint16_t addr);
void rx_create_uint32_address(struct protocol_address_def* def, uint32_t addr);
void rx_create_uint64_address(struct protocol_address_def* def, uint64_t addr);
void rx_create_mac_address(struct protocol_address_def* def, uint8_t* addr);
void rx_create_ip4_address(struct protocol_address_def* def, const struct sockaddr_in* addr);
void rx_create_ip6_address(struct protocol_address_def* def, const struct sockaddr_in6* addr);
void rx_create_string_address(struct protocol_address_def* def, const char* addr);
void rx_create_bytes_address(struct protocol_address_def* def, const uint8_t* addr, size_t size);

int rx_is_null_address(const struct protocol_address_def* def);
rx_protocol_result_t rx_extract_uint8_address(const struct protocol_address_def* def, uint8_t* addr);
rx_protocol_result_t rx_extract_uint16_address(const struct protocol_address_def* def, uint16_t* addr);
rx_protocol_result_t rx_extract_uint32_address(const struct protocol_address_def* def, uint32_t* addr);
rx_protocol_result_t rx_extract_uint64_address(const struct protocol_address_def* def, uint64_t* addr);
rx_protocol_result_t rx_extract_mac_address(const struct protocol_address_def* def, const uint8_t** addr);
rx_protocol_result_t rx_extract_ip4_address(const struct protocol_address_def* def, const struct sockaddr_in** addr);
rx_protocol_result_t rx_extract_ip6_address(const struct protocol_address_def* def, const struct sockaddr_in6** addr);
rx_protocol_result_t rx_extract_string_address(const struct protocol_address_def* def, const char** addr);
rx_protocol_result_t rx_extract_bytes_address(const struct protocol_address_def* def, const uint8_t** addr, size_t* size);

void rx_free_address(struct protocol_address_def* def);
void rx_copy_address(struct protocol_address_def* dest, const struct protocol_address_def* src);
void rx_move_address(struct protocol_address_def* dest, struct protocol_address_def* src);



#ifdef __cplusplus
}
#endif


#endif
