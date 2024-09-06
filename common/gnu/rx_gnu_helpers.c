

/****************************************************************************
*
*  common\gnu\rx_gnu_helpers.c
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


#include "pch.h"

#include <pthread.h>
#include "../rx_common.h"
#include "../rx_common_version.h"
#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"



int rx_hd_timer = 1;
int g_init_count = 0;


size_t g_page_size = 0;

uint32_t g_trans_id = 1;
uint32_t g_handle_id = 1;

RX_COMMON_API runtime_transaction_id_t rx_get_new_transaction_id()
{
    uint32_t ret =__sync_add_and_fetch(&g_trans_id, 1);
    if(ret==0)
        ret =__sync_add_and_fetch(&g_trans_id, 1);
    return ret;
}
RX_COMMON_API runtime_handle_t rx_get_new_handle()
{
    uint32_t ret =__sync_add_and_fetch(&g_handle_id, 1);
    if(ret==0)
        ret =__sync_add_and_fetch(&g_handle_id, 1);
    return ret;
}

rx_protocol_result_t rx_init_protocols(struct rx_hosting_functions* memory);
rx_protocol_result_t rx_deinit_protocols();


void handle_error(const char* file, int lineno, const char* msg) {
    fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
    //ERR_print_errors_fp(stderr);
    exit(1);
}

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)


int g_is_debug_instance = 0;


extern SSL_CTX* g_ssl_ctx;



void create_ssl_context()
{
/* create the SSL server context */
    g_ssl_ctx = SSL_CTX_new(TLS_method());

    SSL_CTX_set_options(g_ssl_ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    //SSL_CTX_set_default_verify_paths(g_ssl_ctx);

    //SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

}
size_t rx_init_heap(size_t initial_heap, size_t heap_alloc, size_t heap_trigger, size_t bucket_size);

const uint64_t g_my_version = ((uint64_t)RX_COMMON_MAJOR_VERSION << 48)
    | (((uint64_t)RX_COMMON_MINOR_VERSION & 0xffff) << 32)
    | ((uint64_t)RX_COMMON_BUILD_NUMBER);

uint64_t g_init_version = 0;

RX_COMMON_API int rx_init_common_library(const rx_platform_init_data* init_data)
{
    if (init_data->version == 0)
        return RX_ERROR;
    g_init_version = init_data->version;
    if (g_init_version > g_my_version)
        return RX_ERROR;

    g_is_debug_instance = init_data->is_debug;
    if(g_init_count == 0)
    {
        rx_init_heap(
            init_data->rx_initial_heap_size != 0 ? init_data->rx_initial_heap_size : 2 * 1024 * 1024
            , init_data->rx_alloc_heap_size != 0 ? init_data->rx_alloc_heap_size : 1 * 1024 * 1024
            , init_data->rx_heap_alloc_trigger != 0 ? init_data->rx_heap_alloc_trigger : 950
            , init_data->rx_bucket_capacity != 0 ? init_data->rx_bucket_capacity : 0x1000);

        rx_hd_timer = init_data->rx_hd_timer;

        g_init_count = 1;

        // query page size for optimization purpose
        g_page_size = sysconf(_SC_PAGESIZE);

        rx_init_protocols(NULL);


        /* SSL library initialisation */
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        create_ssl_context();

        return RX_OK;
    }
    else
    {
        return RX_ERROR;
    }
}
RX_COMMON_API void rx_deinit_common_library()
{

	SSL_CTX_free(g_ssl_ctx);
}


RX_COMMON_API int rx_is_debug_instance()
{
    return g_is_debug_instance;
}

uint32_t rx_border_rand(uint32_t min, uint32_t max)
{
    if (max > min)
    {
        uint32_t diff = (max - min);
        if (diff > RAND_MAX)
        {
            int shifts = 0;
            while (diff > RAND_MAX)
            {
                shifts++;
                diff >>= 1;
            }
            uint32_t gen = rand() << shifts;
            gen = gen % (max - min) + min;
            return gen;
        }
        else
        {
            uint32_t gen = rand();
            gen = gen % (max - min) + min;
            return gen;
        }
    }
    else
        return min;
}

void* rx_allocate_os_memory(size_t size)
{
    void* addr= mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return addr;
}
void rx_deallocate_os_memory(void* p, size_t size)
{
    int ret = munmap(p, size);
    if (ret == -1)
        perror("Unmap");
}

size_t rx_os_page_size()
{
    RX_ASSERT(g_page_size);
    return g_page_size;
}

RX_COMMON_API rx_module_handle_t rx_load_library(const char* path)
{
    rx_module_handle_t ret = dlopen(path, RTLD_NOW);
    if(ret==0)
        printf("\r\n**********%s\r\n", dlerror());
    return ret;
}
RX_COMMON_API rx_func_addr_t rx_get_func_address(rx_module_handle_t module_handle, const char* name)
{
    return dlsym(module_handle, name);
}
RX_COMMON_API void rx_unload_library(rx_module_handle_t module_handle)
{
    if (module_handle)
        dlclose(module_handle);
}

RX_COMMON_API rx_timer_ticks_t rx_get_tick_count()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t ret = (uint64_t)ts.tv_sec * 1000;
	ret = ret + (uint64_t)ts.tv_nsec / 1000000ul;
	return (uint64_t)ret;
}
RX_COMMON_API rx_timer_ticks_t rx_get_us_ticks()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t ret = (uint64_t)ts.tv_sec * 1000000ul;
	ret = ret + (uint64_t)ts.tv_nsec / 1000ul;
	return ret;
}

RX_COMMON_API void rx_ms_sleep(uint32_t timeout)
{
    if (timeout)
    {
        struct timespec ts;
        ts.tv_sec = timeout / 1000;
        ts.tv_nsec = timeout % 1000 * 1000000;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
    }
    else//sleep 0 just give over the procesor
        sched_yield();
}
RX_COMMON_API void rx_us_sleep(uint64_t timeout)
{
    if (timeout)
    {
        struct timespec ts;
        ts.tv_sec = timeout / 1000000ul;
        ts.tv_nsec = timeout % 1000000ul * 1000;
        clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
    }
    else//sleep 0 just give over the procesor
        sched_yield();
}

#define TIME_CONVERSION_CONST  116444736000000000ull

uint64_t timeval_to_rx_time(const struct timespec* tv)
{
    if (tv->tv_sec == 0 && tv->tv_nsec == 0)
        return 0;
    else
    {
        uint64_t temp = ((uint64_t)tv->tv_nsec) / 100 + ((uint64_t)tv->tv_sec) * 10000000;
        temp += TIME_CONVERSION_CONST;
        return temp;
    }

}

void rx_time_to_timeval(uint64_t rx_time,struct timespec* tv)
{
    if (rx_time< TIME_CONVERSION_CONST)
    {
        tv->tv_sec=0;
        tv->tv_nsec=0;
    }
    else
    {

        uint64_t temp = rx_time-TIME_CONVERSION_CONST;
        tv->tv_sec=temp/10000000;
        tv->tv_nsec=temp%10000000;
        tv->tv_nsec=tv->tv_nsec*100;
    }

}

RX_COMMON_API int rx_os_get_system_time(struct rx_time_struct_t* st)
{
    struct timespec tv;
    int ret=clock_gettime(CLOCK_REALTIME,&tv);
    if(ret==-1)
        return RX_ERROR;
    else
    {
        st->t_value=timeval_to_rx_time(&tv);
        return RX_OK;
    }
}
RX_COMMON_API int rx_os_to_local_time(struct rx_time_struct_t* st)
{
    assert(0);
    return RX_ERROR;
}
RX_COMMON_API int rx_os_to_utc_time(struct rx_time_struct_t* st)
{
    assert(0);
    return RX_ERROR;
}
RX_COMMON_API int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full)
{
    struct tm tm;
    struct timespec tv;
    rx_time_to_timeval(st->t_value,&tv);
    time_t tt=tv.tv_sec;
    memzero(&tm, sizeof(tm));
    if(NULL == gmtime_r(&tt,&tm))
        return RX_ERROR;
    full->year=tm.tm_year+1900;
    full->month=tm.tm_mon+1;
    full->day=tm.tm_mday;
    full->w_day=tm.tm_wday;
    full->hour=tm.tm_hour;
    full->minute=tm.tm_min;
    full->second=tm.tm_sec;
    full->milliseconds=tv.tv_nsec/1000000;

    return RX_OK;
}
RX_COMMON_API int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st)
{
    struct timespec tv;
    struct tm tm;
    memzero(&tm, sizeof(tm));
    tm.tm_year=full->year-1900;
    tm.tm_mon=full->month-1;
    tm.tm_mday=full->day;
    tm.tm_hour=full->hour;
    tm.tm_min=full->minute;
    tm.tm_sec=full->second;
    time_t tt=mktime(&tm);
    if((time_t)(-1)==tt)
        return RX_ERROR;
    tv.tv_nsec=full->milliseconds*1000000ll;
    tv.tv_sec=tt;
    st->t_value=timeval_to_rx_time(&tv);

    return RX_OK;
}

/////////////////////////////////////
// uuid code


void linux_uuid_to_uuid(const uuid_t* uuid, rx_uuid_t* u)
{
    memcpy(u->Data4, &(*uuid)[8], 8);
    u->Data1 = ntohl(*((uint32_t*)&(*uuid)[0]));
    u->Data2 = ntohs(*((uint16_t*)&(*uuid)[4]));
    u->Data3 = ntohs(*((uint16_t*)&(*uuid)[6]));
}

void uuid_to_linux_uuid(const rx_uuid_t* u, uuid_t* uuid)
{
    memcpy(&(*uuid)[8], u->Data4, 8);
    *((uint32_t*)&(*uuid)[0]) = htonl(u->Data1);
    *((uint16_t*)&(*uuid)[4]) = ntohs(u->Data2);
    *((uint16_t*)&(*uuid)[6]) = ntohs(u->Data3);
}

RX_COMMON_API void rx_generate_new_uuid(rx_uuid_t* u)
{
    uuid_t uuid;
    uuid_generate(uuid);
    linux_uuid_to_uuid(&uuid, u);
}
RX_COMMON_API int rx_uuid_to_string(const rx_uuid_t* u, char* str)
{
    uuid_t uuid;
    uuid_to_linux_uuid(u, &uuid);
    uuid_unparse(uuid, str);
    return RX_OK;
}
RX_COMMON_API int rx_string_to_uuid(const char* str, rx_uuid_t* u)
{
    uuid_t uuid;
    if (uuid_parse(str, uuid) < 0)
        return RX_ERROR;
    linux_uuid_to_uuid(&uuid, u);
    return RX_OK;
}



RX_COMMON_API void rx_slim_lock_create(pslim_lock_t plock)
{
    uintptr_t addr_offset=((uintptr_t)plock)&0x7;
    pthread_mutex_t* mtx = (pthread_mutex_t*)&plock->data[8 - addr_offset];
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(mtx, &attr);
}
RX_COMMON_API void rx_slim_lock_destroy(pslim_lock_t plock)
{
    uintptr_t addr_offset=((uintptr_t)plock)&0x7;
    pthread_mutex_t* mtx = (pthread_mutex_t*)&plock->data[8 - addr_offset];
    pthread_mutex_destroy(mtx);
}
RX_COMMON_API void rx_slim_lock_aquire(pslim_lock_t plock)
{
    uintptr_t addr_offset=((uintptr_t)plock)&0x7;
    pthread_mutex_t* mtx = (pthread_mutex_t*)&plock->data[8 - addr_offset];
    pthread_mutex_lock(mtx);
}
RX_COMMON_API void rx_slim_lock_release(pslim_lock_t plock)
{
    uintptr_t addr_offset=((uintptr_t)plock)&0x7;
    pthread_mutex_t* mtx = (pthread_mutex_t*)&plock->data[8 - addr_offset];
    pthread_mutex_unlock(mtx);
}

RX_COMMON_API void rx_rw_slim_lock_create(prw_slim_lock_t plock)
{
    rx_slim_lock_create((pslim_lock_t)plock);
}
RX_COMMON_API void rx_rw_slim_lock_destroy(prw_slim_lock_t plock)
{
    rx_slim_lock_destroy((pslim_lock_t)plock);
}
RX_COMMON_API void rx_rw_slim_lock_aquire_reader(prw_slim_lock_t plock)
{
    rx_slim_lock_aquire((pslim_lock_t)plock);
}
RX_COMMON_API void rx_rw_slim_lock_release_reader(prw_slim_lock_t plock)
{
    rx_slim_lock_release((pslim_lock_t)plock);
}
RX_COMMON_API void rx_rw_slim_lock_aquire_writter(prw_slim_lock_t plock)
{
    rx_slim_lock_aquire((pslim_lock_t)plock);
}
RX_COMMON_API void rx_rw_slim_lock_release_writter(prw_slim_lock_t plock)
{
    rx_slim_lock_release((pslim_lock_t)plock);
}
///////////////////////////////////////////////////////////////////////////////////////////////////


