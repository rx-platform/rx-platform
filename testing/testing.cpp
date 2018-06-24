

#include "stdafx.h"
#include "testing.h"
#include "lib/rx_thread.h"
#include "lib/rx_job.h"
#include "lib/rx_io.h"
#include "lib/rx_std.h"
#include "system/meta/rx_objbase.h"
#include "system/callbacks/rx_callback.h"
#include "system/constructors/rx_construct.h"
#include "system/server/rx_inf.h"
#include "lib/rx_log.h"
#include "system/server/rx_server.h"
#include "lib/rx_ptr.h"


using namespace rx;


namespace testing
{

namespace obsolite
{
	class my_periodic_job : public rx::jobs::periodic_job
	{
		DECLARE_REFERENCE_PTR(my_periodic_job);
	public:
		my_periodic_job()
		{
		}
		void process()
		{
			printf("Timer fired at %s\r\n", rx::rx_time(rx::time_stamp::now().rx_time).get_string().c_str());
		}
	};

	void test_timers()
	{
		rx::threads::dispatcher_pool pool(2, "ime",55);
		rx::threads::timer tm("tm",70);

		pool.run();
		tm.start();

		printf("Sleeping\r\n");

		my_periodic_job::smart_ptr job(rx::pointers::_create_new);
		tm.append_job(job, &pool, 100, false);

		rx_msleep(1000);


		printf("Stopping\r\n");

		tm.stop();
		tm.wait_handle();
		pool.end();
	}

/////////////// callbacks



class virtual_test : public pointers::virtual_reference_object
{
	DECLARE_VIRTUAL_REFERENCE_PTR(virtual_test);
public:
	virtual_test()
	{
		printf("UsaoV\r\n");
	}
	~virtual_test()
	{
		printf("IzasaoV\r\n");
	}
};


class smart_test : public pointers::reference_object, public virtual_test
{
	DECLARE_REFERENCE_PTR(smart_test);
public:
	smart_test()
	{
		printf("Usao\r\n");
	}
	~smart_test()
	{
		printf("Izasao\r\n");
	}
protected:
	void virtual_bind()
	{
		bind();
	}

	void virtual_release()
	{
		release();
	}
};

using namespace rx::pointers;

class A : public reference_object
{
};

void accept_reference(reference<reference_object> ref);
// this function is not aware of class A
void send_object(reference<reference_object> ref)
{
	accept_reference(ref);
}


// this function implementation knows definition of class A
void accept_reference(reference<reference_object> ref)
{
	reference<A> received = ref.cast_to<reference<A> >();
}

// this function is knows definition of class A
void some_function1()
{
	send_object(create_reference<A>());
}

int test_smart_ptr()
{


	string_type str = rx::rx_time::now().get_string();

	printf("Time=%s \r\n", str.c_str());

	rx_reference<smart_test> ref(pointers::_create_new);

	virtual_test::smart_ptr vref = ref;

	vref = rx_create_reference<smart_test>();

	return 0;
}

class my_thread : public rx::threads::thread
{
public:
	my_thread()
		: rx::threads::thread("Test",66)
	{

	}

protected:
	virtual uint32_t handler()
	{
		printf("Usao\r\n");
		rx_msleep(1500);
		printf("Izasao\r\n");
		return 0;
	}
};

class my_job : public rx::jobs::job
{
	DECLARE_REFERENCE_PTR(my_job);
public:
	void process()
	{
		printf("sleeping inside job\r\n");
		rx_msleep(700);
		printf("sleep done\r\n");
	}
};

void testing(int i)
{
	printf("jbt_uaso sam!!!!\r\n");
		rx_msleep(2000);
	printf("jbt_izasao sam!!!!\r\n");
}
/*
class my_socket : public io::tcp_socket
{
	DECLARE_REFERENCE_PTR(my_socket);
public:
	my_socket(sys_handle_t handle, sockaddr_in* addr, threads::dispatcher_pool::smart_ptr& dispatcher)
		: io::tcp_socket(handle, addr,addr, dispatcher)
	{
        printf("Socket created \r\n");
	}
	~my_socket()
	{
        printf("Socket destoryed \r\n");
	}
	bool on_startup()
	{
		char buff[0x100];
		rx_collect_system_info(buff, 0x100);
		string_type hello("Hello from ");
		hello += buff;
		hello += " !!!\r\n";
		send(hello.c_str(), hello.size());
		return io::tcp_socket::on_startup();
	}
	bool send_complete()
	{
		return receive(nullptr)==RX_OK;
	}
	bool receive_complete(const void* data, size_t count)
	{
		if (count > 0 && *(char*)data == 'q')
			return false;
		send(data, count);
		return true;
	}
};

class my_listener : public io::tcp_lisent_socket
{
protected:
	io::tcp_socket::smart_ptr make_client(sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher)
	{
		return my_socket::smart_ptr(handle,addr,dispatcher);
	}
};

*/
void test_thread()
{
    char buff[0x100];
    rx_collect_system_info(buff,0x100);
    printf("OS:%s\r\n",buff);
    rx_collect_processor_info(buff,0x100);
    printf("CPU:%s\r\n",buff);
	size_t total,free,process;
    rx_collect_memory_info(&total,&free,&process);
    printf("MEM: Total %dMB / Free %dMB\r\n",(int)(total/(1024*1024)),(int)(free/(1024*1024)));

	//my_listener lisent;
	my_thread thread;
	rx::threads::dispatcher_pool::smart_ptr jthread(2,"test pool",78);
	//rx::thr_fx::physical_job_thread jthread;
	jthread->run();
	thread.start();

	//lisent.start(jthread, 12345);

	//my_job::smart_ptr job(pointers::_create_new);

	std::function<void(int)> fptr(testing);

	rx::jobs::lambda_job<int>::smart_ptr job(testing,55);

	jthread->append(job);
	jthread->append(job);
	jthread->append(job);

	thread.wait_handle();

	int a;
	scanf("%d",&a);

	printf("%d\r\n",a);

	//lisent.stop();

	jthread->end();
    printf("Izasao Main\r\n");
}

void test_classes()
{
	rx_platform::meta::object_class_ptr obj("test_class", 55, true);
}


namespace tcp_connect_test
{
class tcp_test_client : public rx::io::tcp_client_socket<rx::memory::std_buffer>
{
	DECLARE_REFERENCE_PTR(tcp_test_client);
public:
	tcp_test_client()
	{
	}
protected:
	bool readed(const void* data, size_t count, rx_thread_handle_t destination)
	{
		return true;
	}
	virtual void release_buffer(buffer_ptr what)
	{
	}
};


}// tcp_connect_test

}// obsolite

}// testing
