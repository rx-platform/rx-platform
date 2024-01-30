

/****************************************************************************
*
*  os_itf\linux\linux_ethernet.c
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




#include "rx_linux.h"

#include "os_itf/rx_ositf.h"


#include <ifaddrs.h>

typedef uint16_t ether_ethtype_t;
typedef uint16_t ether_vlan_tci_t;

typedef uint32_t ether_interval_t;

#define ETHER_DEFAULT_PACKET_SIZE 2048

typedef struct ether_t
{
    int socket_fd;
    int last_errno;
} ether_t;

typedef uint8_t ether_octet_t;
typedef int ether_size_t;

typedef struct ether_il_prefix_t {
    uint8_t address_len;
    uint8_t* address;
    uint8_t prefixlen;
} ether_il_prefix_t;

typedef struct ether_il_device_t
{
    char* name;
    uint8_t mac_len;
    uint8_t* mac;
    int interface_index;
    int prefix_count;
    ether_il_prefix_t* prefixes;
    ether_il_prefix_t* prefix_masks;
} ether_il_device_t;

typedef struct ether_il_t
{
    int size;
    ether_il_device_t* devices;
} ether_il_t;



#define loop for(;;)

#define ETHER_IL_RESPONSE_BUFFER_SIZE 8192

#define ETHER_RESULT(name, code, string) ETHER_##name = code,
typedef enum ether_result_t
{

    ETHER_RESULT(OK, 0, "OK")
    ETHER_RESULT(ERROR_GENERIC, -100, "Error Generic")

    ETHER_RESULT(ERROR_BIND, -101, "Error bind")
    ETHER_RESULT(ERROR_FCNTL, -102, "Error fcntl")
    ETHER_RESULT(ERROR_IOCTL, -103, "Error ioctl")
    ETHER_RESULT(ERROR_NLMSG, -104, "Error nlmsg result")
    ETHER_RESULT(ERROR_OPEN, -105, "Error open")
    ETHER_RESULT(ERROR_PARTIAL_SENT, -106, "Error partial sent")
    ETHER_RESULT(ERROR_POLL, -107, "Error poll")
    ETHER_RESULT(ERROR_RECV, -108, "Error recv")
    ETHER_RESULT(ERROR_RECVMSG, -109, "Error recvmsg")
    ETHER_RESULT(ERROR_SEND, -110, "Error send")
    ETHER_RESULT(ERROR_SETSOCKOPT, -111, "Error setsockopt")
    ETHER_RESULT(ERROR_SOCKET, -112, "Error socket")
    ETHER_RESULT(ERROR_TIMEOUT, -113, "Error timeout")
    ETHER_RESULT_END_
} ether_result_t;
#undef ETHER_RESULT



#define ADAPTER_NAME_BUFF_SIZE 0x100
#define RAW_SOCK_BUFFER_SIZE 0x10000

typedef struct eth_socket
{
    ether_t gep;
    uint8_t buffer[ETHER_DEFAULT_PACKET_SIZE];
} eth_socket;

typedef struct mac_addr_t
{
	uint8_t addr[MAC_ADDR_SIZE];
} mac_addr_t;



typedef struct nl_req_s nl_req_t;

struct nl_req_s {
    struct nlmsghdr hdr;
    struct rtgenmsg gen;
};

struct IP_list {
    int interface_number;
    unsigned char family;
    unsigned char address_len;
    unsigned char* address;
    unsigned char broadcast_len;
    unsigned char* broadcast;
    /*
      unsigned char broadcast_len;
      unsigned char *broadcast;
      unsigned char local_len;
      unsigned char *local;
    */
    unsigned char label_len;
    unsigned char* label;
    unsigned char prefixlen;
    struct IP_list* next;
};

struct MAC_list {
    int interface_number;
    unsigned short type;
    unsigned char mac_len;
    unsigned char* mac;
    unsigned char name_len;
    unsigned char* name;
    struct MAC_list* next;
};




