

/****************************************************************************
*
*  os_itf\linux\rx_linux.c
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



int g_base_rt_priority=0;
int g_idle_priority=0;
int g_realtime_priority=0;
int g_priority_add=0;
int g_rt = 0;
int rx_big_endian = 0;
rx_thread_data_t rx_tls = 0;


const char* rx_server_name = NULL;
char node_name_buff[0x100];
void collect_computer_name()
{
    gethostname(node_name_buff, sizeof(node_name_buff));
    rx_server_name = node_name_buff;
}
const char* rx_get_node_name()
{
    if(rx_server_name==NULL)
        collect_computer_name();
    return rx_server_name;
}

int add_pending_op(linux_epoll_subscriber* internal)
{
    return __atomic_add_fetch(&internal->pending_operations, 1, __ATOMIC_SEQ_CST);
}
int remove_pending_op(linux_epoll_subscriber* internal)
{
    return __atomic_sub_fetch(&internal->pending_operations, 1, __ATOMIC_SEQ_CST);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//

const char* g_ositf_version = "ERROR!!!";
char ver_buffer[0x100];
rx_pid_t rx_pid;
int rx_hd_timer = 1;

void rx_init_hal_version()
{
	create_module_version_string(RX_HAL_NAME, RX_HAL_MAJOR_VERSION, RX_HAL_MINOR_VERSION, RX_HAL_BUILD_NUMBER, __DATE__, __TIME__, ver_buffer);
	g_ositf_version = ver_buffer;
}

int init_common_result = RX_ERROR;

void rx_initialize_os(int rt, int hdt, rx_thread_data_t tls, int is_debug)
{
    rx_platform_init_data common_data;
    common_data.rx_hd_timer = hdt;
    common_data.is_debug = is_debug;

    init_common_result = rx_init_common_library(&common_data);

	create_module_version_string(RX_HAL_NAME, RX_HAL_MAJOR_VERSION, RX_HAL_MINOR_VERSION, RX_HAL_BUILD_NUMBER, __DATE__, __TIME__, ver_buffer);
	g_ositf_version = ver_buffer;

	collect_computer_name();

	rx_tls = tls;
	rx_hd_timer = hdt;
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

    if(rt)
    {
        g_rt = 1;
        // scheduler stuff
        printf("Setting up RT Scheduler\r\n");

        // retrive max priority
        memzero(&rl,sizeof(rl));
        ret=getrlimit(RLIMIT_RTPRIO,&rl);
        if(ret==-1)
            perror("limit");
        else
        {
            g_realtime_priority=sched_get_priority_max(SCHED_FIFO);
            g_idle_priority=sched_get_priority_min(SCHED_FIFO);
            g_base_rt_priority=g_realtime_priority/2;
            printf("Max RT priority: %d\n",(int)g_realtime_priority);
            printf("Min RT priority: %d\n",(int)g_idle_priority);
            memzero(&sp,sizeof(sp));
            sp.__sched_priority=g_base_rt_priority;
            ret=sched_setscheduler(0,SCHED_FIFO,&sp);
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
    if (init_common_result)
        rx_deinit_common_library();
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
    pipes->client_write=pipe1[1];

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
    int ret = write(pipes->server_write, data, size);
    if(ret<0 || ret!=(int)size)
    {
        return RX_ERROR;
    }
    else
    {
        return RX_OK;
    }
}
int rx_write_pipe_client(struct pipe_client_t* pipes, const void* data, size_t size)
{
    int ret = write(pipes->client_write, data, size);
    if(ret<0 || ret!=(int)size)
    {
        return RX_ERROR;
    }
    else
    {
        return RX_OK;
    }
}
int rx_read_pipe_server(struct pipe_server_t* pipes, void* data, size_t* size)
{
    int ret = read(pipes->server_write, data, *size);
    if(ret<0)
    {
        return RX_ERROR;
    }
    else
    {
        *size=(size_t)ret;
        return RX_OK;
    }
}
int rx_read_pipe_client(struct pipe_client_t* pipes, void* data, size_t* size)
{
    int ret = read(pipes->client_read, data, *size);
    if(ret<0)
    {
        return RX_ERROR;
    }
    else
    {
        *size=(size_t)ret;
        return RX_OK;
    }
}


uint8_t pipe_dummy_buffer[0x100];


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

void rx_collect_processor_info(char* buffer, size_t buffer_size, size_t* count)
{
    FILE *fp = fopen(_PATH_PROC_CPUINFO,"r");
    char buf[BUFSIZ];
    char* model_name=NULL;
    /* details */
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if (lookup(buf, "model name", &model_name)) ;
        //else if (lookup(buf, "cpu cores", &cores)) ;
        else
            continue;
    }
    if(model_name!=NULL)
    {
        strcpy(buffer,model_name);
        free(model_name);
    }
    else
    {
        buffer[0]='\0';
    }
    *count = (size_t)get_nprocs_conf();
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
    int ret;
	struct linux_thread_chunk* start_chunk = (struct linux_thread_chunk*)arg;
	eventfd_t buff = 1;
	start_address_t addr = start_chunk->f;


	(addr)(start_chunk->arg);

	//(*start_chunk-f)(start_chunk->arg);
	//start_chunk.f(NULL);
	//(start_chunk->f)(start_chunk->arg);

	ret = write(start_chunk->thread_fd, &buff, sizeof(buff));
	if(ret<0)
        perror("Error in do stuff - write.");
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////
sys_handle_t rx_thread_create(start_address_t f, void* arg, int priority, uint32_t* thread_id, const char* name)
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
    int policy = SCHED_RR;
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
            if (g_rt)
                policy = SCHED_FIFO;
            break;
        case RX_PRIORITY_ABOVE_NORMAL:
            sp.__sched_priority=g_base_rt_priority+g_priority_add;
            if (g_rt)
                policy = SCHED_FIFO;
            break;
        case RX_PRIORITY_HIGH:
            sp.__sched_priority=g_base_rt_priority+2*g_priority_add;
            if (g_rt)
                policy = SCHED_FIFO;
            break;
        case RX_PRIORITY_REALTIME:
            sp.__sched_priority=g_realtime_priority;
            break;
	}

	pthread_create(&chunk->thr, &chunk->attr, do_stuff, chunk);
    *thread_id = chunk->thr;


	pthread_setname_np(chunk->thr, name);

    int ret = pthread_setschedparam(chunk->thr, policy,&sp);
    if(ret==-1)
    {
        perror("pthread_setschedparam");
    }
	ret = prctl(PR_SET_NAME, name);
	if (ret == -1)
	{
		perror("prctl(PR_SET_NAME)");
	}

	return chunk->thread_fd;
}
int rx_thread_join(sys_handle_t what)
{
	eventfd_t buff = 0;
	int ret = read(what, &buff, sizeof(buff));
	if(ret<0)
        return RX_ERROR;
    else
        return RX_OK;
}
int rx_thread_close(sys_handle_t what)
{
	// warning this here leaves memory lekage
	// not so many threads created or stopped to have problem yet, but see what to do with it
	close(what);
	return RX_ERROR;
}
sys_handle_t rx_current_thread()
{
	return pthread_self();
}
int rx_thread_set_afinity(sys_handle_t what, uint64_t mask)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    uint64_t loop_mask=0x1;
    int current_bit=0;
    while(loop_mask)
    {
        if((loop_mask&mask)!=0)
            CPU_SET(current_bit, &set);
        loop_mask<<=1;
        current_bit++;
    }
    int ret_val = sched_setaffinity(0, sizeof(cpu_set_t), &set);
    if(ret_val==-1)
    {
        return RX_ERROR;
    }
    else
    {
        return RX_OK;
    }

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
		flags |= (O_CREAT | O_TRUNC);
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

	if (access != RX_FILE_OPEN_READ)
	{
		int mode = S_IRWXU | S_IRWXG | S_IROTH;

		ret = open(path, flags, mode);
	}
	else
    {
        ret = open(path, flags);
    }


	if (ret == -1)
	{
		return 0;
	}
	// this bellow is a bit crazy but that's the price for compatibility with windows!!!
	else if(ret == 0)
	{
        int nfd=dup(ret);
        close(ret);
        if (ret == -1)
        {
            return 0;
        }
        else
        {
            return nfd;
        }
	}
	else
	{
		return ret;
	}
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
    if(hndl<=0)
    {
        return RX_ERROR;
    }

	if (close(hndl) == -1)
	{
		return RX_ERROR;
	}
	else
	{
		return RX_OK;
    }
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
    int ret = access(path, F_OK);
    if(ret<0)
    {
        return 0;
    }
    else
        return 1;
}
// xstat stuff