RX_COMMON_API uint32_t rx_handle_wait(sys_handle_t what, uint32_t timeout)
{
    struct pollfd pfds;
    int ret;
    eventfd_t buff = 0;

    pfds.fd = what;
    pfds.events = POLLIN;
    pfds.revents = 0;
    ret = poll(&pfds, 1, (int)timeout);
    if (ret == 0)
        return RX_WAIT_TIMEOUT;
    else if (ret == 1)
    {
        //do the read to release it
        ret = read(what, &buff, sizeof(buff));
        if (ret > 0)
            return RX_WAIT_0;
    }
    return RX_WAIT_ERROR;

}
RX_COMMON_API uint32_t rx_handle_wait_us(sys_handle_t what, uint64_t timeout)
{
    struct pollfd pfds;
    int ret;
    eventfd_t buff = 0;

    struct timespec ts;
    ts.tv_sec = timeout / 1000000ul;
    ts.tv_nsec = timeout % 1000000ul * 1000;

    pfds.fd = what;
    pfds.events = POLLIN;
    pfds.revents = 0;
    ret = ppoll(&pfds, 1, &ts, NULL);
    if (ret == 0)
        return RX_WAIT_TIMEOUT;
    else if (ret == 1)
    {
        //do the read to release it
        ret = read(what, &buff, sizeof(buff));
        if (ret > 0)
            return RX_WAIT_0;
    }
    return RX_WAIT_ERROR;

}
RX_COMMON_API uint32_t rx_handle_wait_for_multiple(sys_handle_t* what, size_t count, uint32_t timeout)
{
    struct pollfd pfds[0x10];
    int ret;
    size_t i;
    eventfd_t buff = 0;

    if (count > 0x10)
        return RX_WAIT_ERROR;//to large for this function

    for (i = 0; i < count; i++)
    {
        pfds[i].fd = what[i];
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
    }
    ret = poll(pfds, (int)count, (int)timeout);
    if (ret == 0)
        return RX_WAIT_TIMEOUT;
    else if (ret > 0)
    {
        int first = -1;
        //do the read to release it
        for (i = 0; i < count; i++)
        {
            if (pfds[i].revents != 0)
            {
                if (first < 0)
                    first = (int)i;
                ret = read(pfds[i].fd, &buff, sizeof(buff));
                if (ret <= 0)
                    break;
            }
        }
        if (ret > 0)
            return RX_WAIT_0 + first;
    }
    return RX_WAIT_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

#define MANUAL_EVENT

///////////////////////////////////////////////////////////////////////////////////////////////////
// event apstractions ( wait and the rest of the stuff
RX_COMMON_API sys_handle_t rx_event_create(int initialy_set)
{
    int fd = 0;
    fd = eventfd(initialy_set ? 1 : 0, EFD_NONBLOCK);

    return fd;
}
RX_COMMON_API int rx_event_destroy(sys_handle_t hndl)
{
    close(hndl);
    return RX_ERROR;
}
RX_COMMON_API int rx_event_set(sys_handle_t hndl)
{
    eventfd_t val = 0x1;// 0xfffffffe;
    int fd = (int)hndl;
    int ret = write(fd, &val, sizeof(val));
    if (ret < 0)
        return RX_ERROR;
    else
        return RX_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// errors support

RX_COMMON_API rx_os_error_t rx_last_os_error(const char* text, char* buffer, size_t buffer_size)
{
    char buff[0x100];
    char* msg;
    int err = errno;
    msg = strerror_r(err, buff, sizeof(buff));
    if (text)
        snprintf(buffer, buffer_size, "%s. %s (%d)", text, msg, err);
    else
        snprintf(buffer, buffer_size, "%s (%d)", msg, err);
    return err;
}




