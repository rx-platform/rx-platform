

/****************************************************************************
*
*  os_itf\windows\win_ethernet.c
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



#include "rx_win.h"

#include "os_itf/rx_ositf.h"

#include <stdio.h>


// ethernet stuff
int rx_list_eth_cards(struct ETH_interface** interfaces, size_t* count)
{
	IP_ADAPTER_INFO info[20];
	ULONG size = sizeof(info);
	DWORD ret;
	size_t idx;

	ret = GetAdaptersInfo(info, &size);
	if (ret != ERROR_SUCCESS)
		return RX_ERROR;

	IP_ADAPTER_INFO* iter = info;

	*count = 0;
	// iterate to get count first
	do
	{

		if (iter->Type == MIB_IF_TYPE_ETHERNET || iter->Type==IF_TYPE_IEEE80211)
			(*count)++;

		iter = iter->Next;

	} while (iter);

	if (*count > 0)
	{
		*interfaces = malloc(sizeof(struct ETH_interface) * (*count));
		iter = info;
		idx = 0;
		// iterate to get count first
		do
		{

			if (iter->Type == MIB_IF_TYPE_ETHERNET || iter->Type==IF_TYPE_IEEE80211)
			{
				(*interfaces)[idx].index = idx;
				strcpy((*interfaces)[idx].name, iter->AdapterName);
				strcpy((*interfaces)[idx].description, iter->Description);
				memcpy((*interfaces)[idx].mac_address, iter->Address, MAC_ADDR_SIZE);
				idx++;
			}

			iter = iter->Next;

		} while (iter);
	}
	return RX_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////}
// IP 4 addresses
int rx_add_ip_address(uint32_t addr, uint32_t mask, int itf, ip_addr_ctx_t* ctx)
{
	ULONG NTEContext = 0;
	ULONG NTEInstance = 0;

	if (NO_ERROR == AddIPAddress(addr, mask, itf, &NTEContext, &NTEInstance))
	{
		*ctx = NTEContext;
		return RX_OK;
	}
	return RX_ERROR;
}
int rx_remove_ip_address(ip_addr_ctx_t ctx)
{
	if (ctx)
	{
		if (DeleteIPAddress(ctx) == NO_ERROR)
		{
			return RX_OK;
		}
	}
	return RX_ERROR;
}
int rx_is_valid_ip_address(uint32_t addr, uint32_t mask)
{
	BYTE tablebuff[4096];

	int ret = 0;

	MIB_IPADDRTABLE* data = (MIB_IPADDRTABLE*)tablebuff;
	ULONG size = sizeof(tablebuff);

	uint32_t err = GetIpAddrTable(data, &size, FALSE);

	if (SUCCEEDED(err))
	{
		for (uint32_t i = 0; i < data->dwNumEntries; i++)
		{
			if (addr == data->table[i].dwAddr &&
				mask == data->table[i].dwMask &&
				(data->table[i].wType & (MIB_IPADDR_DISCONNECTED | MIB_IPADDR_DELETED)) == 0)
			{
				ret = 1;
				break;
			}
		}
	}

	return ret;
}