struct xstat_parameters {
    unsigned long long	request_mask;
#define XSTAT_REQUEST_MODE		0x00000001ULL
#define XSTAT_REQUEST_NLINK		0x00000002ULL
#define XSTAT_REQUEST_UID		0x00000004ULL
#define XSTAT_REQUEST_GID		0x00000008ULL
#define XSTAT_REQUEST_RDEV		0x00000010ULL
#define XSTAT_REQUEST_ATIME		0x00000020ULL
#define XSTAT_REQUEST_MTIME		0x00000040ULL
#define XSTAT_REQUEST_CTIME		0x00000080ULL
#define XSTAT_REQUEST_INO		0x00000100ULL
#define XSTAT_REQUEST_SIZE		0x00000200ULL
#define XSTAT_REQUEST_BLOCKS		0x00000400ULL
#define XSTAT_REQUEST__BASIC_STATS	0x000007ffULL
#define XSTAT_REQUEST_BTIME		0x00000800ULL
#define XSTAT_REQUEST_GEN		0x00001000ULL
#define XSTAT_REQUEST_DATA_VERSION	0x00002000ULL
#define XSTAT_REQUEST__EXTENDED_STATS	0x00003fffULL
#define XSTAT_REQUEST__ALL_STATS	0x00003fffULL
};

