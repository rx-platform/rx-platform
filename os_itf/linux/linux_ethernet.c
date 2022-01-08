

/****************************************************************************
*
*  os_itf\linux\linux_ethernet.c
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




#include "rx_linux.h"

#include "os_itf/rx_ositf.h"


// ethernet stuff
int rx_list_eth_cards(struct ETH_interface** interfaces, size_t* count)
{
	return RX_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////}
// IP 4 addresses
int rx_add_ip_address(uint32_t addr, uint32_t mask, int itf, ip_addr_ctx_t* ctx)
{
	return RX_ERROR;
}
int rx_remove_ip_address(ip_addr_ctx_t ctx)
{
	return RX_ERROR;
}
int rx_is_valid_ip_address(uint32_t addr, uint32_t mask)
{
	return RX_ERROR;
}


