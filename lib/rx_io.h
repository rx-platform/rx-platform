

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


#include "os_itf/rx_ositf.h"

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
        return _destination_context;
      }



  protected:

      bool connect_dispatcher (threads::dispatcher_pool::smart_ptr& dispatcher);

      bool disconnect_dispatcher ();

      void register_timed ();

      void unregister_timed ();


      threads::dispatcher_pool::smart_ptr _disptacher;


      rx_io_register_data _dispatcher_data;


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



      static time_aware_subscribers_type _time_aware_subscribers;


      static locks::lockable _time_aware_subscribers_lock;

      rx_kernel_dispather_t _dispatcher_handle;

      rx_thread_handle_t _destination_context;

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
        _receive_timeout = value;
      }



  protected:

      bool read_loop ();

      virtual bool readed (const void* data, size_t count, rx_thread_handle_t destination) = 0;

      virtual void release_buffer (buffer_ptr what) = 0;


      bool _sending;

      bool _receiving;

      bool _shutdown_called;

      locks::lockable _read_lock;

      locks::lockable _write_lock;


  private:

      int internal_read_callback (size_t count, dword status);

      int internal_write_callback (dword status);

      int internal_shutdown_callback (dword status);

      bool write_loop ();



      dword _send_timeout;

      dword _receive_timeout;

      dword _send_tick;

      dword _receive_tick;

      queue_type _sending_queue;

      buffer_ptr _current_buffer;


};






template <class buffT>
class tcp_socket : public full_duplex_comm<buffT>  
{
	DECLARE_REFERENCE_PTR(tcp_socket<buffT>);


  public:
      tcp_socket (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher);

      virtual ~tcp_socket();


  protected:
      tcp_socket();


  private:


      byte* _buffer;

      string_type _peer_name;

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

      bool start_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, word port);

      void stop ();


  protected:

      virtual result_ptr make_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher, rx_thread_handle_t destination) = 0;


  private:

      int internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, dword status);

      bool accept_new ();

      int internal_shutdown_callback (dword status);



      byte _buffer[ACCEPT_BUFFER_SIZE];

	  friend int dispatcher_accept_callback(void* data, dword status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr, size_t size);
	  friend int listen_dispatcher_shutdown_callback(void* data, dword status);
};






typedef tcp_listen_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_listent_std_buffer;






template <class buffT>
class tcp_client_socket : public tcp_socket<buffT>  
{
	DECLARE_REFERENCE_PTR(tcp_client_socket<buffT>);

  public:
      tcp_client_socket();

      virtual ~tcp_client_socket();


      bool bind_socket (threads::dispatcher_pool::smart_ptr dispatcher, sockaddr_in* addr);

      bool bind_socket_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, word port = 0);

      bool connect_to (threads::dispatcher_pool::smart_ptr dispatcher, sockaddr* addr, size_t addrsize);

      bool connect_to_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, unsigned long address, word port);

      bool connect_to_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, const string_type& address, word port);

      void timer_tick (dword tick);

      void close ();


      void set_connect_timeout (dword value)
      {
        _connect_timeout = value;
      }



  protected:

  private:

      int internal_connect_callback (dword status);



      dword _connect_timeout;

      bool _connecting;

      dword _connect_tick;


};






typedef tcp_client_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_client_socket_std_buffer;






typedef tcp_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_socket_std_buffer;


// Parameterized Class rx::io::full_duplex_comm 

template <class buffT>
full_duplex_comm<buffT>::full_duplex_comm()
      : _send_timeout(2000),
        _receive_timeout(10000),
        _sending(false),
        _receiving(false),
        _send_tick(0),
        _receive_tick(0),
        _shutdown_called(false)
{
}

template <class buffT>
full_duplex_comm<buffT>::full_duplex_comm (sys_handle_t handle, threads::dispatcher_pool::smart_ptr& dispatcher)
      : _send_timeout(2000),
        _receive_timeout(10000),
        _sending(false),
        _receiving(false),
        _send_tick(0),
        _receive_tick(0),
        _shutdown_called(false)
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
	_read_lock.lock();
	receiving=_receiving;
	_receiving = false;
	_read_lock.unlock();
	if(!receiving)
        return 0;

	if (status == 0 && count != 0)
	{
		if (readed(_dispatcher_data.read_buffer, count,get_destination_context()))
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
	_write_lock.lock();
	_sending = false;
	current = _current_buffer;
	_current_buffer = buffer_ptr::null_ptr;
	_write_lock.unlock();
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

	if (_shutdown_called)
		return 0;
	_read_lock.lock();
	_write_lock.lock();
	if (_shutdown_called)
	{
		_write_lock.unlock();
		_read_lock.unlock();
		return 0;
	}
	_receiving = false;
	_sending = false;
	disconnect_dispatcher();
	rx_close_socket(_dispatcher_data.handle);
	_shutdown_called = true;
	_write_lock.unlock();
	_read_lock.unlock();
	on_shutdown(get_destination_context());
	unregister_timed();
	return 1;
}

