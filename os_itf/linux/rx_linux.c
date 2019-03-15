

/****************************************************************************
*
*  os_itf\linux\rx_linux.c
*
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




// rx_linux
#include "os_itf/linux/rx_linux.h"

#include "version/rx_version.h"
#include "os_itf/rx_ositf.h"
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/eventfd.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <poll.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <time.h>
#include <uuid/uuid.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <byteswap.h>


#ifdef __cplusplus
extern "C" {
#endif


size_t g_page_size=0;

int g_base_rt_priority=0;
int g_idle_priority=0;
int g_realtime_priority=0;
int g_priority_add=0;

int rx_big_endian = 0;
rx_thread_data_t rx_tls = 0;
const char* rx_server_name=NULL;

int add_pending_op(linux_epoll_subscriber* internal)
{
    return __atomic_add_fetch(&internal->pending_operations, 1, __ATOMIC_SEQ_CST);
}
int remove_pending_op(linux_epoll_subscriber* internal)
{
    return __atomic_sub_fetch(&internal->pending_operations, 1, __ATOMIC_SEQ_CST);
}


const char* rx_get_server_name()
{
    return rx_server_name;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//

const char* g_ositf_version = "ERROR!!!";
char ver_buffer[0x100];
rx_pid_t rx_pid;
void rx_initialize_os(int rt, rx_thread_data_t tls,const char* server_name)
{
	create_module_version_string(RX_HAL_NAME, RX_HAL_MAJOR_VERSION, RX_HAL_MINOR_VERSION, RX_HAL_BUILD_NUMBER, __DATE__, __TIME__, ver_buffer);
	g_ositf_version = ver_buffer;


    rx_server_name=server_name;
	rx_tls = tls;
	rx_pid= getpid();
	// determine big endian or little endian
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };
	rx_big_endian = (bint.c[0] == 1 ? 1 : 0);

    int ret;
    struct sched_param sp;
    struct rlimit rl;
    // query page size for optimization purpose
    g_page_size = sysconf(_SC_PAGESIZE);

    if(rt)
    {
        // scheduler stuff
        printf("Setting up RT Scheduler\r\n");

        // retrive max priority
        memzero(&rl,sizeof(rl));
        ret=getrlimit(RLIMIT_RTPRIO,&rl);
        if(ret==-1)
            perror("limit");
        else
        {
            g_realtime_priority=sched_get_priority_max(SCHED_RR);
            g_idle_priority=sched_get_priority_min(SCHED_RR);
            g_base_rt_priority=g_realtime_priority/2;
            printf("Max RT priority: %d\n",(int)g_realtime_priority);
            printf("Min RT priority: %d\n",(int)g_idle_priority);
            memzero(&sp,sizeof(sp));
            sp.__sched_priority=g_base_rt_priority;
            ret=sched_setscheduler(0,SCHED_RR,&sp);
            if(ret==-1)
                perror("sched_setscheduler");
            else
            {
                printf("Base priority set to %d\n",g_base_rt_priority);
                g_priority_add=g_base_rt_priority/10;

                printf("RT Scheduler set up successfuly\r\n");
            }

        }
    }
}
void rx_deinitialize_os()
{
}
///////////////////////////////////////////////////////////////////
// errors support pipes
uint32_t rx_get_last_socket_eror()
{
    return errno;
}
///////////////////////////////////////////////////////////////////
// anynoimus pipes
void rx_initialize_server_side_pipe(struct pipe_server_t* pipes)
{
    memzero(pipes,sizeof(struct pipe_server_t));
}
void rx_initialize_client_side_pipe(struct pipe_client_t* pipes)
{
    memzero(pipes,sizeof(struct pipe_server_t));
}
int rx_create_server_side_pipe(struct pipe_server_t* pipes,size_t size)
{
    int pipe1[2];
    int pipe2[2];

    int ret=pipe(pipe1);
    if(ret!=0)
    {// success
        return RX_ERROR;
    }
    ret=pipe(pipe2);
    if(ret!=0)
    {// success
        close(pipe1[0]);
        close(pipe1[1]);
        return RX_ERROR;
    }
    pipes->server_read=pipe1[0];
    pipes->server_write=pipe2[1];
    pipes->client_read=pipe2[0];
    pipes->client_read=pipe1[1];

    return RX_OK;
}
int rx_create_client_side_pipe(struct pipe_server_t* server_pipes, struct pipe_client_t* pipes)
{
    pipes->client_read=server_pipes->server_write;
    pipes->client_write=server_pipes->server_read;
    return RX_OK;
}
int rx_destry_server_side_pipe(struct pipe_server_t* pipes)
{
    if(pipes->server_read)
    {
        close(pipes->server_read);
        pipes->server_read=0;
    }
    if(pipes->server_write)
    {
        close(pipes->server_write);
        pipes->server_write=0;
    }
    if(pipes->client_read)
    {
        close(pipes->client_read);
        pipes->client_read=0;
    }
    if(pipes->client_write)
    {
        close(pipes->client_write);
        pipes->client_write=0;
    }
    return RX_OK;
}
int rx_destry_client_side_pipe(struct pipe_client_t* pipes)
{

    if(pipes->client_read)
    {
        close(pipes->client_read);
        pipes->client_read=0;
    }
    if(pipes->client_write)
    {
        close(pipes->client_write);
        pipes->client_write=0;
    }
    return RX_OK;
}

int rx_write_pipe_server(struct pipe_server_t* pipes, const void* data, size_t size)
{
    return RX_ERROR;
}
int rx_write_pipe_client(struct pipe_client_t* pipes, const void* data, size_t size)
{
    return RX_ERROR;
}
int rx_read_pipe_server(struct pipe_server_t* pipes, void* data, size_t size)
{
    return RX_ERROR;
}
int rx_read_pipe_client(struct pipe_client_t* pipes, void* data, size_t size)
{
    return RX_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////
// IP addresses
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
    return 0;
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

void rx_generate_new_uuid(rx_uuid_t* u)
{
	uuid_t uuid;
	uuid_generate(uuid);
	linux_uuid_to_uuid(&uuid, u);
}
uint32_t rx_uuid_to_string(const rx_uuid_t* u, char* str)
{
	uuid_t uuid;
	uuid_to_linux_uuid(u, &uuid);
	uuid_unparse(uuid, str);
	return RX_OK;
}
uint32_t rx_string_to_uuid(const char* str, rx_uuid_t* u)
{
	uuid_t uuid;
	uuid_parse(str, uuid);
	linux_uuid_to_uuid(&uuid, u);
	return RX_OK;
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
    if (rx_time==0)
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

int rx_os_get_system_time(struct rx_time_struct_t* st)
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
int rx_os_to_local_time(struct rx_time_struct_t* st)
{
    RX_ASSERT(0);
    return RX_ERROR;
}
int rx_os_to_utc_time(struct rx_time_struct_t* st)
{
    RX_ASSERT(0);
    return RX_ERROR;
}
int rx_os_split_time(const struct rx_time_struct_t* st, struct rx_full_time_t* full)
{
    struct tm tm;
    struct timespec tv;
    rx_time_to_timeval(st->t_value,&tv);
    time_t tt=tv.tv_sec;
    gmtime_r(&tt,&tm);
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
int rx_os_collect_time(const struct rx_full_time_t* full, struct rx_time_struct_t* st)
{
    struct timespec tv;
    struct tm tm;
    tm.tm_year=full->year-1900;
    tm.tm_mon=full->month;
    tm.tm_mday=full->day;
    tm.tm_hour=full->hour;
    tm.tm_min=full->minute;
    tm.tm_sec=full->second;
    time_t tt=mktime(&tm);

    tv.tv_nsec=full->milliseconds*1000000ll;
    tv.tv_sec=tt;
    st->t_value=timeval_to_rx_time(&tv);

    return RX_OK;
}


uint8_t pipe_dummy_buffer[0x100];


uint32_t rx_border_rand(uint32_t min, uint32_t max)
{
	return RX_ERROR;
}

void* rx_allocate_os_memory(size_t size)
{
    return mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
}
void rx_deallocate_os_memory(void* p,size_t size)
{
    int ret=munmap(p,size);
    if(ret==-1)
        perror("Unmap");
}

size_t rx_os_page_size()
{
    RX_ASSERT(g_page_size);
    return g_page_size;
}
////////////////////////////////////////////////////////////
// system info classes


void rx_collect_system_info(char* buffer, size_t buffer_size)
{
    struct utsname data;
    uname(&data);
    strcpy(buffer,data.sysname);
    strcat(buffer," Ver:");
    strcat(buffer,data.release);
    strcat(buffer," [");
    strcat(buffer,data.machine);
    strcat(buffer,"]");
}

int lookup(char *line, char *pattern, char **value)
{
    char *p, *v;
    int len = strlen(pattern);

    /* don't re-fill already found tags, first one wins */
    if (!*line || *value)
        return RX_ERROR;

    /* pattern */
    if (strncmp(line, pattern, len))
        return RX_ERROR;

    /* white spaces */
    for (p = line + len; isspace(*p); p++);

    /* separator */
    if (*p != ':')
        return RX_ERROR;

    /* white spaces */
    for (++p; isspace(*p); p++);

    /* value */
    if (!*p)
        return RX_ERROR;
    v = p;

    /* end of value */
    len = strlen(line) - 1;
    for (p = line + len; isspace(*(p-1)); p--);
    *p = '\0';

    *value=(char*)malloc(strlen(v)+1);
    strcpy(*value,v);
    return RX_OK;
}