struct xstat_dev {
    unsigned int	major;
    unsigned int	minor;
};

struct xstat_time {
    unsigned long long	tv_sec;
    unsigned long long	tv_nsec;
};

struct xstat {
    unsigned int		st_mode;
    unsigned int		st_nlink;
    unsigned int		st_uid;
    unsigned int		st_gid;
    struct xstat_dev	st_rdev;
    struct xstat_dev	st_dev;
    struct xstat_time	st_atim;
    struct xstat_time	st_mtim;
    struct xstat_time	st_ctim;
    struct xstat_time	st_btim;
    unsigned long long	st_ino;
    unsigned long long	st_size;
    unsigned long long	st_blksize;
    unsigned long long	st_blocks;
    unsigned long long	st_gen;
    unsigned long long	st_data_version;
    unsigned long long	st_result_mask;
    unsigned long long	st_extra_results[0];
};


#include <sys/syscall.h>

#define __NR_xstat				300
#define __NR_fxstat				301

ssize_t xstat(int dfd, const char* filename, unsigned flags,
    struct xstat_parameters* params,
    struct xstat* buffer, size_t bufsize)
{
    return syscall(__NR_xstat, dfd, filename, flags,
        params, buffer, bufsize);
}
ssize_t fxstat(int fd, unsigned flags,
    struct xstat_parameters* params,
    struct xstat* buffer, size_t bufsize)
{
    return syscall(__NR_fxstat, fd, flags,
        params, buffer, bufsize);
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



struct rx_time_struct_t get_rx_time_from_xstat(const struct xstat_time* tv)
{
    struct rx_time_struct_t ret;
    if (tv->tv_sec == 0 && tv->tv_nsec == 0)
    {
       ret.t_value = 0;
    }
    else
    {
        uint64_t temp = ((uint64_t)tv->tv_nsec) / 100 + ((uint64_t)tv->tv_sec) * 10000000;
        temp += server_time_struct_DIFF_TIMEVAL;
        ret.t_value = temp;
    }
    return ret;
}


int rx_file_get_time(sys_handle_t hndl, struct rx_time_struct_t* created, struct rx_time_struct_t* modified)
{
    struct xstat_parameters params;
	struct xstat data;

    params.request_mask = XSTAT_REQUEST_MTIME | XSTAT_REQUEST_BTIME;
	int ret = fxstat(hndl, 0, &params, &data, sizeof(data));
	if (ret == -1)
	{
		return RX_ERROR;
	}
	else
	{
        *created = get_rx_time_from_xstat(&data.st_btim);
        *modified = get_rx_time_from_xstat(&data.st_mtim);
		return RX_OK;
	}
}


int rx_file_get_time_from_path(const char* path, struct rx_time_struct_t* created, struct rx_time_struct_t* modified)
{
    struct xstat_parameters params;
    struct xstat data;

    params.request_mask = XSTAT_REQUEST_MTIME | XSTAT_REQUEST_BTIME;
    int ret = xstat(AT_FDCWD, path, 0, &params, &data, sizeof(data));
    if (ret == -1)
    {
        return RX_ERROR;
    }
    else
    {
        *created = get_rx_time_from_xstat(&data.st_btim);
        *modified = get_rx_time_from_xstat(&data.st_mtim);
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

	strcpy(path, ipath);

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
						while (pentry != NULL && !rx_match_pattern(pentry->d_name, filter, 1))
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
				while (pentry != NULL && !rx_match_pattern(pentry->d_name, ff->filter, 1))
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
uint32_t rx_io_read(struct rx_io_register_data_t* what, size_t* readed)
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
uint32_t rx_socket_read_from(struct rx_io_register_data_t* what, size_t* readed, struct sockaddr_storage* addr)
{
	struct linux_epoll_subscriber_t* internal = (struct linux_epoll_subscriber_t*)what->internal;
	internal->read_type = EPOLL_READ_FROM_TYPE;

    add_pending_op(internal);

    socklen_t addr_len = SOCKET_ADDR_SIZE;
	int ret = recvfrom(what->handle, what->read_buffer, what->read_buffer_size, 0, (struct sockaddr*)addr, &addr_len);
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
uint32_t rx_io_write(struct rx_io_register_data_t* what, const void* data, size_t count)
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
            ret=epoll_ctl(type_data->epoll_handle,EPOLL_CTL_MOD, __atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST),&event);
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
			return rx_io_write(what, temp, count - written);
		}
		else
		{
			internal->write_buffer = NULL;
			internal->write_type = 0;
		}
	}
	return RX_OK;
}

