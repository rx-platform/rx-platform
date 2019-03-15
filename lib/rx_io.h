

/****************************************************************************
*
*  lib\rx_io.h
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


#ifndef rx_io_h
#define rx_io_h 1



// rx_mem
#include "lib/rx_mem.h"
// rx_thread
#include "lib/rx_thread.h"
// rx_ptr
#include "lib/rx_ptr.h"

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

      ~dispatcher_subscriber();


      static void propagate_timer ();

      virtual void timer_tick (uint32_t tick);


      const rx_thread_handle_t get_destination_context () const
      {
        return destination_context_;
      }



  protected:

      bool connect_dispatcher (threads::dispatcher_pool& dispatcher);

      bool disconnect_dispatcher ();

      void register_timed ();

      void unregister_timed ();


      rx_io_register_data dispatcher_data_;


  private:

      virtual int internal_read_callback (size_t count, uint32_t status);

      virtual int internal_write_callback (uint32_t status);

      virtual int internal_shutdown_callback (uint32_t status);

      int _internal_read_callback (size_t count, uint32_t status);

      int _internal_write_callback (uint32_t status);

      int _internal_shutdown_callback (uint32_t status);

      virtual int internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status);

      int _internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status);

      virtual int internal_connect_callback (uint32_t status);

      int _internal_connect_callback (uint32_t status);



      static time_aware_subscribers_type time_aware_subscribers_;


      static locks::lockable time_aware_subscribers_lock_;

      rx_kernel_dispather_t dispatcher_handle_;

      rx_thread_handle_t destination_context_;

	  friend int dispatcher_read_callback(void* data, uint32_t status, size_t size);
	  friend int dispatcher_write_callback(void* data, uint32_t status);
	  friend int dispatcher_connect_callback(void* data, uint32_t status);
	  friend int dispatcher_shutdown_callback(void* data, uint32_t status);
	  friend int dispatcher_accept_callback(void* data, uint32_t status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr, size_t size);
};







template <class buffT>
class full_duplex_comm : public dispatcher_subscriber  
{
	DECLARE_REFERENCE_PTR(full_duplex_comm<buffT>);

protected:
	typedef std::function<bool(const void*, size_t, rx_thread_handle_t)> readed_function_t;
	typedef typename buffT::smart_ptr buffer_ptr;
	typedef std::queue<buffer_ptr> queue_type;

  public:
      full_duplex_comm();

      ~full_duplex_comm();


      void timer_tick (uint32_t tick);

      virtual bool on_startup (rx_thread_handle_t destination);

      virtual void on_shutdown (rx_thread_handle_t destination);

      void initiate_shutdown ();

      bool write (buffer_ptr what);

      bool start_loops ();


      void set_receive_timeout (uint32_t value)
      {
        receive_timeout_ = value;
      }



  protected:

      bool read_loop ();

      virtual void release_buffer (buffer_ptr what) = 0;

      virtual bool readed (const void* data, size_t count, rx_thread_handle_t destination) = 0;


      bool sending_;

      bool receiving_;

      bool shutdown_called_;

      locks::lockable read_lock_;

      locks::lockable write_lock_;


  private:

      int internal_read_callback (size_t count, uint32_t status);

      int internal_write_callback (uint32_t status);

      int internal_shutdown_callback (uint32_t status);

      bool write_loop ();



      uint32_t send_timeout_;

      uint32_t receive_timeout_;

      uint32_t send_tick_;

      uint32_t receive_tick_;

      queue_type sending_queue_;

      buffer_ptr current_buffer_;


};






template <class buffT>
class tcp_socket : public full_duplex_comm<buffT>  
{
	DECLARE_REFERENCE_PTR(tcp_socket<buffT>);


  public:
      tcp_socket (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher);

      ~tcp_socket();


  protected:
      tcp_socket();


  private:


      uint8_t* buffer_;

      string_type peer_name_;

	  friend int dispatcher_connect_callback(void* data, uint32_t status);
};







template <class buffT>
class tcp_listen_socket : public dispatcher_subscriber  
{
	DECLARE_REFERENCE_PTR(tcp_listen_socket<buffT>);
protected:
	typedef typename tcp_socket<buffT>::smart_ptr result_ptr;
public:
	typedef std::function<result_ptr(sys_handle_t, sockaddr_in*, sockaddr_in*, rx_thread_handle_t)> make_function_t;

  public:
      tcp_listen_socket (make_function_t make_function);

      ~tcp_listen_socket();


      bool start (sockaddr_in* addr, threads::dispatcher_pool& dispatcher);

      bool start_tcpip_4 (uint16_t port, threads::dispatcher_pool& dispatcher);

      void stop ();


  protected:

  private:

      int internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status);

      bool accept_new ();

      int internal_shutdown_callback (uint32_t status);



      uint8_t buffer_[ACCEPT_BUFFER_SIZE];

      bool connected_;

      make_function_t make_function_;

	  friend int dispatcher_accept_callback(void* data, uint32_t status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr, size_t size);
	  friend int listen_dispatcher_shutdown_callback(void* data, uint32_t status);
};






typedef tcp_listen_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_listent_std_buffer;






typedef tcp_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_socket_std_buffer;






template <class buffT>
class tcp_client_socket : public tcp_socket<buffT>  
{
	DECLARE_REFERENCE_PTR(tcp_client_socket<buffT>);

  public:
      tcp_client_socket();

      ~tcp_client_socket();


      bool bind_socket (sockaddr_in* addr, threads::dispatcher_pool& dispatcher);

      bool bind_socket_tcpip_4 (uint16_t port, threads::dispatcher_pool& dispatcher);

      bool connect_to (sockaddr* addr, size_t addrsize);

      bool connect_to_tcpip_4 (unsigned long address, uint16_t port);

      bool connect_to_tcpip_4 (const string_type& address, uint16_t port);

      void timer_tick (uint32_t tick);

      void close ();

      virtual bool connect_complete ();

      bool bind_socket_tcpip_4 (threads::dispatcher_pool& dispatcher);


      void set_connect_timeout (uint32_t value)
      {
        connect_timeout_ = value;
      }



  protected:

  private:

      int internal_connect_callback (uint32_t status);



      uint32_t connect_timeout_;

      bool connecting_;

      uint32_t connect_tick_;


};






typedef tcp_client_socket< memory::std_strbuff<memory::std_vector_allocator>  > tcp_client_socket_std_buffer;


// Parameterized Class rx::io::full_duplex_comm

template <class buffT>
full_duplex_comm<buffT>::full_duplex_comm()
      : send_timeout_(2000),
        receive_timeout_(10000),
        sending_(false),
        receiving_(false),
        send_tick_(0),
        receive_tick_(0),
        shutdown_called_(false)
{
}


template <class buffT>
full_duplex_comm<buffT>::~full_duplex_comm()
{
}



template <class buffT>
int full_duplex_comm<buffT>::internal_read_callback (size_t count, uint32_t status)
{
    bool receiving;
	bool ret = false;
	read_lock_.lock();
	receiving=receiving_;
	receiving_ = false;
	read_lock_.unlock();
	if(!receiving)
        return 0;

	if (status == 0 && count != 0)
	{
		if (readed(dispatcher_data_.read_buffer, count,get_destination_context()))
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
int full_duplex_comm<buffT>::internal_write_callback (uint32_t status)
{

	buffer_ptr current;
	bool ret = true;
	write_lock_.lock();
	sending_ = false;
	current = current_buffer_;
	current_buffer_ = buffer_ptr::null_ptr;
	write_lock_.unlock();
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
int full_duplex_comm<buffT>::internal_shutdown_callback (uint32_t status)
{

	if (shutdown_called_)
		return 0;
	read_lock_.lock();
	write_lock_.lock();
	if (shutdown_called_)
	{
		write_lock_.unlock();
		read_lock_.unlock();
		return 0;
	}
	receiving_ = false;
	sending_ = false;
	disconnect_dispatcher();
	rx_close_socket(dispatcher_data_.handle);
	shutdown_called_ = true;
	write_lock_.unlock();
	read_lock_.unlock();
	on_shutdown(get_destination_context());
	unregister_timed();
	return 1;
}

template <class buffT>
void full_duplex_comm<buffT>::timer_tick (uint32_t tick)
{

	read_lock_.lock();
	uint32_t rec_diff = tick - receive_tick_;
	bool receiving = receiving_;
	read_lock_.unlock();

	write_lock_.lock();
	uint32_t send_diff = tick - send_tick_;
	bool sending = sending_;
	write_lock_.unlock();


	if (rec_diff>0x80000000)
		rec_diff = 0;
	if (send_diff>0x80000000)
		send_diff = 0;

	if (receiving && receive_timeout_>0)
	{
		if (rec_diff>receive_timeout_)
		{
            initiate_shutdown();
			return;
		}

	}
	else if (sending && send_timeout_>0)
	{
		if (send_diff>send_timeout_)
		{
            initiate_shutdown();
			return;
		}

	}
}

template <class buffT>
bool full_duplex_comm<buffT>::write_loop ()
{

	uint32_t result = RX_OK;
	bool ret = true;
	bool shutdown = false;

	write_lock_.lock();

	while (result == RX_OK)
	{
		void* data = nullptr;
		size_t size = 0;
		if (!current_buffer_ && !sending_queue_.empty())
		{
			current_buffer_ = sending_queue_.front();
			sending_queue_.pop();
			if (!current_buffer_)
			{// close sign
				ret = false;
				shutdown = true;
				break;
			}
			else
			{
				data = this->current_buffer_->get_data();
				size = current_buffer_->get_size();
			}
		}
		if (data && size)
		{// do the sending

			send_tick_ = rx_get_tick_count();
			sending_ = true;
			bind();
			result = rx_socket_write(&dispatcher_data_, data, size);
			if (result != RX_ASYNC)
			{
				release();

				sending_ = false;

				if (ret == RX_ERROR)
				{
					shutdown = true;
					ret = false;
				}
				else if (ret == RX_OK)
				{
					buffer_ptr current;
					if (current_buffer_)
					{
						current = current_buffer_;
						current_buffer_ = buffer_ptr::null_ptr;
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

	write_lock_.unlock();

	if (shutdown)
		internal_shutdown_callback(255);

    if(!ret)
    {
        initiate_shutdown();
    }

	return ret;
}

template <class buffT>
bool full_duplex_comm<buffT>::read_loop ()
{


	uint32_t result = RX_OK;
	bool ret = true;
	size_t bytes = 0;

    read_lock_.lock();

	do
	{
		receiving_ = true;
		receive_tick_ = rx_get_tick_count();
		bind();
		result = rx_socket_read(&dispatcher_data_, &bytes);
		if (result != RX_ASYNC)
		{
			release();
			receiving_ = false;
			if (result == RX_OK)
			{// done in sync fire it
                if(bytes==0)
                {
                    ret=false;
                    break;
                }
				if (!readed(dispatcher_data_.read_buffer, bytes, get_destination_context()))
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

    read_lock_.unlock();

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
	if (what && what->empty())
		return true;
	write_lock_.lock();
	sending_queue_.push(what);
	write_lock_.unlock();
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
	buffer_ = (uint8_t*)rx_allocate_os_memory(TCP_BUFFER_SIZE);
	this->dispatcher_data_.read_buffer = buffer_;
	this->dispatcher_data_.read_buffer_size = TCP_BUFFER_SIZE;
	this->dispatcher_data_.data = this;
}

template <class buffT>
tcp_socket<buffT>::tcp_socket (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool& dispatcher)
{

  // allocate paged memory to be faster
	buffer_ = (uint8_t*)rx_allocate_os_memory(TCP_BUFFER_SIZE);
	peer_name_ = inet_ntoa(addr->sin_addr);
	if (peer_name_.empty())
		peer_name_ = "<unknown>";

	this->dispatcher_data_.handle = handle;
	this->dispatcher_data_.read_buffer = buffer_;
	this->dispatcher_data_.read_buffer_size = TCP_BUFFER_SIZE;
	this->dispatcher_data_.data = this;
	this->connect_dispatcher(dispatcher);

	this->register_timed();
}


template <class buffT>
tcp_socket<buffT>::~tcp_socket()
{
	//printf("###### tcp sockwet destroyed\r\n");
	rx_deallocate_os_memory(buffer_, TCP_BUFFER_SIZE);
}



// Parameterized Class rx::io::tcp_listen_socket

template <class buffT>
tcp_listen_socket<buffT>::tcp_listen_socket (make_function_t make_function)
      : connected_(false),
        make_function_(make_function)
{
	dispatcher_data_.read_buffer = buffer_;
	dispatcher_data_.read_buffer_size = ACCEPT_BUFFER_SIZE;
	dispatcher_data_.data = this;
}


template <class buffT>
tcp_listen_socket<buffT>::~tcp_listen_socket()
{
	//printf("###### tcp listener destroyed\r\n");
}



template <class buffT>
int tcp_listen_socket<buffT>::internal_accept_callback (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status)
{
	if (!connected_)
		return 0;
    typedef typename tcp_socket<buffT>::smart_ptr target_ptr;
	target_ptr created = make_function_(handle, addr, local_addr, get_destination_context());
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
bool tcp_listen_socket<buffT>::start (sockaddr_in* addr, threads::dispatcher_pool& dispatcher)
{
	this->dispatcher_data_.handle = ::rx_create_and_bind_ip4_tcp_socket(addr);
	if (this->dispatcher_data_.handle)
	{
		if (rx_socket_listen(this->dispatcher_data_.handle))
		{
			if (this->connect_dispatcher(dispatcher))
			{
				if (accept_new())
					return true;
			}
		}
	}
	if (this->dispatcher_data_.handle)
		rx_close_socket(this->dispatcher_data_.handle);
	return false;
}

template <class buffT>
bool tcp_listen_socket<buffT>::start_tcpip_4 (uint16_t port, threads::dispatcher_pool& dispatcher)
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
	rx_close_socket(dispatcher_data_.handle);
}

template <class buffT>
bool tcp_listen_socket<buffT>::accept_new ()
{
	bind();
	uint32_t ret = rx_socket_accept(&dispatcher_data_);
	if (ret == RX_ERROR)
	{
		release();
		return false;
	}
	else
		return true;
}

template <class buffT>
int tcp_listen_socket<buffT>::internal_shutdown_callback (uint32_t status)
{
	return 1;
}


// Parameterized Class rx::io::tcp_client_socket

template <class buffT>
tcp_client_socket<buffT>::tcp_client_socket()
      : connect_timeout_(2000),
        connecting_(false),
        connect_tick_(0)
{
}


template <class buffT>
tcp_client_socket<buffT>::~tcp_client_socket()
{
}



template <class buffT>
bool tcp_client_socket<buffT>::bind_socket (sockaddr_in* addr, threads::dispatcher_pool& dispatcher)
{
	this->dispatcher_data_.handle = rx_create_and_bind_ip4_tcp_socket(addr);
	if (this->dispatcher_data_.handle)
	{
		this->connect_dispatcher(dispatcher);
		return true;
	}
	if (this->dispatcher_data_.handle)
		rx_close_socket(this->dispatcher_data_.handle);
	return false;
}

template <class buffT>
bool tcp_client_socket<buffT>::bind_socket_tcpip_4 (uint16_t port, threads::dispatcher_pool& dispatcher)
{
	struct sockaddr_in temp_addr;
	memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = INADDR_ANY;
	return bind_socket(&temp_addr, dispatcher);
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_to (sockaddr* addr, size_t addrsize)
{
	{

		if (this->connecting_ || this->sending_ || this->receiving_)
			return false;

		this->connect_tick_ = ::rx_get_tick_count();

		this->connecting_ = true;
	}
	this->bind();
	bool ret = (RX_ERROR != rx_socket_connect(&this->dispatcher_data_,  addr, addrsize));
	if (!ret)
	{
		this->connecting_ = false;
		this->release();
		close();
	}
	return ret;
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_to_tcpip_4 (unsigned long address, uint16_t port)
{
	struct sockaddr_in temp_addr;
	memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = address;
	return connect_to((sockaddr*)&temp_addr,sizeof(temp_addr));
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_to_tcpip_4 (const string_type& address, uint16_t port)
{
    unsigned long num_addr=inet_addr(address.c_str());
    struct sockaddr_in temp_addr;
    memzero(&temp_addr, sizeof(temp_addr));
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port);
	temp_addr.sin_addr.s_addr = num_addr;
	return connect_to((sockaddr*)&temp_addr,sizeof(temp_addr));
}

template <class buffT>
int tcp_client_socket<buffT>::internal_connect_callback (uint32_t status)
{
	this->write_lock_.lock();
	connecting_ = false;
	this->write_lock_.unlock();
	if (status == 0)
	{
		if (connect_complete())
			return 1;
	}
	this->initiate_shutdown();
	return 0;
}

template <class buffT>
void tcp_client_socket<buffT>::timer_tick (uint32_t tick)
{
	full_duplex_comm<buffT>::timer_tick(tick);

	if (!this->connecting_)
		return;// don't have to lock here because we have double lock bellow

	this->write_lock_.lock();
	uint32_t conn_diff = tick - this->connect_tick_;
	bool connecting = this->connecting_;

	this->write_lock_.unlock();

	if (conn_diff>0x80000000)
		conn_diff = 0;
	if (connecting && this->connect_timeout_>0)
	{
		if (conn_diff>this->connect_timeout_)
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
	::rx_close_socket(this->dispatcher_data_.handle);
}

template <class buffT>
bool tcp_client_socket<buffT>::connect_complete ()
{
	this->read_loop();
	return true;
}

template <class buffT>
bool tcp_client_socket<buffT>::bind_socket_tcpip_4 (threads::dispatcher_pool& dispatcher)
{
	return bind_socket(0, dispatcher);
}


} // namespace io
} // namespace rx



#endif