template <class buffT>
void full_duplex_comm<buffT>::timer_tick (dword tick)
{

	_read_lock.lock();
	dword rec_diff = tick - _receive_tick;
	bool receiving = _receiving;
	_read_lock.unlock();

	_write_lock.lock();
	dword send_diff = tick - _send_tick;
	bool sending = _sending;
	_write_lock.unlock();


	if (rec_diff>0x80000000)
		rec_diff = 0;
	if (send_diff>0x80000000)
		send_diff = 0;

	if (receiving && _receive_timeout>0)
	{
		if (rec_diff>_receive_timeout)
		{
            initiate_shutdown();
			return;
		}

	}
	else if (sending && _send_timeout>0)
	{
		if (send_diff>_send_timeout)
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

	_write_lock.lock();

	while (result == RX_OK)
	{
		void* data = nullptr;
		size_t size = 0;
		if (!_current_buffer && !_sending_queue.empty())
		{
			_current_buffer = _sending_queue.front();
			_sending_queue.pop();
			if (!_current_buffer)
			{// close sign
				ret = false;
				shutdown = true;
				break;
			}
			else
			{
				data = this->_current_buffer->get_data();
				size = _current_buffer->get_size();
			}
		}
		if (data && size)
		{// do the sending

			_send_tick = rx_get_tick_count();
			_sending = true;
			bind();
			result = rx_system_write(&_dispatcher_data, data, size);
			if (result != RX_ASYNC)
			{
				release();

				_sending = false;

				if (ret == RX_ERROR)
				{
					shutdown = true;
					ret = false;
				}
				else if (ret == RX_OK)
				{
					buffer_ptr current;
					if (_current_buffer)
					{
						current = _current_buffer;
						_current_buffer = buffer_ptr::null_ptr;
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

	_write_lock.unlock();

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

    _read_lock.lock();

	do
	{
		_receiving = true;
		_receive_tick = rx_get_tick_count();
		bind();
		result = rx_system_read(&_dispatcher_data, &bytes);
		if (result != RX_ASYNC)
		{
			release();
			_receiving = false;
			if (result == RX_OK)
			{// done in sync fire it
                if(bytes==0)
                {
                    ret=false;
                    break;
                }
				if (!readed(_dispatcher_data.read_buffer, bytes, get_destination_context()))
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

    _read_lock.unlock();

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
	_write_lock.lock();
	_sending_queue.push(what);
	_write_lock.unlock();
	write_loop();
	return true;
}

template <class buffT>
bool full_duplex_comm<buffT>::send (buffer_ptr what)
{
	// same code as previous, hope the compiler will figure it out :)
	if (what && what->empty())
		return true;
	_write_lock.lock();
	_sending_queue.push(what);
	_write_lock.unlock();
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
{

  // allocate paged memory to be faster
	_buffer = (byte*)rx_allocate_os_memory(TCP_BUFFER_SIZE);
	this->_dispatcher_data.read_buffer = _buffer;
	this->_dispatcher_data.read_buffer_size = TCP_BUFFER_SIZE;
	this->_dispatcher_data.data = this;
}

template <class buffT>
tcp_socket<buffT>::tcp_socket (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher)
{

  // allocate paged memory to be faster
	_buffer = (byte*)rx_allocate_os_memory(TCP_BUFFER_SIZE);
	_peer_name = inet_ntoa(addr->sin_addr);
	if (_peer_name.empty())
		_peer_name = "<unknown>";

	this->_dispatcher_data.handle = handle;
	this->_dispatcher_data.read_buffer = _buffer;
	this->_dispatcher_data.read_buffer_size = TCP_BUFFER_SIZE;
	this->_dispatcher_data.data = this;
	this->connect_dispatcher(dispatcher);

	this->register_timed();
}


template <class buffT>
tcp_socket<buffT>::~tcp_socket()
{
	printf("###### tcp sockwet destroyed\r\n");
	rx_deallocate_os_memory(_buffer, TCP_BUFFER_SIZE);
}



// Parameterized Class rx::io::tcp_listen_socket 

template <class buffT>
tcp_listen_socket<buffT>::tcp_listen_socket()
{

	_dispatcher_data.read_buffer = _buffer;
	_dispatcher_data.read_buffer_size = ACCEPT_BUFFER_SIZE;
	_dispatcher_data.data = this;

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
	target_ptr created = make_client(handle, addr, local_addr, _disptacher,get_destination_context());
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
	this->_dispatcher_data.handle = ::rx_create_and_bind_ip4_tcp_socket(addr);
	if (this->_dispatcher_data.handle)
	{
		if (rx_socket_listen(this->_dispatcher_data.handle))
		{
			if (this->connect_dispatcher(dispatcher))
			{
				if (accept_new())
					return true;
			}
		}
	}
	if (this->_dispatcher_data.handle)
		rx_close_socket(this->_dispatcher_data.handle);
	return false;
}

template <class buffT>
bool tcp_listen_socket<buffT>::start_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, word port)
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
	rx_close_socket(_dispatcher_data.handle);
}

template <class buffT>
bool tcp_listen_socket<buffT>::accept_new ()
{
	bind();
	dword ret = rx_system_accept(&_dispatcher_data);
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


// Parameterized Class rx::io::tcp_client_socket 

template <class buffT>
tcp_client_socket<buffT>::tcp_client_socket()
      : _connect_timeout(2000),
        _connecting(false),
        _connect_tick(0)
{
}


template <class buffT>
tcp_client_socket<buffT>::~tcp_client_socket()
{
}



template <class buffT>
bool tcp_client_socket<buffT>::bind_socket (threads::dispatcher_pool::smart_ptr dispatcher, sockaddr_in* addr)
{
	this->connect_dispatcher(dispatcher);
	this->_dispatcher_data.handle = rx_create_and_bind_ip4_tcp_socket(addr);
	if (this->_dispatcher_data.handle)
	{
		return true;
	}
	if (this->_dispatcher_data.handle)
		rx_close_socket(this->_dispatcher_data.handle);
	return false;
}

template <class buffT>
bool tcp_client_socket<buffT>::bind_socket_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, word port)
{
	struct sockaddr_in temp_addr;
	memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = INADDR_ANY;
	return bind_socket(dispatcher, &temp_addr);
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_to (threads::dispatcher_pool::smart_ptr dispatcher, sockaddr* addr, size_t addrsize)
{
	{

		if (this->_connecting || !this->_sending || !this->_receiving)
			return false;

		this->_connect_tick = ::rx_get_tick_count();

		this->_connecting = true;
	}
	this->bind();
	bool ret = (RX_OK == rx_system_connect(&this->_dispatcher_data,  addr, addrsize));
	if (!ret)
	{
		this->_connecting = false;
		this->release();
		close();
	}
	return ret;
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_to_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, unsigned long address, word port)
{
	struct sockaddr_in temp_addr;
	memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = htonl(address);
	return connect_to(dispatcher,(sockaddr*)&temp_addr,sizeof(temp_addr));
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_to_tcpip_4 (threads::dispatcher_pool::smart_ptr dispatcher, const string_type& address, word port)
{
    unsigned long num_addr=inet_addr(address.c_str());
    struct sockaddr_in temp_addr;
    memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = htonl(num_addr);
	return connect_to(dispatcher, (sockaddr*)&temp_addr,sizeof(temp_addr));
}

template <class buffT>
int tcp_client_socket<buffT>::internal_connect_callback (dword status)
{
	this->_write_lock.lock();
	_connecting = false;
	this->_write_lock.unlock();
	if (status == 0)
	{
		//if (connect_complete())
		return 1;
	}
	this->initiate_shutdown();
	return 0;
}

template <class buffT>
void tcp_client_socket<buffT>::timer_tick (dword tick)
{
	full_duplex_comm<buffT>::timer_tick(tick);

	if (!this->_connecting)
		return;// don't have to lock here because we have double lock bellow

	this->_write_lock.lock();
	dword conn_diff = tick - this->_connect_tick;
	bool connecting = this->_connecting;

	this->_write_lock.unlock();

	if (conn_diff>0x80000000)
		conn_diff = 0;
	if (connecting && this->_connect_timeout>0)
	{
		if (conn_diff>this->_connect_timeout)
		{
			this->initiate_shutdown();
			return;
		}
	}
}

template <class buffT>
void tcp_client_socket<buffT>::close ()
{
	this->disconnect_dispatcher();
	::rx_close_socket(this->_dispatcher_data.handle);
}


} // namespace io
} // namespace rx



#endif