uint32_t rx_socket_write_to(struct rx_io_register_data_t* what, const void* data, size_t count, const struct sockaddr* addr, size_t addrsize)
{
	struct linux_epoll_subscriber_t* internal = (struct linux_epoll_subscriber_t*)what->internal;
	internal->write_buffer = data;
	internal->left_to_write = count;
	internal->write_type = EPOLL_WRITE_TO_TYPE;
	if(addrsize > SOCKET_ADDR_SIZE)
        return RX_ERROR;
    memcpy(internal->addr_buffer, addr, addrsize);
    internal->addr_size = addrsize;

    add_pending_op(internal);

	int ret = sendto(what->handle, data, count, 0, addr, (socklen_t)addrsize);
	if (ret == -1)
	{
		int err = errno;
		if (err == EAGAIN || err == EWOULDBLOCK)
		{

            struct linux_epoll_subscriber_t* type_data=(struct linux_epoll_subscriber_t*)what->internal;
            struct epoll_event event;
            event.data.ptr=what;
            event.events=EPOLLOUT|EPOLLET|EPOLLONESHOT;
            ret=epoll_ctl(type_data->epoll_handle,EPOLL_CTL_MOD, __atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST),&event);
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
			return rx_socket_write_to(what, temp, count - written, addr, addrsize);
		}
		else
		{
			internal->write_buffer = NULL;
			internal->write_type = 0;
		}
	}
	return RX_OK;
}

uint32_t rx_socket_accept(struct rx_io_register_data_t* what, uint32_t keep_alive)
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
        (what->accept_callback)(what->data,0,ret,addr,(struct sockaddr*)&local_addr);
    }

    return RX_OK;
}

