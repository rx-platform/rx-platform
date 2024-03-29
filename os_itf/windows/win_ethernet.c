

/****************************************************************************
*
*  os_itf\windows\win_ethernet.c
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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



#include "third-party/win10pcap/NdisDriverCommon.h"

// ethernet stuff
int rx_list_eth_cards(struct ETH_interface** interfaces, size_t* count)
{
	IP_ADAPTER_INFO info[20];
	ULONG size = sizeof(info);
	DWORD ret;
	size_t idx;

	size_t temp_ip_count = 0;

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
		*interfaces = rx_heap_alloc(sizeof(struct ETH_interface) * (*count));
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
				(*interfaces)[idx].ip_addrs_size = 0;
				IP_ADDR_STRING* iter2 = &iter->IpAddressList;
				do
				{
					char* temp_addr = iter2->IpAddress.String;
					if (temp_addr && strcmp(temp_addr, "0.0.0.0") != 0)
						(*interfaces)[idx].ip_addrs_size++;
					iter2= iter2->Next;

				} while (iter2);
				if ((*interfaces)[idx].ip_addrs_size)
				{
					(*interfaces)[idx].ip_addrs = rx_heap_alloc(sizeof(struct IP_interface) * (*interfaces)[idx].ip_addrs_size);
					iter2 = &iter->IpAddressList;
					size_t idx2 = 0;
					do
					{
						char* temp_addr = iter2->IpAddress.String;
						char* temp_addr2 = iter2->IpMask.String;

						if (temp_addr && strcmp(temp_addr, "0.0.0.0") != 0)
						{
							(*interfaces)[idx].ip_addrs[idx2].broadcast_address[0] = '\0';
							(*interfaces)[idx].ip_addrs[idx2].network[0] = '\0';
							struct in_addr addr1;
							struct in_addr mask1;
							struct in_addr brod;
							int ret = inet_pton(AF_INET, temp_addr, &addr1);
							if (ret == 1)
							{
								ret = inet_pton(AF_INET, temp_addr2, &mask1);
								if (ret == 1)
								{
									brod.S_un.S_addr = mask1.S_un.S_addr & addr1.S_un.S_addr | (~mask1.S_un.S_addr);
									strcpy((*interfaces)[idx].ip_addrs[idx2].broadcast_address, inet_ntoa(brod));
									mask1.S_un.S_addr = mask1.S_un.S_addr & addr1.S_un.S_addr;
									strcpy((*interfaces)[idx].ip_addrs[idx2].network, inet_ntoa(mask1));
								}
							}
							strcpy((*interfaces)[idx].ip_addrs[idx2].ip_address, temp_addr);
						}
						idx2++;
						iter2 = iter2->Next;

					} while (iter2);
				}

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

struct eth_socket_data
{
	uint8_t* write_buffer;
	uint8_t* buffer;
	HANDLE handle;
	size_t next_read;
	size_t current_size;
};

uint32_t rx_create_ethernet_socket(const char* adapter_name, peth_socket* psock)
{
	struct eth_socket_data* pret = rx_heap_alloc(sizeof(struct eth_socket_data));
	*psock = NULL;
	char name_buff[0x100];
	sprintf(name_buff, "\\\\.\\WTCAP_A_%s", adapter_name);
	pret->handle=CreateFileA(name_buff, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (pret->handle == INVALID_HANDLE_VALUE)
	{
		rx_heap_free(pret);
		return RX_ERROR;
	}
	pret->next_read = 0;
	pret->current_size = 0;
	pret->buffer=VirtualAlloc(NULL, SL_EXCHANGE_BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
	if (pret->buffer == NULL)
	{
		CloseHandle(pret->handle);
		rx_heap_free(pret);
		return RX_ERROR;
	}
	pret->write_buffer = VirtualAlloc(NULL, SL_EXCHANGE_BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
	if (pret->write_buffer == NULL)
	{
		VirtualFree(pret->buffer, 0, MEM_RELEASE);
		CloseHandle(pret->handle);
		rx_heap_free(pret);
		return RX_ERROR;
	}
	*psock = pret;
	return RX_OK;
}
uint32_t rx_send_ethernet_packet(peth_socket psock, const void* buffer, size_t size)
{
	struct eth_socket_data* sock = (struct eth_socket_data*)psock;
	SL_NUM_PACKET(sock->write_buffer) = 1;
	SL_SIZE_OF_PACKET(sock->write_buffer, 0) = (UINT)size;
	memcpy(SL_ADDR_OF_PACKET(sock->write_buffer, 0), buffer, size);
	DWORD writen;
	BOOL ret = WriteFile(sock->handle, sock->write_buffer, SL_EXCHANGE_BUFFER_SIZE, &writen, NULL);
	if (ret)
	{
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}

void fill_next_ether_packet(struct eth_socket_data* sock, uint8_t** buffer, size_t* recv_size, int* has_more, struct timeval* tv)
{
	size_t size = SL_SIZE_OF_PACKET(sock->buffer, sock->next_read);
	uint8_t* data = (uint8_t*)SL_ADDR_OF_PACKET(sock->buffer, sock->next_read);
	SL_TIMEVAL* ts = SL_TIMESTAMP_OF_PACKET(sock->buffer, sock->next_read);
	tv->tv_sec = ts->tv_sec;
	tv->tv_usec = ts->tv_usec;
	*recv_size = size;
	*buffer = data;
	*has_more = 1;
	sock->next_read++;
}

uint32_t rx_recive_ethernet_packet(peth_socket psock, uint8_t** buffer, size_t* recv_size, int* has_more, struct timeval* tv)
{
	struct eth_socket_data* sock = (struct eth_socket_data*)psock;
	if (sock->next_read < sock->current_size)
	{
		fill_next_ether_packet(sock, buffer, recv_size, has_more, tv);
		return RX_OK;
	}
	else
	{
		sock->next_read = 0;
		sock->current_size = 0;
		DWORD read;
		BOOL ret = ReadFile(sock->handle, sock->buffer, SL_EXCHANGE_BUFFER_SIZE, &read, NULL);
		if (ret)
		{
			sock->current_size = SL_NUM_PACKET(sock->buffer);
			if (sock->current_size == 0)
			{
				*recv_size = 0;
				*buffer = NULL;
				*has_more = 0;
			}
			else
			{
				fill_next_ether_packet(psock, buffer, recv_size, has_more, tv);
			}
			return RX_OK;
		}
		else
		{
			return RX_ERROR;
		}
	}
}
uint32_t rx_close_ethernet_socket(peth_socket psock)
{
	struct eth_socket_data* sock = (struct eth_socket_data*)psock;
	if (sock == NULL)
		return RX_ERROR;
	VirtualFree(sock->buffer, 0, MEM_RELEASE);
	VirtualFree(sock->write_buffer, 0, MEM_RELEASE);
	CloseHandle(sock->handle);
	return RX_OK;
}