#define _PATH_PROC_CPUINFO	"/proc/cpuinfo"

void rx_collect_processor_info(char* buffer, size_t buffer_size)
{
    FILE *fp = fopen(_PATH_PROC_CPUINFO,"r");
    char buf[BUFSIZ];
    char* model_name=NULL;
    char* cores=NULL;
    /* details */
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if (lookup(buf, "model name", &model_name)) ;
        else if (lookup(buf, "cpu cores", &cores)) ;
        else
            continue;
    }
    if(model_name!=NULL)
    {
        strcpy(buffer,model_name);
        free(model_name);
    }
    else
        buffer[0]='\0';
    if(cores!=NULL)
    {
        strcat(buffer," ; Total Cores:");
        strcat(buffer,cores);
        free(cores);
    }
}
void read_off_memory_status(size_t* process)
{
	const char* statm_path = "/proc/self/statm";

	FILE *f = fopen(statm_path, "r");

	if (!f) {
		perror(statm_path);
	}
	uint32_t temp;
	if (1 != fscanf(f, "%u" ,
		&temp))
	{
		perror(statm_path);
	}
	*process = temp;
	fclose(f);
}
void rx_collect_memory_info(size_t* total, size_t* free, size_t* process)
{
    struct sysinfo info;
    sysinfo(&info);
	*total = info.totalram*info.mem_unit;
	*free = info.freeram*info.mem_unit;

	read_off_memory_status(process);
}
/////////////////////////////////////////////////////////////////////////////////////////////////