uint32_t rx_socket_connect(struct rx_io_register_data_t* what, const struct sockaddr* addr, size_t addrsize)
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
            struct linux_epoll_subscriber_t* type_data = (struct linux_epoll_subscriber_t*)what->internal;
            struct epoll_event event;
            event.data.ptr = what;
            event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
            ret = epoll_ctl(type_data->epoll_handle, EPOLL_CTL_MOD, __atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST), &event);
            if (ret == 0)
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
        struct sockaddr_in local_addr;
        socklen_t addr_size=sizeof(local_addr);
        getsockname(ret,(struct sockaddr*)&local_addr,&addr_size);
        (what->connect_callback)(what->data, 0, (struct sockaddr*)addr,(struct sockaddr*)&local_addr);
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
        ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_ADD, __atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST),&event);
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
    int temp_write = 0;
    struct linux_epoll_subscriber_t* type_data = (struct linux_epoll_subscriber_t*)data->internal;
    temp_write = __atomic_exchange_4(&type_data->write_handle, 0, __ATOMIC_SEQ_CST);
    ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_DEL,data->handle,&event);
    if(ret==0)
    {
        ret=epoll_ctl(disp->epoll_fd,EPOLL_CTL_DEL, temp_write,&event);
        if(ret==0)
        {
            // here we take all operations that are not yet callbacked
            //  subsract 1000 to ensure that no one is fired after this code
            int ops=__atomic_fetch_sub(&type_data->pending_operations, 10000, __ATOMIC_SEQ_CST);
            //printf("####Still pending: %d\r\n",ops);
            close(temp_write);
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
                struct sockaddr_storage local_addr;
                struct sockaddr_storage remote_addr;
                struct linux_epoll_subscriber_t* type_data;
                struct rx_io_register_data_t* io_data;
                io_data = (struct rx_io_register_data_t*)one.data.ptr;
                type_data = (struct linux_epoll_subscriber_t*)io_data->internal;



                if (__atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST))
                {

                    if (one.events & EPOLLERR)// || one.events&EPOLLHUP)
                    {
                        int ops = remove_pending_op(type_data);
                        if (ops >= 0)
                            (io_data->shutdown_callback)(io_data->data, errno);
                    }
                    else
                    {
                        if (one.events & EPOLLIN)
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
                                            int ops = remove_pending_op(type_data);
                                            if (ops >= 0)
                                                (io_data->shutdown_callback)(io_data->data, errno);
                                        }
                                        // nothing to do here, he will call us again any way
                                    }
                                    else
                                    {
                                        int ops = remove_pending_op(type_data);
                                        if (ops >= 0)
                                        {
                                            // READLOOP
                                            //if(err==0)// closed up
                                            //    (io_data->shutdown_callback)(io_data->data, errno);
                                           // else
                                            (io_data->read_callback)(io_data->data, 0, err);
                                        }
                                    }

                                    if (__atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST))
                                    {
                                        one.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                                        epoll_ctl(disp->epoll_fd, EPOLL_CTL_MOD, io_data->handle, &one);
                                    }
                                }
                                break;

                            case EPOLL_READ_FROM_TYPE:
                                {
                                    type_data->read_type = 0;

                                    struct sockaddr_storage addr;
                                    socklen_t addr_len = SOCKET_ADDR_SIZE;
                                    int err = recvfrom(io_data->handle, io_data->read_buffer, io_data->read_buffer_size, 0, (struct sockaddr*)&addr, &addr_len);
                                    if (err == -1)
                                    {
                                        if (err != EAGAIN && err != EWOULDBLOCK)
                                        {
                                            int ops = remove_pending_op(type_data);
                                            if (ops >= 0)
                                                (io_data->shutdown_callback)(io_data->data, errno);
                                        }
                                        // nothing to do here, he will call us again any way
                                    }
                                    else
                                    {
                                        int ops = remove_pending_op(type_data);
                                        if (ops >= 0)
                                        {
                                            // READLOOP
                                            //if(err==0)// closed up
                                            //    (io_data->shutdown_callback)(io_data->data, errno);
                                            //else
                                            (io_data->read_from_callback)(io_data->data, 0, err, (struct sockaddr*)&addr, addr_len);
                                        }
                                    }
                                    if (__atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST))
                                    {
                                        one.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                                        epoll_ctl(disp->epoll_fd, EPOLL_CTL_MOD, io_data->handle, &one);
                                    }
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
                                            int ops = remove_pending_op(type_data);
                                            if (ops >= 0)
                                                (io_data->shutdown_callback)(io_data->data, errno);
                                        }
                                        // nothing to do here, he will call us again any way
                                    }
                                    else
                                    {
                                        int ops = remove_pending_op(type_data);
                                        if (ops >= 0)
                                        {
                                            socklen_t addr_size = sizeof(local_addr);
                                            getsockname(err, (struct sockaddr*)&local_addr, &addr_size);
                                            (io_data->accept_callback)(io_data->data, 0, err, io_data->read_buffer, (struct sockaddr*)&local_addr);
                                        }
                                    }
                                    if (__atomic_load_4(&type_data->write_handle, __ATOMIC_SEQ_CST))
                                    {
                                        one.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                                        epoll_ctl(disp->epoll_fd, EPOLL_CTL_MOD, io_data->handle, &one);
                                    }
                                }
                                break;
                            }
                        }
                        if (one.events & EPOLLOUT)
                        {// writing on handle
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
                                                int ops = remove_pending_op(type_data);
                                                if (ops >= 0)
                                                    (io_data->shutdown_callback)(io_data->data, err);
                                            }
                                            // nothing to do here, he will call us again any way
                                        }
                                        else
                                        {
                                            size_t written = (size_t)rt;
                                            if (written < type_data->left_to_write)
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
                                                        int ops = remove_pending_op(type_data);
                                                        if (ops >= 0)
                                                            (io_data->shutdown_callback)(io_data->data, err);
                                                    }
                                                    // nothing to do here, he will call us again any way
                                                }
                                            }
                                            else
                                            {
                                                type_data->write_type = 0;
                                                type_data->write_buffer = NULL;
                                                int ops = remove_pending_op(type_data);
                                                if (ops >= 0)
                                                    (io_data->write_callback)(io_data->data, 0);
                                            }
                                        }
                                    }
                                    else
                                    {
                                    }

                                }
                                break;
                            case EPOLL_WRITE_TO_TYPE:
                                {
                                    if (type_data->write_buffer)
                                    {
                                        int rt = sendto(io_data->handle, type_data->write_buffer, type_data->left_to_write, 0
                                            , (struct sockaddr*)type_data->addr_buffer, type_data->addr_size);
                                        if (rt == -1)
                                        {
                                            int err = errno;
                                            if (err != EAGAIN && err != EWOULDBLOCK)
                                            {// error occured do the shutdown
                                                int ops = remove_pending_op(type_data);
                                                if (ops >= 0)
                                                    (io_data->shutdown_callback)(io_data->data, err);
                                            }
                                            // nothing to do here, he will call us again any way
                                        }
                                        else
                                        {
                                            size_t written = (size_t)rt;
                                            if (written < type_data->left_to_write)
                                            {// din't write all do it again
                                                uint8_t* temp = (uint8_t*)type_data->write_buffer;
                                                temp += written;
                                                type_data->write_buffer = temp;
                                                type_data->left_to_write = type_data->left_to_write - written;
                                                rt = sendto(io_data->handle, type_data->write_buffer, type_data->left_to_write, 0
                                                    , (struct sockaddr*)type_data->addr_buffer, type_data->addr_size);
                                                if (rt == -1)
                                                {
                                                    int err = errno;
                                                    if (err != EAGAIN && err != EWOULDBLOCK)
                                                    {// error occured do the shutdown
                                                        int ops = remove_pending_op(type_data);
                                                        if (ops >= 0)
                                                            (io_data->shutdown_callback)(io_data->data, err);
                                                    }
                                                    // nothing to do here, he will call us again any way
                                                }
                                            }
                                            else
                                            {
                                                type_data->write_type = 0;
                                                type_data->write_buffer = NULL;
                                                int ops = remove_pending_op(type_data);
                                                if (ops >= 0)
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

                                    socklen_t addr_size = sizeof(local_addr);
                                    int err = getsockname(io_data->handle, (struct sockaddr*)&local_addr, &addr_size);
                                    if (err == 0)
                                    {
                                        addr_size = sizeof(local_addr);
                                        err = getpeername(io_data->handle, (struct sockaddr*)&remote_addr, &addr_size);
                                    }
                                    int ops = remove_pending_op(type_data);
                                    if (ops >= 0)
                                    {
                                        if (err == 0)
                                            (io_data->connect_callback)(io_data->data, 0, (struct sockaddr*)&remote_addr, (struct sockaddr*)&local_addr);
                                        else
                                            (io_data->shutdown_callback)(io_data->data, err);
                                    }

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

sys_handle_t rx_create_and_bind_ip4_tcp_socket(const struct sockaddr_in* addr, uint32_t keep_alive)
{
    sys_handle_t ret=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
    if(ret>0)
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


        if(addr->sin_family == AF_INET && 0==bind(ret,(const struct sockaddr*)addr,sizeof(struct sockaddr_in)))
        {
            return ret;
        }
    }
    if(ret>0)
        close(ret);

    return 0;
}

sys_handle_t rx_create_and_bind_ip4_udp_socket(const struct sockaddr_in* addr)
{
    sys_handle_t ret=socket(AF_INET,SOCK_DGRAM|SOCK_NONBLOCK,0);
    if(ret>0)
    {

        int on = 1;
        setsockopt(ret, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on));

        if(addr->sin_port!=0)
        {//this is listen or udp server socket mark resuse
            on=1;
            setsockopt(ret,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
        }

        if (addr->sin_family == AF_INET)
        {
            int result = bind(ret, (const struct sockaddr*)addr, sizeof(struct sockaddr_in));
            if (0 == result)
            {
                return ret;
            }
        }
    }
    if(ret>0)
        close(ret);
    return 0;
}
uint32_t rx_socket_listen(sys_handle_t handle)
{
    return (0==listen(handle,SOMAXCONN));
}
void rx_close_socket(sys_handle_t handle)
{
    if(handle>0)
    {
        shutdown(handle,SHUT_RDWR);
        close(handle);
    }
}

sys_handle_t rx_open_serial_port(const char* port, uint32_t baud_rate, int stop_bits, int parity, uint8_t data_bits, int handshake)
{
    errno = ENOSYS;
    return 0;
}
void rx_close_serial_port(sys_handle_t handle)
{
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