ether_result_t ether_get_ipl(struct IP_list** ip_list)
{
    int fd;
    struct sockaddr_nl local;
    struct sockaddr_nl kernel;

    struct msghdr rtnl_msg;
    struct iovec io;

    nl_req_t req;
    char reply[ETHER_IL_RESPONSE_BUFFER_SIZE];

    pid_t pid = getpid();
    int end = 0;
    struct IP_list* ipl_head, * ipl_temp;

    ipl_head = (struct IP_list*)NULL;
    ipl_temp = (struct IP_list*)NULL;


    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = pid;
    local.nl_groups = 0;

    if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0)
        return ETHER_ERROR_BIND;

    memset(&rtnl_msg, 0, sizeof(rtnl_msg));
    memset(&kernel, 0, sizeof(kernel));
    memset(&req, 0, sizeof(req));

    kernel.nl_family = AF_NETLINK;

    req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    req.hdr.nlmsg_type = RTM_GETADDR;
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.hdr.nlmsg_seq = 1;
    req.hdr.nlmsg_pid = pid;
    //  req.gen.rtgen_family = AF_PACKET;
    //  req.gen.rtgen_family = AF_INET6;
    req.gen.rtgen_family = AF_INET;

    io.iov_base = &req;
    io.iov_len = req.hdr.nlmsg_len;
    rtnl_msg.msg_iov = &io;
    rtnl_msg.msg_iovlen = 1;
    rtnl_msg.msg_name = &kernel;
    rtnl_msg.msg_namelen = sizeof(kernel);

    sendmsg(fd, (struct msghdr*)&rtnl_msg, 0);

    while (!end)
    {
        int len;
        struct nlmsghdr* msg_ptr;

        struct msghdr rtnl_reply;
        struct iovec io_reply;

        memset(&io_reply, 0, sizeof(io_reply));
        memset(&rtnl_reply, 0, sizeof(rtnl_reply));

        io.iov_base = reply;
        io.iov_len = ETHER_IL_RESPONSE_BUFFER_SIZE;
        rtnl_reply.msg_iov = &io;
        rtnl_reply.msg_iovlen = 1;
        rtnl_reply.msg_name = &kernel;
        rtnl_reply.msg_namelen = sizeof(kernel);

        len = recvmsg(fd, &rtnl_reply, 0);
        if (len)
        {
            for (msg_ptr = (struct nlmsghdr*)reply; NLMSG_OK(msg_ptr, len); msg_ptr = NLMSG_NEXT(msg_ptr, len))
            {
                struct ifaddrmsg* iface;
                struct rtattr* attribute;
                int len;

                switch (msg_ptr->nlmsg_type)
                {
                case NLMSG_NOOP:
                case NLMSG_ERROR:
                case NLMSG_OVERRUN:
                    return ETHER_ERROR_NLMSG;
                case NLMSG_DONE:
                    end = 1;
                    break;
                case RTM_NEWADDR:
                    iface = NLMSG_DATA(msg_ptr);
                    len = msg_ptr->nlmsg_len - NLMSG_LENGTH(sizeof(*iface));
                    ipl_temp = (struct IP_list*)calloc(1, sizeof(struct IP_list));
                    ipl_temp->interface_number = iface->ifa_index;
                    ipl_temp->family = iface->ifa_family;
                    ipl_temp->prefixlen = iface->ifa_prefixlen;
                    ipl_temp->next = (struct IP_list*)NULL;
                    ipl_temp->address_len = 0;
                    ipl_temp->address = (unsigned char*)NULL;
                    ipl_temp->broadcast_len = 0;
                    ipl_temp->broadcast = (unsigned char*)NULL;

                    for (attribute = IFA_RTA(iface); RTA_OK(attribute, len); attribute = RTA_NEXT(attribute, len))
                    {
                        switch (attribute->rta_type)
                        {
                        case IFA_LOCAL:
                            ipl_temp->address_len = attribute->rta_len - sizeof(struct rtattr);
                            //                    printf("address len %d\n", ipl_temp->address_len);
                            ipl_temp->address = (unsigned char*)malloc(ipl_temp->address_len);
                            //                    printf("%p\n",ipl_temp->address);
                            if (ipl_temp->address)
                                memcpy(ipl_temp->address, (char*)RTA_DATA(attribute), ipl_temp->address_len);
                            //                    printf("addr %d.%d.%d.%d\n", ipl_temp->address[0],ipl_temp->address[1],ipl_temp->address[2],ipl_temp->address[3]);
                            break;
                          case IFA_BROADCAST:
                            ipl_temp->broadcast_len= attribute->rta_len - sizeof(struct rtattr);
                            ipl_temp->broadcast=(unsigned char *)malloc(ipl_temp->broadcast_len);
                            if(ipl_temp->broadcast)
                              memcpy(ipl_temp->broadcast, (char *) RTA_DATA(attribute),ipl_temp->broadcast_len);
                            break;
                        }
                    }
                    ipl_temp->next = ipl_head;
                    ipl_head = ipl_temp;

                    break;
                }
            }
        }
    }
    close(fd);

    *ip_list = ipl_head;
    return ETHER_OK;
}


ether_result_t ether_destroy_ipl(struct IP_list* ip_list)
{
    struct IP_list* ipl_next, * ipl_temp;

    ipl_temp = ip_list;
    while (ipl_temp)
    {
        ipl_next = ipl_temp->next;
        free(ipl_temp->address);
        /*    free(ipl_temp->broadcast); */
        /*    free(ipl_temp->local); */
        /*    free(ipl_temp->label); */
        free(ipl_temp);
        ipl_temp = ipl_next;
    }
    return ETHER_OK;
}

ether_result_t ether_get_macl(struct MAC_list** mac_list)
{
    int fd;
    struct sockaddr_nl local;
    struct sockaddr_nl kernel;

    struct msghdr rtnl_msg;
    struct iovec io;

    nl_req_t req;
    char response[ETHER_IL_RESPONSE_BUFFER_SIZE];
    pid_t pid = getpid();
    int end = 0;
    struct MAC_list* macl_head, * macl_temp;

    macl_head = (struct MAC_list*)NULL;
    macl_temp = (struct MAC_list*)NULL;

    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = pid;
    local.nl_groups = 0;

    if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0)
        return ETHER_ERROR_BIND;

    memset(&rtnl_msg, 0, sizeof(rtnl_msg));
    memset(&kernel, 0, sizeof(kernel));
    memset(&req, 0, sizeof(req));

    kernel.nl_family = AF_NETLINK;

    req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    req.hdr.nlmsg_type = RTM_GETLINK;
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.hdr.nlmsg_seq = 1;
    req.hdr.nlmsg_pid = pid;
    req.gen.rtgen_family = AF_PACKET;

    io.iov_base = &req;
    io.iov_len = req.hdr.nlmsg_len;
    rtnl_msg.msg_iov = &io;
    rtnl_msg.msg_iovlen = 1;
    rtnl_msg.msg_name = &kernel;
    rtnl_msg.msg_namelen = sizeof(kernel);

    sendmsg(fd, (struct msghdr*)&rtnl_msg, 0);

    while (!end)
    {
        int len;
        struct nlmsghdr* msg_ptr;
        struct msghdr rtnl_reply;
        struct iovec io_reply;

        memset(&io_reply, 0, sizeof(io_reply));
        memset(&rtnl_reply, 0, sizeof(rtnl_reply));

        io.iov_base = response;
        io.iov_len = ETHER_IL_RESPONSE_BUFFER_SIZE;
        rtnl_reply.msg_iov = &io;
        rtnl_reply.msg_iovlen = 1;
        rtnl_reply.msg_name = &kernel;
        rtnl_reply.msg_namelen = sizeof(kernel);

        len = recvmsg(fd, &rtnl_reply, 0);
        if (len)
        {
            for (msg_ptr = (struct nlmsghdr*)response; NLMSG_OK(msg_ptr, len); msg_ptr = NLMSG_NEXT(msg_ptr, len))
            {
                struct ifinfomsg* iface;
                int len;
                struct rtattr* attribute;

                switch (msg_ptr->nlmsg_type)
                {
                case NLMSG_NOOP:
                case NLMSG_ERROR:
                case NLMSG_OVERRUN:
                    return ETHER_ERROR_NLMSG;
                case NLMSG_DONE:
                    end = 1;
                    break;
                case RTM_NEWLINK:
                    iface = NLMSG_DATA(msg_ptr);
                    len = msg_ptr->nlmsg_len - NLMSG_LENGTH(sizeof(*iface));
                    macl_temp = (struct MAC_list*)calloc(1, sizeof(struct MAC_list));
                    macl_temp->interface_number = iface->ifi_index;
                    macl_temp->type = iface->ifi_type;
                    macl_temp->next = (struct MAC_list*)NULL;
                    for (attribute = IFLA_RTA(iface); RTA_OK(attribute, len); attribute = RTA_NEXT(attribute, len))
                    {
                        switch (attribute->rta_type)
                        {
                        case IFLA_ADDRESS:
                            macl_temp->mac_len = attribute->rta_len - sizeof(struct rtattr);
                            macl_temp->mac = (unsigned char*)malloc(macl_temp->mac_len);
                            if (macl_temp->mac)
                                memcpy(macl_temp->mac, (char*)RTA_DATA(attribute), macl_temp->mac_len);
                            break;
                        case IFLA_IFNAME:
                            {
                                macl_temp->name_len = attribute->rta_len - sizeof(struct rtattr);
                                macl_temp->name = (unsigned char*)malloc(macl_temp->name_len);
                                if (macl_temp->name)
                                    memcpy(macl_temp->name, (char*)RTA_DATA(attribute), macl_temp->name_len);
                            }
                            break;
                        }
                    }
                    macl_temp->next = macl_head;
                    macl_head = macl_temp;

                    break;
                default:
                    break;
                }
            }
        }
    }
    close(fd);
    *mac_list = macl_head;
    return ETHER_OK;
}

ether_result_t ether_destroy_macl(struct MAC_list* mac_list)
{
    struct MAC_list* macl_next, * macl_temp;

    macl_temp = mac_list;
    while (macl_temp)
    {
        macl_next = macl_temp->next;
        //    free(macl_temp->mac); // left allocated in ether_il structure
        //    free(macl_temp->name); // left allocated in ether_il structure
        free(macl_temp->mac);
        free(macl_temp);
        macl_temp = macl_next;
    }
    return ETHER_OK;
}

const struct sockaddr_in* get_net_mask_for_addr(struct ifaddrs* ifap, unsigned char* addr)
{
    uint32_t int_addr;
    int_addr = (uint32_t)addr[0] | (((uint32_t)addr[1])<<8)
        | (((uint32_t)addr[2])<<16)
        | (((uint32_t)addr[3])<<24);
    struct ifaddrs* ifa;
    struct sockaddr_in* sa;

    if(ifap)
    {

        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
                sa = (struct sockaddr_in*)ifa->ifa_addr;
                if(sa->sin_addr.s_addr == int_addr)
                {
                    return  (const struct sockaddr_in*)ifa->ifa_netmask;
                }
            }
        }
    }
    return 0;
}

ether_result_t ether_get_il(ether_il_t* il)
{
    struct MAC_list* mac_list, * macl_temp;
    struct IP_list* ip_list, * ipl_temp;
    int total_number_of_interfaces;
    int max_interface_index;
    ether_result_t result;
    ether_il_device_t* il_devices;
    int i, j;
    int current_index;
    int* offset_index_map;
    int temp;

    if ((result = ether_get_ipl(&ip_list) != ETHER_OK))
        return result;

    //  ether_ipl_to_xml_file(ip_list,stdout);

    if ((result = ether_get_macl(&mac_list) != ETHER_OK))
        return result;

    struct ifaddrs* ifap;


    if(getifaddrs(&ifap)<0)
        ifap=NULL;

    //  ether_macl_to_xml_file(mac_list,stdout);

    total_number_of_interfaces = 0;
    max_interface_index = -1;
    macl_temp = mac_list;
    while (macl_temp)
    {
        if (macl_temp->interface_number > max_interface_index)
            total_number_of_interfaces++;
        macl_temp = macl_temp->next;
    }

    offset_index_map = (int*)calloc(total_number_of_interfaces, sizeof(int));
    current_index = 0;
    macl_temp = mac_list;
    while (macl_temp)
    {
        offset_index_map[current_index] = macl_temp->interface_number;
        current_index++;
        macl_temp = macl_temp->next;
    }

    for (i = 0; i < total_number_of_interfaces; i++)
        for (j = i + 1; j < total_number_of_interfaces; j++)
        {
            if (offset_index_map[i] > offset_index_map[j])
            {
                temp = offset_index_map[i];
                offset_index_map[i] = offset_index_map[j];
                offset_index_map[j] = temp;
            }
        }

    il_devices = (ether_il_device_t*)calloc(total_number_of_interfaces, sizeof(ether_il_device_t));
    il->size = total_number_of_interfaces;

    current_index = 0;
    for (i = 0; i < total_number_of_interfaces; i++)
    {
        current_index = offset_index_map[i];
        il_devices[i].prefixes = (ether_il_prefix_t*)NULL;
        il_devices[i].prefix_count = 0;
        il_devices[i].name = (char*)NULL;
        il_devices[i].mac = (unsigned char*)NULL;
        il_devices[i].interface_index = -1;

        macl_temp = mac_list;
        while (macl_temp)
        {
            if (macl_temp->interface_number == offset_index_map[i])
            {
                il_devices[i].name = (char*)macl_temp->name;
                il_devices[i].mac_len = macl_temp->mac_len;
                il_devices[i].mac = (unsigned char*)calloc(1, macl_temp->mac_len);
                memcpy(il_devices[i].mac, macl_temp->mac, macl_temp->mac_len);
                //        il_devices[i].mac= macl_temp->mac;
                il_devices[i].interface_index = macl_temp->interface_number;
            }
            macl_temp = macl_temp->next;
        }
    }

    for (i = 0; i < total_number_of_interfaces; i++)
    {
        int interface_alias_count;
        int offset;

        il_devices[i].prefix_count = -1;
        ipl_temp = ip_list;
        while (ipl_temp)
        {
            if (ipl_temp->interface_number == il_devices[i].interface_index)
            {
                struct IP_list* ipl_temp2;

                ipl_temp2 = ip_list;
                interface_alias_count = 0;
                while (ipl_temp2)
                {
                    if (ipl_temp2->interface_number == il_devices[i].interface_index)
                        interface_alias_count++;
                    ipl_temp2 = ipl_temp2->next;
                }

                if (il_devices[i].prefix_count == -1)
                {
                    //          printf("prefix count %d ix %d\n",interface_alias_count,il_devices[i].interface_index);
                    il_devices[i].prefix_count = interface_alias_count;
                    il_devices[i].prefixes = (ether_il_prefix_t*)calloc(interface_alias_count, sizeof(ether_il_prefix_t));
                    il_devices[i].prefix_masks = (ether_il_prefix_t*)calloc(interface_alias_count, sizeof(ether_il_prefix_t));

                    offset = 0;
                    ipl_temp2 = ip_list;
                    while (ipl_temp2)
                    {
                        if (ipl_temp2->interface_number == il_devices[i].interface_index)
                        {
                            //              printf("adrl %d, i %d, o %d ix %d\n",ipl_temp2->address_len,i,offset,ipl_temp->interface_number);
                            il_devices[i].prefixes[offset].address = (unsigned char*)0;
                            il_devices[i].prefixes[offset].address = (unsigned char*)calloc(ipl_temp2->address_len, sizeof(unsigned char));
                            if (il_devices[i].prefixes[offset].address)
                            {
                                memcpy(il_devices[i].prefixes[offset].address, ipl_temp2->address, ipl_temp2->address_len);
                                il_devices[i].prefixes[offset].address_len = ipl_temp2->address_len;
                                const struct sockaddr_in* mask=get_net_mask_for_addr(ifap, ipl_temp2->address);
                                if(mask)
                                {
                                    il_devices[i].prefix_masks[offset].address = (unsigned char*)calloc(4, sizeof(unsigned char));
                                    memcpy(il_devices[i].prefix_masks[offset].address, &mask->sin_addr.s_addr, 4);
                                    il_devices[i].prefix_masks[offset].address_len = 4;
                                }
                                else
                                {
                                    il_devices[i].prefix_masks[offset].address = (unsigned char*)calloc(ipl_temp2->broadcast_len, sizeof(unsigned char));
                                    memcpy(il_devices[i].prefix_masks[offset].address, ipl_temp2->broadcast, ipl_temp2->broadcast_len);
                                    il_devices[i].prefix_masks[offset].address_len = ipl_temp2->broadcast_len;
                                }
                            }
                            else
                            {
                                //              printf("unable to alloc\n");
                                il_devices[i].prefixes[offset].address_len = 0;
                                il_devices[i].prefix_masks[offset].address_len = 0;
                            }

                            il_devices[i].prefixes[offset].prefixlen = ipl_temp2->prefixlen;
                            il_devices[i].prefix_masks[offset].prefixlen = ipl_temp2->prefixlen;
                            offset++;
                        }
                        ipl_temp2 = ipl_temp2->next;
                    }
                }
            }
            ipl_temp = ipl_temp->next;
        }
    }

    if(ifap)
        freeifaddrs(ifap);

    free(offset_index_map);
    ether_destroy_ipl(ip_list);
    ether_destroy_macl(mac_list);
    il->devices = il_devices;
    return ETHER_OK;
}

