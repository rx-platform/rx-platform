

/****************************************************************************
*
*  interfaces\rx_datagram_io.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_datagram_io_h
#define rx_datagram_io_h 1



// rx_io
#include "interfaces/rx_io.h"



namespace rx_internal {

namespace interfaces {

namespace ip_endpoints {





template <class buffT>
class udp_socket : public io_endpoints::dispatcher_subscriber  
{
    DECLARE_REFERENCE_PTR(udp_socket);
public:

    typedef typename buffT::smart_ptr buffer_ptr;
    typedef buffT buffer_t;

protected:
    typedef std::queue<std::pair<sockaddr_storage, buffer_ptr> > queue_type;

  public:
      ~udp_socket();


      rx_result bind_socket_udpip_4 (const sockaddr_in* addr, threads::dispatcher_pool& dispatcher);

      void close ();

      virtual bool on_startup (rx_thread_handle_t destination);

      virtual void on_shutdown (rx_security_handle_t identity);

      void initiate_shutdown ();

      bool write (buffer_ptr what, const struct sockaddr* addr, size_t addrsize);

      bool start_loops ();


      void set_receive_timeout (uint32_t value)
      {
        receive_timeout_ = value;
      }



  protected:
      udp_socket();


      bool read_loop ();

      virtual void release_buffer (buffer_ptr what) = 0;

      virtual bool readed (const void* data, size_t count, const struct sockaddr* addr, rx_security_handle_t identity) = 0;


      bool sending_;

      bool receiving_;

      bool shutdown_called_;

      locks::lockable read_lock_;

      locks::lockable write_lock_;


  private:

      int internal_read_from_callback (size_t count, uint32_t status, struct sockaddr* addr, size_t addrsize);

      int internal_write_callback (uint32_t status);

      int internal_shutdown_callback (uint32_t status);

      bool write_loop ();



      uint8_t* buffer_;

      string_type peer_name_;

      uint32_t send_timeout_;

      uint32_t receive_timeout_;

      rx_timer_ticks_t send_tick_;

      rx_timer_ticks_t receive_tick_;

      queue_type sending_queue_;

      buffer_ptr current_buffer_;


};






typedef udp_socket< memory::std_strbuff<memory::std_vector_allocator>  > udp_socket_std_buffer;


// Parameterized Class rx_internal::interfaces::ip_endpoints::udp_socket 

template <class buffT>
udp_socket<buffT>::udp_socket()
      : send_timeout_(2000),
        receive_timeout_(10000),
        sending_(false),
        receiving_(false),
        send_tick_(0),
        receive_tick_(0),
        shutdown_called_(false)
{
    buffer_ = (uint8_t*)rx_allocate_os_memory(UDP_BUFFER_SIZE);
    this->dispatcher_data_.read_buffer = buffer_;
    this->dispatcher_data_.read_buffer_size = UDP_BUFFER_SIZE;
    this->dispatcher_data_.data = this;
    register_timed();
}


template <class buffT>
udp_socket<buffT>::~udp_socket()
{
}



template <class buffT>
rx_result udp_socket<buffT>::bind_socket_udpip_4 (const sockaddr_in* addr, threads::dispatcher_pool& dispatcher)
{
    this->dispatcher_data_.handle = rx_create_and_bind_ip4_udp_socket(addr);
    if (this->dispatcher_data_.handle)
    {
        this->connect_dispatcher(dispatcher);
        this->start_loops();
        return true;
    }
    return rx_result::create_from_last_os_error("Unable to bind to endpoint.");
}

template <class buffT>
void udp_socket<buffT>::close ()
{
    this->disconnect_dispatcher();
    ::rx_close_socket(this->dispatcher_data_.handle);
}

template <class buffT>
int udp_socket<buffT>::internal_read_from_callback (size_t count, uint32_t status, struct sockaddr* addr, size_t addrsize)
{
    bool receiving;
    bool ret = false;
    read_lock_.lock();
    receiving = receiving_;
    receiving_ = false;
    read_lock_.unlock();
    if (!receiving)
        return 0;

    if (status == 0)
    {
        if (count != 0)
        {
            if (readed(dispatcher_data_.read_buffer, count, addr, get_identity()))
            {
                ret = read_loop();
                if (!ret)
                {   // read closes socket here
                    internal_shutdown_callback(0);
                }
            }
        }
        else
        {// UDP error shit
            ret = read_loop();
        }
    }
    return ret ? 1 : 0;
}

template <class buffT>
int udp_socket<buffT>::internal_write_callback (uint32_t status)
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
int udp_socket<buffT>::internal_shutdown_callback (uint32_t status)
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
    on_shutdown(get_identity());
    unregister_timed();
    return 1;
}

template <class buffT>
bool udp_socket<buffT>::write_loop ()
{
    uint32_t result = RX_OK;
    bool ret = true;
    bool shutdown = false;
    std::pair<sockaddr_storage, buffer_ptr> packet_data;

    write_lock_.lock();

    while (result == RX_OK)
    {
        void* data = nullptr;
        size_t size = 0;
        if (!current_buffer_ && !sending_queue_.empty())
        {
            packet_data = sending_queue_.front();
            current_buffer_ = packet_data.second;
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
            result = rx_socket_write_to(&dispatcher_data_, data, size, (sockaddr*)&packet_data.first, SOCKET_ADDR_SIZE);
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
                    if (current)
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

    if (!ret)
    {
        initiate_shutdown();
    }

    return ret;
}

template <class buffT>
bool udp_socket<buffT>::read_loop ()
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
        sockaddr_storage addr;
        result = rx_socket_read_from(&dispatcher_data_, &bytes, &addr);
        if (result != RX_ASYNC)
        {
            release();
            receiving_ = false;
            if (result == RX_OK)
            {// done in sync fire it
                if (bytes == 0)
                {
                    ret = false;
                    break;
                }
                if (!readed(dispatcher_data_.read_buffer, bytes,(sockaddr*)&addr, get_identity()))
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
bool udp_socket<buffT>::on_startup (rx_thread_handle_t destination)
{
    return true;
}

template <class buffT>
void udp_socket<buffT>::on_shutdown (rx_security_handle_t identity)
{
}

template <class buffT>
void udp_socket<buffT>::initiate_shutdown ()
{
    internal_shutdown_callback(255);
}

template <class buffT>
bool udp_socket<buffT>::write (buffer_ptr what, const struct sockaddr* addr, size_t addrsize)
{
    if (what && what->empty())
        return true;
    if (addr == nullptr || addrsize > sizeof(sockaddr_storage))
        return false;

    std::pair<sockaddr_storage, buffer_ptr> entry;
    memcpy(&entry.first, addr, addrsize);
    entry.second = what;
    write_lock_.lock();
    sending_queue_.push(entry);
    write_lock_.unlock();
    write_loop();
    return true;
}

template <class buffT>
bool udp_socket<buffT>::start_loops ()
{
    return read_loop();
}


} // namespace ip_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
