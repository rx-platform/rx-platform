

/****************************************************************************
*
*  lib\rx_io.h
*
*  Copyright (c) 2017 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_io_h
#define rx_io_h 1



// rx_ptr
#include "lib/rx_ptr.h"
// rx_mem
#include "lib/rx_mem.h"
// rx_thread
#include "lib/rx_thread.h"

namespace rx {
namespace io {
class dispatcher_subscriber;

} // namespace io
} // namespace rx




namespace rx {

namespace io {
typedef std::set<io::dispatcher_subscriber> dispatcher_subscriber_ptr;
typedef std::set<rx::pointers::reference<dispatcher_subscriber> > time_aware_subscribers_type;





class dispatcher_subscriber : public pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(dispatcher_subscriber);

  public:
      dispatcher_subscriber (rx_thread_handle_t destination = RX_THREAD_NULL);

      virtual ~dispatcher_subscriber();


      static void propagate_timer ();

      virtual void timer_tick (dword tick);


      const rx_thread_handle_t get_destination_context () const
      {
        return m_destination_context;
      }



  protected:

      bool connect_dispatcher (threads::dispatcher_pool::smart_ptr& dispatcher);

      bool disconnect_dispatcher ();

      void register_timed ();

      void unregister_timed ();


      threads::dispatcher_pool::smart_ptr m_disptacher;


      rx_io_register_data m_dispatcher_data;


  private:

      virtual int internal_read_callback (size_t count, dword status);

      virtual int internal_write_callback (dword status);

      virtual int internal_shutdown_callback (dword status);

      int _internal_read_callback (size_t count, dword status);

      int _internal_write_callback (dword status);

      int _internal_shutdown_callback (dword status);

      virtual int internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, dword status);

      int _internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, dword status);

      virtual int internal_connect_callback (dword status);

      int _internal_connect_callback (dword status);



      static time_aware_subscribers_type m_time_aware_subscribers;


      static locks::lockable m_time_aware_subscribers_lock;

      rx_kernel_dispather_t m_dispatcher_handle;

      rx_thread_handle_t m_destination_context;

	  friend int dispatcher_read_callback(void* data, dword status, size_t size);
	  friend int dispatcher_write_callback(void* data, dword status);
	  friend int dispatcher_connect_callback(void* data, dword status);
	  friend int dispatcher_shutdown_callback(void* data, dword status);
	  friend int dispatcher_accept_callback(void* data, dword status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr, size_t size);
};







template <class buffT>
class full_duplex_comm : public dispatcher_subscriber  
{
	DECLARE_REFERENCE_PTR(full_duplex_comm<buffT>);

protected:
	typedef typename buffT::smart_ptr buffer_ptr;
	typedef std::queue<buffer_ptr> queue_type;

  public:
      full_duplex_comm();

      full_duplex_comm (sys_handle_t handle, threads::dispatcher_pool::smart_ptr& dispatcher);

      virtual ~full_duplex_comm();


      void timer_tick (dword tick);

      virtual bool on_startup (rx_thread_handle_t destination);

      virtual void on_shutdown (rx_thread_handle_t destination);

      void initiate_shutdown ();

      bool write (buffer_ptr what);

      bool send (buffer_ptr what);

      bool start_loops ();


      void set_receive_timeout (dword value)
      {
        m_receive_timeout = value;
      }



  protected:

      bool read_loop ();

      virtual bool readed (const void* data, size_t count, rx_thread_handle_t destination) = 0;

      virtual void release_buffer (buffer_ptr what) = 0;


      locks::lockable m_read_lock;

      locks::lockable m_write_lock;


  private:

      int internal_read_callback (size_t count, dword status);

      int internal_write_callback (dword status);

      int internal_shutdown_callback (dword status);

      bool write_loop ();



      dword m_send_timeout;

      dword m_receive_timeout;

      bool m_sending;

      bool m_receiving;

      dword m_send_tick;

      dword m_receive_tick;

      bool m_shutdown_called;

      queue_type m_sending_queue;

      buffer_ptr m_current_buffer;


};






template <class buffT>
class tcp_socket : public full_duplex_comm<buffT>  
{
	DECLARE_REFERENCE_PTR(tcp_socket<buffT>);


  public:
      tcp_socket();

      tcp_socket (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher);

      virtual ~tcp_socket();


      void timer_tick (dword tick);


      void set_connect_timeout (dword value)
      {
        m_connect_timeout = value;
      }



  protected:

  private:

      int internal_connect_callback (dword status);



      byte* m_buffer;

      dword m_connect_timeout;

      bool m_connecting;

      dword m_connect_tick;

      string_type m_peer_name;

	  friend int dispatcher_connect_callback(void* data, dword status);
};







template <class buffT>
class tcp_listen_socket : public dispatcher_subscriber  
{
	DECLARE_REFERENCE_PTR(tcp_listen_socket<buffT>);
protected:
	typedef typename tcp_socket<buffT>::smart_ptr result_ptr;

  public:
      tcp_listen_socket();

      virtual ~tcp_listen_socket();


      bool start (threads::dispatcher_pool::smart_ptr dispatcher, sockaddr_in* addr);

      bool start (threads::dispatcher_pool::smart_ptr dispatcher, word port);

      void stop ();


  protected:

      virtual result_ptr make_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher, rx_thread_handle_t destination) = 0;


  private:

      int internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, dword status);

      bool accept_new ();

      int internal_shutdown_callback (dword status);



      byte m_buffer[ACCEPT_BUFFER_SIZE];

	  friend int dispatcher_accept_callback(void* data, dword status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr, size_t size);
	  friend int listen_dispatcher_shutdown_callback(void* data, dword status);
};






typedef tcp_listen_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_listent_str_buffer;






typedef tcp_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_socket_str_buffer;


// Parameterized Class rx::io::full_duplex_comm 

template <class buffT>
full_duplex_comm<buffT>::full_duplex_comm()
      : m_send_timeout(2000),
        m_receive_timeout(10000),
        m_sending(false),
        m_receiving(false),
        m_send_tick(0),
        m_receive_tick(0),
        m_shutdown_called(false)
{
}

template <class buffT>
full_duplex_comm<buffT>::full_duplex_comm (sys_handle_t handle, threads::dispatcher_pool::smart_ptr& dispatcher)
      : m_send_timeout(2000),
        m_receive_timeout(10000),
        m_sending(false),
        m_receiving(false),
        m_send_tick(0),
        m_receive_tick(0),
        m_shutdown_called(false)
{
}


template <class buffT>
full_duplex_comm<buffT>::~full_duplex_comm()
{
}



template <class buffT>
int full_duplex_comm<buffT>::internal_read_callback (size_t count, dword status)
{
    bool receiving;
	bool ret = false;
	m_read_lock.lock();
	receiving=m_receiving;
	m_receiving = false;
	m_read_lock.unlock();
	if(!receiving)
        return 0;

	if (status == 0 && count != 0)
	{
		if (readed(m_dispatcher_data.read_buffer, count,get_destination_context()))
		{
			ret = read_loop();
			if(!ret)
			{   // read closes socket here
				internal_shutdown_callback(0);
			}
		}
	}
	return ret ? 1 : 0;
}

template <class buffT>
int full_duplex_comm<buffT>::internal_write_callback (dword status)
{

	buffer_ptr current;
	bool ret = true;
	m_write_lock.lock();
	m_sending = false;
	current = m_current_buffer;
	m_current_buffer = buffer_ptr::null_ptr;
	m_write_lock.unlock();
	if (current)
	{
		release_buffer(current);
		if (status == 0)
		{
			ret = write_loop();
		}
	}
	return ret ? 1 : 0;
}

template <class buffT>
int full_duplex_comm<buffT>::internal_shutdown_callback (dword status)
{

	if (m_shutdown_called)
		return 0;
	m_read_lock.lock();
	m_write_lock.lock();
	if (m_shutdown_called)
	{
		m_write_lock.unlock();
		m_read_lock.unlock();
		return 0;
	}
	m_receiving = false;
	m_sending = false;
	disconnect_dispatcher();
	rx_close_socket(m_dispatcher_data.handle);
	m_shutdown_called = true;
	m_write_lock.unlock();
	m_read_lock.unlock();
	on_shutdown(get_destination_context());
	unregister_timed();
	return 1;
}

template <class buffT>
void full_duplex_comm<buffT>::timer_tick (dword tick)
{

	m_read_lock.lock();
	dword rec_diff = tick - m_receive_tick;
	bool receiving = m_receiving;
	m_read_lock.unlock();

	m_write_lock.lock();
	dword send_diff = tick - m_send_tick;
	bool sending = m_sending;
	m_write_lock.unlock();


	if (rec_diff>0x80000000)
		rec_diff = 0;
	if (send_diff>0x80000000)
		send_diff = 0;

	if (receiving && m_receive_timeout>0)
	{
		if (rec_diff>m_receive_timeout)
		{
            initiate_shutdown();
			return;
		}

	}
	else if (sending && m_send_timeout>0)
	{
		if (send_diff>m_send_timeout)
		{
            initiate_shutdown();
			return;
		}

	}
}

template <class buffT>
bool full_duplex_comm<buffT>::write_loop ()
{

	dword result = RX_OK;
	bool ret = true;
	bool shutdown = false;

	m_write_lock.lock();

	while (result == RX_OK)
	{
		void* data = nullptr;
		size_t size = 0;
		if (!m_current_buffer && !m_sending_queue.empty())
		{
			m_current_buffer = m_sending_queue.front();
			m_sending_queue.pop();
			if (!m_current_buffer)
			{// close sign
				ret = false;
				shutdown = true;
				break;
			}
			else
			{
				data = this->m_current_buffer->get_data();
				size = m_current_buffer->get_size();
			}
		}
		if (data && size)
		{// do the sending

			m_send_tick = rx_get_tick_count();
			m_sending = true;
			bind();
			result = rx_system_write(&m_dispatcher_data, data, size);
			if (result != RX_ASYNC)
			{
				release();

				m_sending = false;

				if (ret == RX_ERROR)
				{
					shutdown = true;
					ret = false;
				}
				else if (ret == RX_OK)
				{
					buffer_ptr current;
					if (m_current_buffer)
					{
						current = m_current_buffer;
						m_current_buffer = buffer_ptr::null_ptr;
					}
					if(current)
                        release_buffer(current);
				}
			}
			// if result==RX_ASYNC we do nothing just wait for callback
		}
		else
		{// nothing more
			break;
		}
	}

	m_write_lock.unlock();

	if (shutdown)
		internal_shutdown_callback(255);
    else if(!ret)// close requested
        initiate_shutdown();
	return ret;
}

template <class buffT>
bool full_duplex_comm<buffT>::read_loop ()
{


	dword result = RX_OK;
	bool ret = true;
	size_t bytes = 0;

    m_read_lock.lock();

	do
	{
		m_receiving = true;
		m_receive_tick = rx_get_tick_count();
		bind();
		result = rx_system_read(&m_dispatcher_data, &bytes);
		if (result != RX_ASYNC)
		{
			release();
			m_receiving = false;
			if (result == RX_OK)
			{// done in sync fire it
                if(bytes==0)
                {
                    ret=false;
                    break;
                }
				if (!readed(m_dispatcher_data.read_buffer, bytes, get_destination_context()))
				{
					ret = false;
					break;
				}
			}
			else if (result == RX_ERROR)
			{
				ret = false;
			}
		}
		// if result==RX_ASYNC we do nothing just wait for callback

	} while (result == RX_OK);

    m_read_lock.unlock();

	return ret;
}

template <class buffT>
bool full_duplex_comm<buffT>::on_startup (rx_thread_handle_t destination)
{
	return true;
}

template <class buffT>
void full_duplex_comm<buffT>::on_shutdown (rx_thread_handle_t destination)
{
}

template <class buffT>
void full_duplex_comm<buffT>::initiate_shutdown ()
{

	internal_shutdown_callback(255);
	
}

template <class buffT>
bool full_duplex_comm<buffT>::write (buffer_ptr what)
{
	if (what->empty())
		return true;
	m_write_lock.lock();
	m_sending_queue.push(what);
	m_write_lock.unlock();
	write_loop();
	return true;
}

template <class buffT>
bool full_duplex_comm<buffT>::send (buffer_ptr what)
{
	// same code as previous, hope the compiler will figure it out :)
	if (what && what->empty())
		return true;
	m_write_lock.lock();
	m_sending_queue.push(what);
	m_write_lock.unlock();
	write_loop();
	return true;
}

template <class buffT>
bool full_duplex_comm<buffT>::start_loops ()
{
	return read_loop();
}


// Parameterized Class rx::io::tcp_socket 

template <class buffT>
tcp_socket<buffT>::tcp_socket()
      : m_connect_timeout(2000),
        m_connecting(false),
        m_connect_tick(0)
{

  // allocate paged memory to be faster
	m_buffer = (byte*)rx_allocate_os_memory(TCP_BUFFER_SIZE);
	this->m_dispatcher_data.read_buffer = m_buffer;
	this->m_dispatcher_data.read_buffer_size = TCP_BUFFER_SIZE;
	this->m_dispatcher_data.data = this;
}

template <class buffT>
tcp_socket<buffT>::tcp_socket (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher)
      : m_connect_timeout(2000),
        m_connecting(false),
        m_connect_tick(0)
{

  // allocate paged memory to be faster
	m_buffer = (byte*)rx_allocate_os_memory(TCP_BUFFER_SIZE);
	m_peer_name = inet_ntoa(addr->sin_addr);
	if (m_peer_name.empty())
		m_peer_name = "<unknown>";

	this->m_dispatcher_data.handle = handle;
	this->m_dispatcher_data.read_buffer = m_buffer;
	this->m_dispatcher_data.read_buffer_size = TCP_BUFFER_SIZE;
	this->m_dispatcher_data.data = this;
	this->connect_dispatcher(dispatcher);

	this->register_timed();
}


template <class buffT>
tcp_socket<buffT>::~tcp_socket()
{
	printf("###### tcp sockwet destroyed\r\n");
	rx_deallocate_os_memory(m_buffer, TCP_BUFFER_SIZE);
}



template <class buffT>
int tcp_socket<buffT>::internal_connect_callback (dword status)
{
	this->m_write_lock.lock();
	m_connecting = false;
	this->m_write_lock.unlock();
	if (status == 0)
	{
		//if (connect_complete())
		return 1;
	}
	this->initiate_shutdown();
	return 0;
}

template <class buffT>
void tcp_socket<buffT>::timer_tick (dword tick)
{
	full_duplex_comm<buffT>::timer_tick(tick);

	if (!this->m_connecting)
		return;// don't have to lock here because we have double lock bellow

	this->m_write_lock.lock();
	dword conn_diff = tick - this->m_connect_tick;
	bool connecting = this->m_connecting;

	this->m_write_lock.unlock();

	if (conn_diff>0x80000000)
		conn_diff = 0;
	if (connecting && this->m_connect_timeout>0)
	{
		if (conn_diff>this->m_connect_timeout)
		{
			this->initiate_shutdown();
			return;
		}
	}
}


// Parameterized Class rx::io::tcp_listen_socket 

template <class buffT>
tcp_listen_socket<buffT>::tcp_listen_socket()
{

	m_dispatcher_data.read_buffer = m_buffer;
	m_dispatcher_data.read_buffer_size = ACCEPT_BUFFER_SIZE;
	m_dispatcher_data.data = this;

}


template <class buffT>
tcp_listen_socket<buffT>::~tcp_listen_socket()
{
	printf("###### tcp listener destroyed\r\n");
}



template <class buffT>
int tcp_listen_socket<buffT>::internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, dword status)
{
    typedef typename tcp_socket<buffT>::smart_ptr target_ptr;
	target_ptr created = make_client(handle, addr, local_addr, m_disptacher,get_destination_context());
	if (created)
	{
		if (created->start_loops())
		{
			created->on_startup(get_destination_context());
		}
	}
	else
	{
		rx_close_socket(handle);
	}
	bool ret = accept_new();
	return ret ? 1 : 0;
}

template <class buffT>
bool tcp_listen_socket<buffT>::start (threads::dispatcher_pool::smart_ptr dispatcher, sockaddr_in* addr)
{
	m_dispatcher_data.handle = rx_create_and_bind_ip4_tcp_socket(addr);
	if (m_dispatcher_data.handle)
	{
		if (rx_socket_listen(m_dispatcher_data.handle))
		{
			if (connect_dispatcher(dispatcher))
			{
				if (accept_new())
					return true;
			}
		}
	}
	if (m_dispatcher_data.handle)
		rx_close_socket(m_dispatcher_data.handle);
	return false;
}

template <class buffT>
bool tcp_listen_socket<buffT>::start (threads::dispatcher_pool::smart_ptr dispatcher, word port)
{
	struct sockaddr_in temp_addr;
	memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = INADDR_ANY;
	return start(dispatcher, &temp_addr);
}

template <class buffT>
void tcp_listen_socket<buffT>::stop ()
{
	disconnect_dispatcher();
	rx_close_socket(m_dispatcher_data.handle);
}

template <class buffT>
bool tcp_listen_socket<buffT>::accept_new ()
{
	bind();
	dword ret = rx_system_accept(&m_dispatcher_data);
	if (ret == RX_ERROR)
	{
		release();
		return false;
	}
	else
		return true;
}

template <class buffT>
int tcp_listen_socket<buffT>::internal_shutdown_callback (dword status)
{
	return 1;
}


} // namespace io
} // namespace rx



#endif