uint32_t rx_handle_wait(sys_handle_t what, uint32_t timeout)
{
    struct pollfd pfds;
    int ret;
	eventfd_t buff = 0;

	pfds.fd=what;
	pfds.events=POLLIN;
	pfds.revents=0;
	ret = poll(&pfds,1,(int)timeout);
	if(ret==0)
        return RX_WAIT_TIMEOUT;
	else if(ret==1)
	{
        //do the read to release it
        read(what, &buff, sizeof(buff));
        return RX_WAIT_0;
    }
	else
        return RX_WAIT_ERROR;

}
uint32_t rx_handle_wait_us(sys_handle_t what, uint64_t timeout)
{
    struct pollfd pfds;
    int ret;
	eventfd_t buff = 0;

    struct timespec ts;
    ts.tv_sec = timeout / 1000000ul;
    ts.tv_nsec = timeout % 1000000ul * 1000;

	pfds.fd=what;
	pfds.events=POLLIN;
	pfds.revents=0;
	sigset_t sigset;
	ret = ppoll(&pfds,1,&ts,&sigset);
	if(ret==0)
        return RX_WAIT_TIMEOUT;
	else if(ret==1)
	{
        //do the read to release it
        read(what, &buff, sizeof(buff));
        return RX_WAIT_0;
    }
	else
        return RX_WAIT_ERROR;

}
uint32_t rx_handle_wait_for_multiple(sys_handle_t* what, size_t count,uint32_t timeout)
{
	struct pollfd pfds[0x10];
	int ret;
	size_t i;
	eventfd_t buff = 0;

	if (count>0x10)
		return RX_WAIT_ERROR;//to large for this function

	for (i = 0; i<count; i++)
	{
		pfds[i].fd = what[i];
		pfds[i].events = POLLIN;
		pfds[i].revents = 0;
	}
	ret = poll(pfds, (int)count, (int)timeout);
	if (ret == 0)
		return RX_WAIT_TIMEOUT;
	else if (ret>0)
	{
		int first = -1;
		//do the read to release it
		for (i = 0; i<count; i++)
		{
			if (pfds[i].revents != 0)
			{
				if (first<0)
					first = (int)i;
				read(pfds[i].fd, &buff, sizeof(buff));
			}
		}
		return RX_WAIT_0 + first;
	}
	else
		return RX_WAIT_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// mutex apstractions ( wait and the rest of the stuff
sys_handle_t rx_mutex_create(int initialy_owned)
{
	int fd = 0;
	fd = eventfd(initialy_owned == 0 ? 1 : 0, EFD_NONBLOCK);

	return fd;
}
int rx_mutex_destroy(sys_handle_t hndl)
{
	close(hndl);
	return RX_ERROR;
}
int rx_mutex_aquire(sys_handle_t hndl, uint32_t timeout)
{
    rx_handle_wait(hndl,timeout);
	return RX_ERROR;
}
int rx_mutex_release(sys_handle_t hndl)
{

	eventfd_t val = 1;
	int fd = (int)hndl;
	write(fd, &val, sizeof(val));
	return RX_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MANUAL_EVENT

///////////////////////////////////////////////////////////////////////////////////////////////////
// event apstractions ( wait and the rest of the stuff
sys_handle_t rx_event_create(int initialy_set)
{
	int fd = 0;
	fd = eventfd(initialy_set ? 1 : 0, EFD_NONBLOCK);

	return fd;
}
int rx_event_destroy(sys_handle_t hndl)
{
	close(hndl);
	return RX_ERROR;
}
int rx_event_set(sys_handle_t hndl)
{
	eventfd_t val = 0xfffffffe;
	int fd = (int)hndl;
	write(fd, &val, sizeof(val));
	return RX_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////



void rx_slim_lock_create(pslim_lock_t plock)
{
	pthread_mutex_t* mtx = (pthread_mutex_t*)plock;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(mtx, &attr);
}
void rx_slim_lock_destroy(pslim_lock_t plock)
{
	pthread_mutex_t* mtx = (pthread_mutex_t*)plock;
	pthread_mutex_destroy(mtx);
}
void rx_slim_lock_aquire(pslim_lock_t plock)
{
	pthread_mutex_t* mtx = (pthread_mutex_t*)plock;
	pthread_mutex_lock(mtx);
}
void rx_slim_lock_release(pslim_lock_t plock)
{
	pthread_mutex_t* mtx = (pthread_mutex_t*)plock;
	pthread_mutex_unlock(mtx);
}

void rx_rw_slim_lock_create(prw_slim_lock_t plock)
{
	rx_slim_lock_create((pslim_lock_t)plock);
}
void rx_rw_slim_lock_destroy(prw_slim_lock_t plock)
{
	rx_slim_lock_destroy((pslim_lock_t)plock);
}
void rx_rw_slim_lock_aquire_reader(prw_slim_lock_t plock)
{
	rx_slim_lock_aquire((pslim_lock_t)plock);
}
void rx_rw_slim_lock_release_reader(prw_slim_lock_t plock)
{
	rx_slim_lock_release((pslim_lock_t)plock);
}
void rx_rw_slim_lock_aquire_writter(prw_slim_lock_t plock)
{
	rx_slim_lock_aquire((pslim_lock_t)plock);
}
void rx_rw_slim_lock_release_writter(prw_slim_lock_t plock)
{
	rx_slim_lock_release((pslim_lock_t)plock);
}
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// thread apstractions


////////////////////////////////////////////////////////////////////////////////////////////////
// Linux system thread VERY platform specific not to be used outside
typedef void(*start_address_t)(void*);
typedef struct linux_thread_chunk
{
	int thread_fd;
	start_address_t f;
	void* arg;
	int priority;
	pthread_t thr;
	pthread_attr_t attr;
} *plinux_thread_chunk;

void* do_stuff(void* arg)
{
	struct linux_thread_chunk* start_chunk = (struct linux_thread_chunk*)arg;
	eventfd_t buff = 1;
	start_address_t addr = start_chunk->f;

	(addr)(start_chunk->arg);

	//(*start_chunk-f)(start_chunk->arg);
	//start_chunk.f(NULL);
	//(start_chunk->f)(start_chunk->arg);

	write(start_chunk->thread_fd, &buff, sizeof(buff));
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////
sys_handle_t rx_thread_create(start_address_t f, void* arg, int priority, uint32_t* thread_id)
{
	struct sched_param sp;
	struct linux_thread_chunk* chunk = (struct linux_thread_chunk*)malloc(sizeof(struct linux_thread_chunk));
	chunk->f = f;
	chunk->thread_fd = eventfd(0, EFD_NONBLOCK);
	chunk->arg=arg;
	chunk->priority=priority;
	pthread_attr_init(&chunk->attr);


	memzero(&sp,sizeof(sp));
	sp.__sched_priority = g_base_rt_priority;

	switch(priority)
	{
        case RX_PRIORITY_IDLE:
            sp.__sched_priority=g_idle_priority;
            break;
        case RX_PRIORITY_LOW:
            sp.__sched_priority=g_base_rt_priority-2*g_priority_add;
            break;
        case RX_PRIORITY_BELOW_NORMAL:
            sp.__sched_priority=g_base_rt_priority-g_priority_add;
            break;
        case RX_PRIORITY_ABOVE_NORMAL:
            sp.__sched_priority=g_base_rt_priority+g_priority_add;
            break;
        case RX_PRIORITY_HIGH:
            sp.__sched_priority=g_base_rt_priority+2*g_priority_add;
            break;
        case RX_PRIORITY_REALTIME:
            sp.__sched_priority=g_realtime_priority;
            break;
	}

	pthread_create(&chunk->thr, &chunk->attr, do_stuff, chunk);

    int ret = pthread_setschedparam(chunk->thr,SCHED_RR,&sp);
    if(ret==-1)
    {
        perror("pthread_setschedparam");
    }

	return chunk->thread_fd;
}
int rx_thread_join(sys_handle_t what)
{
	eventfd_t buff = 0;
	read(what, &buff, sizeof(buff));
	return RX_ERROR;
}
int rx_thread_close(sys_handle_t what)
{
	// warning this here leaves memory lekage
	// not so many threads created or stopped to have problem yet, but see what to do with it
	close(what);
	return RX_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// basic apstractions
void rx_msleep(uint32_t timeout)
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
void rx_us_sleep(uint64_t timeout)
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
uint32_t rx_get_tick_count()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t ret = ts.tv_sec * 1000;
	ret = ret + ts.tv_nsec / 1000000ul;
	return (uint32_t)ret;
}
uint64_t rx_get_us_ticks()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t ret = ts.tv_sec * 1000000ul;
	ret = ret + ts.tv_nsec / 1000ul;
	return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////
// file handling functions
int rx_create_directory(const char* path, int fail_on_exsist)
{
	int ret = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (ret != 0)
	{
		if (errno == EEXIST && !fail_on_exsist)
			return RX_OK;
		else
			return RX_ERROR;
	}
	return RX_OK;
}

sys_handle_t rx_file(const char* path, int access, int creation)
{
	int ret;
	int flags = 0;

	switch (access)
	{
	case RX_FILE_OPEN_READ:
		flags = O_RDONLY;
		break;
	case RX_FILE_OPEN_WRITE:
		flags = O_WRONLY;
		break;
	case RX_FILE_OPEN_BOTH:
		flags = O_RDWR;
		break;
	}

	switch (creation)
	{
	case RX_FILE_CREATE_ALWAYS:
		flags |= O_CREAT;
		break;
	case RX_FILE_CREATE_NEW:
		flags = (O_EXCL | O_CREAT);
		break;
	case RX_FILE_OPEN_ALWAYS:
		flags |= O_CREAT;
		break;
	case RX_FILE_OPEN_EXISTING:
		break;
	}

	int mode = S_IRWXU | S_IRWXG | S_IROTH;

	ret = open(path, flags, mode);
	if (ret == -1)
	{
		return 0;
	}
	else
		return ret;
}
int rx_file_read(sys_handle_t hndl, void* buffer, uint32_t size, uint32_t* readed)
{
	int ret = read(hndl, buffer, size);
	if (ret >= 0)
	{
		if (readed)
			*readed = (uint32_t)ret;
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}
int rx_file_write(sys_handle_t hndl, const void* buffer, uint32_t size, uint32_t* written)
{
	int ret = write(hndl, buffer, size);
	if (ret >= 0)
	{
		if (written)
			*written = (uint32_t)ret;
		return RX_OK;
	}
	else
	{
		return RX_ERROR;
	}
}
int rx_file_close(sys_handle_t hndl)
{
	if (close(hndl) == -1)
	{
		return RX_ERROR;
	}
	else
		return RX_OK;
}

int rx_file_delete(const char* path)
{
	if (remove(path) == -1)
	{
		return RX_ERROR;
	}
	else
		return RX_OK;
}

int rx_file_rename(const char* old_path, const char* new_path)
{
	return RX_ERROR;
}


int rx_file_exsist(const char* path)
{
    return access(path,0x7fffffff)==0;
}

int rx_file_get_size(sys_handle_t hndl, uint64_t* size)
{
	struct stat data;
	int ret = fstat(hndl, &data);
	if (ret == -1)
	{
		return RX_ERROR;
	}
	else
	{
		*size = data.st_size;
		return RX_OK;
	}
}
int rx_file_get_time(sys_handle_t hndl, struct rx_time_struct_t* tm)
{
	struct stat data;
	//struct timeval tv;
	int ret = fstat(hndl, &data);
	if (ret == -1)
	{
		return RX_ERROR;
	}
	else
	{
		/*tv.tv_usec = 0;
		tv.tv_sec = data.st_mtime;
		tm->t_value = timeval_to_rx_time(&tv);*/
		rx_os_get_system_time(tm);
		return RX_OK;
	}
}


//directories stuff
/*typedef struct rx_file_directory_entry_t
{
int is_directory;
char file_name[MAX_PATH];
uint32_t size;
rx_time_struct time;
} rx_file_directory_entry;*/

// temp fuction filling drectory entry
int fill_dir_entry(struct rx_file_directory_entry_t* entry, struct dirent* pentry)
{
	int ret = 0;
	strcpy(entry->file_name, pentry->d_name);
	if (pentry->d_type == DT_DIR)
	{
		entry->is_directory = 1;
		entry->size = 0;
		ret = 1;
	}
	else if (pentry->d_type == DT_REG)
	{
		entry->is_directory = 0;
		entry->size = 0;
		ret = 1;
	}
	return ret;
}

// returns NULL if not succeeded
find_file_handle_t rx_open_find_file_list(const char* ipath, struct rx_file_directory_entry_t* entry)
{
	struct linux_find_files_t* ff = NULL;
	char path_revised[MAX_PATH];
	char path[MAX_PATH];
	char* found;
	char* filter;
	DIR* dir;

	strcpy(path, "/");
	strcat(path, ipath);

	dir = opendir(path);
	if (dir)
	{
		do
		{
			struct dirent* pentry = readdir(dir);
			if (pentry)
			{
				if (fill_dir_entry(entry, pentry))
				{
                    ff = (struct linux_find_files_t*)malloc(sizeof(struct linux_find_files_t));
                    ff->pdir = dir;
                    ff->filter = NULL;
					break;// found file or directory
                }
			}
			else
			{
				break;// error or end
			}
		} while (1);
	}
	else
	{// mybe just filter that exsist here
		strcpy(path_revised, path);
		found = &path_revised[strlen(path_revised)];
		while (*found != '/' && found>path_revised)
		{
			found--;
		}
		if (found != path_revised)
		{
			*found = '\0';
			filter = found + 1;
			dir = opendir(path_revised);
			if (dir)
			{
				do
				{
					struct dirent* pentry = readdir(dir);
					if (pentry)
					{
						while (pentry != NULL && !match_pattern(pentry->d_name, filter, 1))
							pentry = readdir(dir);
						if (pentry)
						{
							if (fill_dir_entry(entry, pentry))
							{
                                ff = (struct linux_find_files_t*)malloc(sizeof(struct linux_find_files_t));
                                ff->pdir = dir;
                                ff->filter = (char*)malloc(strlen(filter) + 1);
                                strcpy(ff->filter, filter);
								break;// found file or directory
                            }
						}
					}
					else
					{
						break;// error or end
					}
				} while (1);

			}
		}
	}
	if (dir == NULL)
		return NULL;
	else
	{
        if(ff==NULL)
            closedir(dir);
		return ff;
    }
}
// while not zero files are fethecd
int rx_get_next_file(find_file_handle_t hndl, struct rx_file_directory_entry_t* entry)
{
	struct linux_find_files_t* ff = (struct linux_find_files_t*)hndl;
	int ret = 0;
	do
	{
		struct dirent* pentry = readdir(ff->pdir);
		if (pentry)
		{
			if (ff->filter)
			{
				while (pentry != NULL && !match_pattern(pentry->d_name, ff->filter, 1))
					pentry = readdir(ff->pdir);
				if (pentry == NULL)
					break;
			}
			if (fill_dir_entry(entry, pentry))
			{
				ret = 1;
				break;// found file or directory
			}
		}
		else
		{
			break;// error or end
		}
	} while (1);

	return ret;
}
void rx_find_file_close(find_file_handle_t hndl)
{
	struct linux_find_files_t* ff = (struct linux_find_files_t*)hndl;
	closedir(ff->pdir);
	free(hndl);
}
/////////////////////////////////////////////////////////////////////////////////////////////


typedef struct dispatch_function_data_t
{
    rx_callback f;
    void* arg;
} dispatch_function_data;



rx_kernel_dispather_t rx_create_kernel_dispathcer(int max)
{
    struct epoll_event event;
    struct linux_epoll_struct_t* pepoll=(struct linux_epoll_struct_t*)malloc(sizeof(struct linux_epoll_struct_t));
    int ret=pipe2(pepoll->pipe,O_NONBLOCK);
    if(ret==0)
    {// success
        pepoll->epoll_fd=epoll_create(max);
        if(pepoll->epoll_fd)
        {
            event.data.ptr=NULL;
            event.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
            ret=epoll_ctl(pepoll->epoll_fd,EPOLL_CTL_ADD,pepoll->pipe[0],&event);
            if(ret==0)
            {
                return pepoll;
            }
        }
    }
    return NULL;
}
uint32_t rx_socket_read(struct rx_io_register_data_t* what, size_t* readed)
{
	struct linux_epoll_subscriber_t* internal = (struct linux_epoll_subscriber_t*)what->internal;
	internal->read_type = EPOLL_READ_TYPE;


    add_pending_op(internal);

	int ret = read(what->handle, what->read_buffer, what->read_buffer_size);
	if (ret == -1)
	{
		int err = errno;
		if (err == EAGAIN || err == EWOULDBLOCK)
		{
			*readed = 0;
			return RX_ASYNC;
		}
		else
		{
            remove_pending_op(internal);
			return RX_ERROR;
        }
	}
	else// performed in sync
	{
        remove_pending_op(internal);
		*readed = (size_t)ret;
		internal->read_type = 0;
	}

	return RX_OK;
}
uint32_t rx_socket_write(struct rx_io_register_data_t* what, const void* data, size_t count)
{
	struct linux_epoll_subscriber_t* internal = (struct linux_epoll_subscriber_t*)what->internal;
	internal->write_buffer = data;
	internal->left_to_write = count;
	internal->write_type = EPOLL_WRITE_TYPE;

    add_pending_op(internal);

	int ret = write(what->handle, data, count);
	if (ret == -1)
	{
		int err = errno;
		if (err == EAGAIN || err == EWOULDBLOCK)
		{

            struct linux_epoll_subscriber_t* type_data=(struct linux_epoll_subscriber_t*)what->internal;
            struct epoll_event event;
            event.data.ptr=what;
            event.events=EPOLLOUT|EPOLLET|EPOLLONESHOT;
            ret=epoll_ctl(type_data->epoll_handle,EPOLL_CTL_ADD,type_data->write_handle,&event);
			if(ret==0)
			{
                return RX_ASYNC;
            }
            else
            {
                remove_pending_op(internal);
                return RX_ERROR;
            }
		}
		else
		{
            remove_pending_op(internal);
			return RX_ERROR;
		}
	}
	else// performed in sync
	{
        remove_pending_op(internal);
		// check to see if we written all
		size_t written = (size_t)ret;
		if (written<count)
		{// din't write all do it again

			uint8_t* temp = (uint8_t*)data;
			temp += written;
			return rx_socket_write(what, temp, count - written);
		}
		else
		{
			internal->write_buffer = NULL;
			internal->write_type = 0;
		}
	}
	return RX_OK;
}


uint32_t rx_socket_accept(struct rx_io_register_data_t* what)
{
    struct sockaddr* addr=(struct sockaddr*)what->read_buffer;
    socklen_t addrsize=sizeof(struct sockaddr_in);
    struct linux_epoll_subscriber_t* internal=(struct linux_epoll_subscriber_t*)what->internal;
    internal->read_type=EPOLL_ACCEPT_TYPE;
    if(what->handle<=0)
        return RX_ERROR;// allready closed

    add_pending_op(internal);

    int ret=accept4(what->handle,addr,&addrsize,SOCK_NONBLOCK);
    if(ret==-1)
    {
        int err=errno;
        if(err==EAGAIN || err==EWOULDBLOCK)
        {
            return RX_OK;
        }
        else
        {
            remove_pending_op(internal);
            return RX_ERROR;
        }
    }
    else// performed in sync
    {

        remove_pending_op(internal);
        struct sockaddr_in local_addr;
        socklen_t addr_size=sizeof(local_addr);
        getsockname(ret,(struct sockaddr*)&local_addr,&addr_size);
        (what->accept_callback)(what->data,0,ret,addr,(struct sockaddr*)&local_addr,addrsize);
    }

    return RX_OK;
}
uint32_t rx_socket_connect(struct rx_io_register_data_t* what, struct sockaddr* addr, size_t addrsize)
{
    int ret;
    struct linux_epoll_subscriber_t* internal=(struct linux_epoll_subscriber_t*)what->internal;
	internal->write_type = EPOLL_CONNECT_TYPE;
    add_pending_op(internal);
    ret=connect(what->handle,addr,addrsize);
    if(ret==-1)
    {
        int err=errno;
        if(err==EAGAIN || err==EWOULDBLOCK || err==EINPROGRESS)
        {
            return RX_ASYNC;
        }
        else
        {
            remove_pending_op(internal);
            return RX_ERROR;
        }
    }
    else// performed in sync
    {
        remove_pending_op(internal);
        (what->connect_callback)(what->data,0);
    }

    return RX_OK;
}

uint32_t rx_dispatcher_register(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data)
{
    struct epoll_event event;
    int ret;
    event.data.ptr=data;
    event.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
    ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_ADD,data->handle,&event);
    if(ret==0)
    {
        struct linux_epoll_subscriber_t* type_data=(struct linux_epoll_subscriber_t*)data->internal;
        type_data->write_handle=dup(data->handle);
        type_data->epoll_handle=disp->epoll_fd;
        event.events=0;
        ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_ADD,type_data->write_handle,&event);
        if(ret==0)
        {
            return RX_OK;
        }
        else
            return RX_ERROR;
    }
    else
        return RX_ERROR;
}

int rx_dispatcher_unregister(rx_kernel_dispather_t disp, struct rx_io_register_data_t* data)
{
    struct epoll_event event;
    event.data.ptr=data;
    event.events=0;
    int ret;
    ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_DEL,data->handle,&event);
    if(ret==0)
    {
        struct linux_epoll_subscriber_t* type_data=(struct linux_epoll_subscriber_t*)data->internal;
        ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_DEL,type_data->write_handle,&event);
        if(ret==0)
        {
            // here we take all operations that are not yet callbacked
            //  subsract 1000 to ensure that no one is fired after this code
            int ops=__atomic_fetch_sub(&type_data->pending_operations, 10000, __ATOMIC_SEQ_CST);
            //printf("####Still pending: %d\r\n",ops);
            close(type_data->write_handle);
            return ops;
        }
        else
            return -1;
    }
    else
        return -1;
}
uint32_t rx_destroy_kernel_dispatcher(rx_kernel_dispather_t disp)
{
    close(disp->pipe[0]);
    close(disp->pipe[1]);
    close(disp->epoll_fd);
    free(disp);
    return RX_ERROR;
}
uint32_t rx_dispatcher_signal_end(rx_kernel_dispather_t disp)
{
    return rx_dispatch_function(disp,NULL,NULL);
}

uint32_t rx_dispatch_events(rx_kernel_dispather_t disp)
{
	int ndfs;
	struct epoll_event one;
	one.events = EPOLLIN | EPOLLOUT;
	ndfs = epoll_wait(disp->epoll_fd, &one, 1, -1);
	if (ndfs<0)
	{
		return 1;// error
    }
	else
	{
		if (ndfs == 1)
		{
			if (one.data.ptr == NULL)
			{//function post
				struct dispatch_function_data_t data;
				int ret = read(disp->pipe[0], &data, sizeof(data));

				one.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                epoll_ctl(disp->epoll_fd,EPOLL_CTL_MOD,disp->pipe[0],&one);

				if (ret>0)
				{
					if (data.f)
						(data.f)(data.arg);
					else
						return 0;// exit the loop
				}
			}
			else
			{
                struct sockaddr_in local_addr;
				struct linux_epoll_subscriber_t* type_data;
				struct rx_io_register_data_t* io_data;
				io_data = (struct rx_io_register_data_t*)one.data.ptr;
				type_data = (struct linux_epoll_subscriber_t*)io_data->internal;


                if(one.events&EPOLLERR || one.events&EPOLLHUP)
                {
                    int ops=remove_pending_op(type_data);
                    if(ops>=0)
                        (io_data->shutdown_callback)(io_data->data, errno);
                }
                else
                {
                    if (one.events&EPOLLIN)
                    {// reading on handle
                        switch (type_data->read_type)
                        {
                        case EPOLL_READ_TYPE:
                            {
                                type_data->read_type = 0;
                                int err = read(io_data->handle, io_data->read_buffer, io_data->read_buffer_size);
                                if (err == -1)
                                {
                                    if (err != EAGAIN && err != EWOULDBLOCK)
                                    {
                                        int ops=remove_pending_op(type_data);
                                        if(ops>=0)
                                            (io_data->shutdown_callback)(io_data->data, errno);
                                    }
                                    // nothing to do here, he will call us again any way
                                }
                                else
                                {
                                    int ops=remove_pending_op(type_data);
                                    if(ops>=0)
                                    {
                                        if(err==0)// closed up
                                            (io_data->shutdown_callback)(io_data->data, errno);
                                        else
                                            (io_data->read_callback)(io_data->data, 0, err);
                                    }
                                }

                                one.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                                epoll_ctl(disp->epoll_fd,EPOLL_CTL_MOD,io_data->handle,&one);
                            }
                            break;
                        case EPOLL_ACCEPT_TYPE:
                            {
                                type_data->read_type = 0;
                                socklen_t addr_size = io_data->read_buffer_size;
                                int err = accept4(io_data->handle, (struct sockaddr*)io_data->read_buffer, &addr_size, SOCK_NONBLOCK);
                                if (err == -1)
                                {
                                    if (err != EAGAIN && err != EWOULDBLOCK)
                                    {
                                        int ops=remove_pending_op(type_data);
                                        if(ops>=0)
                                            (io_data->shutdown_callback)(io_data->data, errno);
                                    }
                                    // nothing to do here, he will call us again any way
                                }
                                else
                                {
                                    int ops=remove_pending_op(type_data);
                                    if(ops>=0)
                                    {
                                        socklen_t addr_size=sizeof(local_addr);
                                        getsockname(err,(struct sockaddr*)&local_addr,&addr_size);
										(io_data->accept_callback)(io_data->data, 0, err, io_data->read_buffer, (struct sockaddr*)&local_addr, addr_size);
                                    }
                                }

                                one.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                                epoll_ctl(disp->epoll_fd,EPOLL_CTL_MOD,io_data->handle,&one);
                            }
                            break;
                        }
                    }
                    if (one.events&EPOLLOUT)
                    {// writing on handle
                        printf("out\r\n");
                        switch (type_data->write_type)
                        {
                        case EPOLL_WRITE_TYPE:
                            {
                                if (type_data->write_buffer)
                                {
                                    int rt = write(io_data->handle, type_data->write_buffer, type_data->left_to_write);
                                    if (rt == -1)
                                    {
                                        int err = errno;
                                        if (err != EAGAIN && err != EWOULDBLOCK)
                                        {// error occured do the shutdown
                                            int ops=remove_pending_op(type_data);
                                            if(ops>=0)
                                                (io_data->shutdown_callback)(io_data->data, err);
                                        }
                                        // nothing to do here, he will call us again any way
                                    }
                                    else
                                    {
                                        size_t written = (size_t)rt;
                                        if (written<type_data->left_to_write)
                                        {// din't write all do it again
                                            uint8_t* temp = (uint8_t*)type_data->write_buffer;
                                            temp += written;
                                            type_data->write_buffer = temp;
                                            type_data->left_to_write = type_data->left_to_write - written;
                                            rt = write(io_data->handle, type_data->write_buffer, type_data->left_to_write);
                                            if (rt == -1)
                                            {
                                                int err = errno;
                                                if (err != EAGAIN && err != EWOULDBLOCK)
                                                {// error occured do the shutdown
                                                    int ops=remove_pending_op(type_data);
                                                    if(ops>=0)
                                                        (io_data->shutdown_callback)(io_data->data, err);
                                                }
                                                // nothing to do here, he will call us again any way
                                            }
                                        }
                                        else
                                        {
                                            type_data->write_type = 0;
                                            type_data->write_buffer = NULL;
                                            int ops=remove_pending_op(type_data);
                                            if(ops>=0)
                                                (io_data->write_callback)(io_data->data, 0);
                                        }
                                    }
                                }
                                else
                                {
                                }

                            }
                            break;
                        case EPOLL_CONNECT_TYPE:
                            {
                                type_data->write_type = 0;
                                socklen_t addr_size = io_data->read_buffer_size;
                                int err = accept4(io_data->handle, (struct sockaddr*)io_data->read_buffer, &addr_size, SOCK_NONBLOCK);
                                if (err == -1)
                                {
                                    if (err != EAGAIN && err != EWOULDBLOCK)
                                    {
                                        int ops=remove_pending_op(type_data);
                                        if(ops>=0)
                                            (io_data->shutdown_callback)(io_data->data, errno);
                                    }
                                    // nothing to do here, he will call us again any way
                                }
                                else
                                {
                                    int ops=remove_pending_op(type_data);
                                    if(ops>=0)
                                    {
                                        socklen_t addr_size=sizeof(local_addr);
                                        getsockname(err,(struct sockaddr*)&local_addr,&addr_size);
                                        (io_data->accept_callback)(io_data->data, 0, err, io_data->read_buffer,(struct sockaddr*)&local_addr, addr_size);
                                    }
                                }

                                one.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                                epoll_ctl(disp->epoll_fd,EPOLL_CTL_MOD,io_data->handle,&one);
                            }
                            break;
                        case 0:
                            {
                            }
                            break;
                        }
                    }
				}
			}
		}
	}
	return 1;// do the loop again
}

uint32_t rx_dispatch_function(rx_kernel_dispather_t disp, rx_callback f, void* arg)
{
    struct dispatch_function_data_t data;
    data.arg=arg;
    data.f=f;
    int ret=write(disp->pipe[1],&data,sizeof(data));
    if(ret==sizeof(data))
        return RX_OK;
    else
        return RX_ERROR;
}

sys_handle_t rx_create_and_bind_ip4_tcp_socket(struct sockaddr_in* addr)
{
    sys_handle_t ret=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
    if(ret)
    {

        int on = 1;
        setsockopt(ret, SOL_TCP, TCP_NODELAY, &on, sizeof(on));

        on=1;
        setsockopt(ret,SOL_SOCKET,SO_KEEPALIVE,&on,sizeof(on));

        if(addr->sin_port!=0)
        {//this is listen or udp server socket mark resuse
            on=1;
            setsockopt(ret,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
        }

        addr->sin_family=AF_INET;

        if(0==bind(ret,(const struct sockaddr*)addr,sizeof(struct sockaddr_in)))
        {
            return ret;
        }
    }
    if(ret)
        close(ret);
    return RX_ERROR;
}
uint32_t rx_socket_listen(sys_handle_t handle)
{
    return (0==listen(handle,SOMAXCONN));
}
void rx_close_socket(sys_handle_t handle)
{
    shutdown(handle,SHUT_RDWR);
    close(handle);
}


uint16_t rx_swap_2bytes(uint16_t val)
{
	return bswap_16(val);
}
uint32_t rx_swap_4bytes(uint32_t val)
{
	return bswap_32(val);
}
uint64_t rx_swap_8bytes(uint64_t val)
{
	return bswap_64(val);
}

uint32_t rx_atomic_add_fetch_32(volatile uint32_t* val, int add)
{
	return (uint32_t)__atomic_add_fetch((volatile int*)val, add, __ATOMIC_SEQ_CST);
}
uint32_t rx_atomic_inc_fetch_32(volatile uint32_t* val)
{
	return (uint32_t)__atomic_add_fetch((volatile int*)val, 1, __ATOMIC_SEQ_CST);
}
uint32_t rx_atomic_dec_fetch_32(volatile uint32_t* val)
{
	return (uint32_t)__atomic_add_fetch((volatile int*)val, -1, __ATOMIC_SEQ_CST);
}
uint32_t rx_atomic_fetch_32(volatile uint32_t* val)
{
	return (uint32_t)__atomic_add_fetch((volatile int*)val, 0, __ATOMIC_SEQ_CST);
}

uint64_t rx_atomic_inc_fetch_64(volatile uint64_t* val)
{
	return (uint64_t)__atomic_add_fetch((volatile int64_t*)val, 1ll, __ATOMIC_SEQ_CST);
}
uint64_t rx_atomic_dec_fetch_64(volatile uint64_t* val)
{
	return (uint64_t)__atomic_add_fetch((volatile int64_t*)val, -1ll, __ATOMIC_SEQ_CST);
}
uint64_t rx_atomic_fetch_64(volatile uint64_t* val)
{
	return (uint64_t)__atomic_add_fetch((volatile int64_t*)val, 0ll, __ATOMIC_SEQ_CST);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TLS code
rx_thread_data_t rx_alloc_thread_data()
{
    rx_thread_data_t ret_key;
    int ret=pthread_key_create(&ret_key,NULL);
    if(ret!=0)
    {
        return 0;
    }
    return ret_key;
}
void rx_set_thread_data(rx_thread_data_t key, void* data)
{
    int ret=pthread_setspecific(key,data);
    if(ret!=0)
    {
        RX_ASSERT(0);
    }
}
void* rx_get_thread_data(rx_thread_data_t key)
{
    return pthread_getspecific(key);
}
void rx_free_thread_data(rx_thread_data_t key)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif





