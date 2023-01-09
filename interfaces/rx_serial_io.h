

/****************************************************************************
*
*  interfaces\rx_serial_io.h
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


#ifndef rx_serial_io_h
#define rx_serial_io_h 1



// rx_io
#include "interfaces/rx_io.h"



namespace rx_internal {

namespace interfaces {

namespace serial {

struct serial_port_data_t
{
    serial_port_data_t()
        : baud_rate(19200)
        , stop_bits(0)
        , parity(0)
        , data_bits(8)
        , handshake(false)
    {
    }
    string_type port;
    uint32_t baud_rate;
    int stop_bits;
    int parity;
    uint8_t data_bits;
    bool handshake;
};





template <class buffT>
class serial_comm : public io_endpoints::dispatcher_subscriber  
{
    DECLARE_REFERENCE_PTR(serial_comm);
public:

    typedef typename buffT::smart_ptr buffer_ptr;
    typedef buffT buffer_t;

protected:
    typedef std::queue<buffer_ptr> queue_type;

  public:
      ~serial_comm();


      rx_result open_serial (const serial_port_data_t& data, threads::dispatcher_pool& dispatcher);

      void close ();

      virtual bool on_startup (rx_thread_handle_t destination);

      virtual void on_shutdown (rx_security_handle_t identity);

      void initiate_shutdown ();

      bool write (buffer_ptr what);

      bool start_loops ();


      void set_receive_timeout (uint32_t value)
      {
        receive_timeout_ = value;
      }



  protected:
      serial_comm();


      bool read_loop ();

      virtual void release_buffer (buffer_ptr what) = 0;

      virtual bool readed (const void* data, size_t count, rx_security_handle_t identity) = 0;


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



      uint8_t* buffer_;

      uint32_t send_timeout_;

      uint32_t receive_timeout_;

      rx_timer_ticks_t send_tick_;

      rx_timer_ticks_t receive_tick_;

      queue_type sending_queue_;

      buffer_ptr current_buffer_;


};






typedef serial_comm< memory::std_strbuff<memory::std_vector_allocator>  > serial_comm_std_buffer;


// Parameterized Class rx_internal::interfaces::serial::serial_comm 

template <class buffT>
serial_comm<buffT>::serial_comm()
      : send_timeout_(2000),
        receive_timeout_(10000),
        sending_(false),
        receiving_(false),
        send_tick_(0),
        receive_tick_(0),
        shutdown_called_(false)
{
    buffer_ = (uint8_t*)rx_allocate_os_memory(SERIAL_BUFFER_SIZE);

    this->dispatcher_data_.handle = 0;
    this->dispatcher_data_.read_buffer = buffer_;
    this->dispatcher_data_.read_buffer_size = SERIAL_BUFFER_SIZE;
    this->dispatcher_data_.data = this;

    this->register_timed();
}


template <class buffT>
serial_comm<buffT>::~serial_comm()
{
    rx_deallocate_os_memory(buffer_, SERIAL_BUFFER_SIZE);
}



template <class buffT>
rx_result serial_comm<buffT>::open_serial (const serial_port_data_t& data, threads::dispatcher_pool& dispatcher)
{
    auto handle = ::rx_open_serial_port(data.port.c_str(), data.baud_rate, data.stop_bits, data.parity, data.data_bits, data.handshake ? 1 : 0);
    if (handle)
    {
        this->dispatcher_data_.handle = handle;
        this->dispatcher_data_.data = this;
        this->connect_dispatcher(dispatcher);
        this->start_loops();
        return true;
    }
    else
    {
        return rx_result::create_from_last_os_error("Error opening serial port.");
    }
}

template <class buffT>
void serial_comm<buffT>::close ()
{
    this->disconnect_dispatcher();
    ::rx_close_serial_port(this->dispatcher_data_.handle);
}

template <class buffT>
int serial_comm<buffT>::internal_read_callback (size_t count, uint32_t status)
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
            if (readed(dispatcher_data_.read_buffer, count, get_identity()))
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
int serial_comm<buffT>::internal_write_callback (uint32_t status)
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
int serial_comm<buffT>::internal_shutdown_callback (uint32_t status)
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
    rx_close_serial_port(dispatcher_data_.handle);
    shutdown_called_ = true;
    write_lock_.unlock();
    read_lock_.unlock();
    on_shutdown(get_identity());
    unregister_timed();
    return 1;
}

template <class buffT>
bool serial_comm<buffT>::write_loop ()
{
    uint32_t result = RX_OK;
    bool ret = true;
    bool shutdown = false;
    buffer_ptr packet_data;

    write_lock_.lock();

    while (result == RX_OK)
    {
        void* data = nullptr;
        size_t size = 0;
        if (!current_buffer_ && !sending_queue_.empty())
        {
            packet_data = sending_queue_.front();
            current_buffer_ = packet_data;
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
            result = rx_io_write(&dispatcher_data_, data, size);
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
bool serial_comm<buffT>::read_loop ()
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
        result = rx_io_read(&dispatcher_data_, &bytes);
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
                if (!readed(dispatcher_data_.read_buffer, bytes, get_identity()))
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
bool serial_comm<buffT>::on_startup (rx_thread_handle_t destination)
{
    return true;
}

template <class buffT>
void serial_comm<buffT>::on_shutdown (rx_security_handle_t identity)
{
}

template <class buffT>
void serial_comm<buffT>::initiate_shutdown ()
{
    internal_shutdown_callback(255);
}

template <class buffT>
bool serial_comm<buffT>::write (buffer_ptr what)
{
    if (what && what->empty())
        return true;

    buffer_ptr entry;
    entry = what;
    write_lock_.lock();
    sending_queue_.push(entry);
    write_lock_.unlock();
    write_loop();
    return true;
}

template <class buffT>
bool serial_comm<buffT>::start_loops ()
{
    return read_loop();
}


} // namespace serial
} // namespace interfaces
} // namespace rx_internal



#endif