int ether_destroy_il(const ether_il_t* il)
{
    int i;
    int j;

    for (i = 0; i < il->size; i++)
    {
        free(il->devices[i].name);
        free(il->devices[i].mac);
        for (j = 0; j < il->devices[i].prefix_count; j++)
        {
            free(il->devices[i].prefix_masks[j].address);
            free(il->devices[i].prefixes[j].address);
        }
        free(il->devices[i].prefixes);
    }
    free(il->devices);

    return ETHER_OK;
}


ether_result_t ether_open(const char* interface_name, const int nonblocking, ether_t* gep)
{
    struct ifreq req;
    struct sockaddr_ll ll;
    int yes;

    gep->socket_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (gep->socket_fd < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_SOCKET;
    }

    if (nonblocking)
    {
        int flags;

        flags = fcntl(gep->socket_fd, F_GETFL, 0);
        if (flags >= 0 && fcntl(gep->socket_fd, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            gep->last_errno = errno;
            return ETHER_ERROR_FCNTL;
        }
    }

    snprintf(req.ifr_name, IFNAMSIZ, "%s", interface_name);
    if (ioctl(gep->socket_fd, SIOCGIFFLAGS, &req) < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_IOCTL;
    }
    req.ifr_flags |= IFF_PROMISC;
    if (ioctl(gep->socket_fd, SIOCSIFFLAGS, &req) < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_IOCTL;
    }

    memset(&ll, 0, sizeof ll);
    ll.sll_family = PF_PACKET;
    ll.sll_protocol = htons(ETH_P_ALL);
    ll.sll_ifindex = if_nametoindex(interface_name);
    ll.sll_pkttype = PACKET_OUTGOING;
    ll.sll_halen = 0;
    if (bind(gep->socket_fd, (struct sockaddr*)&ll, sizeof ll) < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_BIND;
    }
    yes = 1;
    if (setsockopt(gep->socket_fd, SOL_PACKET, PACKET_AUXDATA, &yes, sizeof yes) < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_SETSOCKOPT;
    }

    return ETHER_OK;
}

ether_result_t ether_close(ether_t* gep)
{
    close(gep->socket_fd);
    gep->socket_fd = -1;
    return ETHER_OK;
}

ether_result_t ether_put(ether_t* gep, const ether_octet_t* buffer, const ether_size_t buffer_length, ether_size_t* sent)
{
    *sent = (ether_size_t)send(gep->socket_fd, (void*)buffer, (size_t)buffer_length, MSG_NOSIGNAL);
    if (*sent < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_SEND;
    }
    if (*sent != buffer_length)
        return ETHER_ERROR_PARTIAL_SENT;
    return ETHER_OK;
}


ether_result_t ether_get_vlan(ether_t* gep, ether_octet_t* buffer, const ether_size_t buffer_max_length, ether_size_t* taken)
{
    ether_vlan_tci_t vlan_tci;
    int had_vlan=0;
    struct msghdr msg;
    struct iovec iov;
    union
    {
        struct cmsghdr cmsg;
        char           buffer_aux[CMSG_SPACE(sizeof(struct tpacket_auxdata))];
    } cmsg_buf;
    struct cmsghdr* cmsg;
    struct tpacket_auxdata* aux;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg_buf;
    msg.msg_controllen = sizeof cmsg_buf;
    msg.msg_flags = 0;
    iov.iov_len = buffer_max_length - 4;
    iov.iov_base = buffer;

    *taken = recvmsg(gep->socket_fd, &msg, 0);
    if (*taken < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_RECVMSG;
    }
    vlan_tci = 0;
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
        if
            (
                cmsg->cmsg_len == CMSG_LEN(sizeof(struct tpacket_auxdata)) &&
                cmsg->cmsg_level == SOL_PACKET &&
                cmsg->cmsg_type == PACKET_AUXDATA
                )
        {
            aux = (struct tpacket_auxdata*)CMSG_DATA(cmsg);
            vlan_tci = (ether_vlan_tci_t)aux->tp_vlan_tci;
            had_vlan=1;
        }
    if(had_vlan && *taken>14)
    {
        memmove(&buffer[12+4], &buffer[12], *taken-12);
        buffer[12]=0x81;
        buffer[13]=0x00;
        buffer[14]=(uint8_t)(vlan_tci&0xff);
        buffer[15]=(uint8_t)(vlan_tci>>8);
        (*taken)+=4;
    }
    return ETHER_OK;
}

ether_result_t ether_wait_for_get(ether_t* gep, const ether_interval_t ms_timeout_interval)
{
    struct pollfd fds;
    int retval;

    fds.fd = gep->socket_fd;
    fds.events = POLLIN;
    retval = poll(&fds, 1, ms_timeout_interval * 10);
    if (retval < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_POLL;
    }
    if (retval > 0 && fds.revents & POLLIN)
        return ETHER_OK;
    return ETHER_ERROR_TIMEOUT;
}


ether_result_t ether_get_vlan_within_interval(ether_t* gep, ether_octet_t* buffer, const ether_size_t buffer_max_length, ether_size_t* taken)
{
    ether_result_t result;

    result = ether_wait_for_get(gep, 1);
    if (result != ETHER_OK)
        return result;
    return ether_get_vlan(gep, buffer, buffer_max_length, taken);
}


ether_result_t ether_get(ether_t* gep, ether_octet_t* buffer,  const ether_size_t buffer_max_length, ether_size_t* taken)
{
    *taken = (ether_size_t)recv(gep->socket_fd, (void*)buffer, (size_t)buffer_max_length, 0);
    if (*taken < 0)
    {
        gep->last_errno = errno;
        return ETHER_ERROR_RECV;
    }
    return ETHER_OK;
}


ether_result_t ether_get_within_interval(ether_t* gep, ether_octet_t* buffer, const ether_size_t buffer_max_length, ether_size_t* taken)
{
    ether_result_t result;

    result = ether_wait_for_get(gep, 1);
    if (result != ETHER_OK)
        return result;
    return ether_get(gep, buffer, buffer_max_length, taken);
}

// ethernet stuff
int rx_list_eth_cards(struct ETH_interface** interfaces, size_t* count)
{
	int i,j;
    ether_il_t il;
    char ip_buffer[0x40];


    ether_result_t res=ether_get_il(&il);
    if(res==ETHER_OK)
    {
        *count=il.size-1;
        *interfaces=(struct ETH_interface*)malloc(sizeof(struct ETH_interface)*(*count));

		for (i = 1; i < il.size; i++)
		{
            strcpy((*interfaces)[i-1].name,il.devices[i].name);
            strcpy((*interfaces)[i-1].description,il.devices[i].name);
            memcpy((*interfaces)[i-1].mac_address, il.devices[i].mac, MAC_ADDR_SIZE);
            (*interfaces)[i-1].index=(uint32_t)(i-1);

            (*interfaces)[i-1].ip_addrs_size = 0;
            for(j = 0; j<il.devices[i].prefix_count; j++)
            {
                if(il.devices[i].prefixes[j].address_len==4
                    && (il.devices[i].prefixes[j].address[0]!=0
                    || il.devices[i].prefixes[j].address[1]!=0
                    || il.devices[i].prefixes[j].address[2]!=0
                    || il.devices[i].prefixes[j].address[3]!=0)
                    && il.devices[i].prefix_masks[j].address_len==4
                    && (il.devices[i].prefix_masks[j].address[0]!=0
                    || il.devices[i].prefix_masks[j].address[1]!=0
                    || il.devices[i].prefix_masks[j].address[2]!=0
                    || il.devices[i].prefix_masks[j].address[3]!=0))
                    (*interfaces)[i-1].ip_addrs_size++;
            }
            if ((*interfaces)[i-1].ip_addrs_size)
            {
                (*interfaces)[i-1].ip_addrs = malloc(sizeof(struct IP_interface) * (*interfaces)[i-1].ip_addrs_size);
                size_t idx2 = 0;
                for(j = 0; j<il.devices[i].prefix_count; j++)
                {
                    if(il.devices[i].prefixes[j].address_len==4
                        && (il.devices[i].prefixes[j].address[0]!=0
                        || il.devices[i].prefixes[j].address[1]!=0
                        || il.devices[i].prefixes[j].address[2]!=0
                        || il.devices[i].prefixes[j].address[3]!=0)
                    && il.devices[i].prefix_masks[j].address_len==4
                    && (il.devices[i].prefix_masks[j].address[0]!=0
                    || il.devices[i].prefix_masks[j].address[1]!=0
                    || il.devices[i].prefix_masks[j].address[2]!=0
                    || il.devices[i].prefix_masks[j].address[3]!=0))
                    {
                        sprintf(ip_buffer,"%d.%d.%d.%d"
                            , (int)il.devices[i].prefixes[j].address[0]
                            , (int)il.devices[i].prefixes[j].address[1]
                            , (int)il.devices[i].prefixes[j].address[2]
                            , (int)il.devices[i].prefixes[j].address[3]);

                        char* dest=(*interfaces)[i-1].ip_addrs[idx2].ip_address;
                        strcpy(dest, ip_buffer);

                        unsigned char brod_addr[4];
                        for(int k=0; k<4; k++)
                        {
                            brod_addr[k]=
                                il.devices[i].prefix_masks[j].address[k]
                                & il.devices[i].prefixes[j].address[k]
                                | (~il.devices[i].prefix_masks[j].address[k]);
                        }

                        sprintf(ip_buffer,"%d.%d.%d.%d"
                            , (int)brod_addr[0]
                            , (int)brod_addr[1]
                            , (int)brod_addr[2]
                            , (int)brod_addr[3]);

                        dest=(*interfaces)[i-1].ip_addrs[idx2].broadcast_address;
                        strcpy(dest, ip_buffer);

                        sprintf(ip_buffer,"%d.%d.%d.%d"
                            , (int)(il.devices[i].prefix_masks[j].address[0]&il.devices[i].prefixes[j].address[0])
                            , (int)(il.devices[i].prefix_masks[j].address[1]&il.devices[i].prefixes[j].address[1])
                            , (int)(il.devices[i].prefix_masks[j].address[2]&il.devices[i].prefixes[j].address[2])
                            , (int)(il.devices[i].prefix_masks[j].address[3]&il.devices[i].prefixes[j].address[3]));

                        dest=(*interfaces)[i-1].ip_addrs[idx2].network;
                        strcpy(dest, ip_buffer);
                        idx2++;
                    }
                }
            }
		}
		ether_destroy_il(&il);
    }
	return RX_OK;
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

uint32_t rx_create_ethernet_socket(const char* adapter_name, peth_socket* psock)
{
	ether_result_t res;
	struct eth_socket* temp = NULL;
	temp=(struct eth_socket*)malloc(sizeof(struct eth_socket));
	memset(temp,0, sizeof(eth_socket));

	res=ether_open(adapter_name,1,&temp->gep);
	if(res==ETHER_OK)
	{
        *psock = temp;
        return RX_OK;
	}
	else
	{
        printf("Error oppenig raw socket:%s\r\n",strerror(temp->gep.last_errno));
	}
	free(temp);
	return RX_ERROR;
}
uint32_t rx_send_ethernet_packet(peth_socket psock, const void* buffer, size_t size)
{
	struct eth_socket* temp = (struct eth_socket*)psock;
	ether_result_t res;
	ether_size_t sent=0;

	res=ether_put(&temp->gep,buffer,size,&sent);
    if(res==ETHER_OK && size==sent)
        return RX_OK;
    else
        return RX_ERROR;
}
uint32_t rx_recive_ethernet_packet(peth_socket psock, uint8_t** buffer, size_t* recv_size, int* has_more, struct timeval* tv)
{
	struct eth_socket* temp = (struct eth_socket*)psock;
	ether_size_t sz = 0;
	ether_result_t res;
	*has_more = 0;

    res=ether_get_vlan_within_interval(&temp->gep,temp->buffer,ETHER_DEFAULT_PACKET_SIZE,&sz);
    //res = ether_get_within_interval(&temp->gep, temp->buffer, ETHER_DEFAULT_PACKET_SIZE, &sz);
    if(res==ETHER_ERROR_TIMEOUT)
    {
        *recv_size=0;
        return RX_OK;
    }
    else if(res==ETHER_OK)
    {
        *recv_size=sz;
        if(sz)
            *buffer=temp->buffer;
        return RX_OK;
    }
	*recv_size = 0;
	return RX_ERROR;
}
uint32_t rx_close_ethernet_socket(peth_socket psock)
{
    struct eth_socket* temp = (struct eth_socket*)psock;
	ether_close(&temp->gep);

	return RX_OK;
}


