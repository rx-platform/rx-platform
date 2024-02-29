

/****************************************************************************
*
*  interfaces\rx_io.h
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


#ifndef rx_io_h
#define rx_io_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"

// rx_thread
#include "system/threads/rx_thread.h"
// rx_commands
#include "terminal/rx_commands.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_internal {
namespace interfaces {
namespace io_endpoints {
class dispatcher_subscriber;

} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal


#include "lib/rx_io_addr.h"


using rx_internal::terminal::console_context_ptr;


namespace rx_internal {

namespace interfaces {

namespace io_endpoints {
typedef std::set<dispatcher_subscriber> dispatcher_subscriber_ptr;
typedef std::set<rx::pointers::reference<dispatcher_subscriber> > time_aware_subscribers_type;






class dispatcher_subscriber : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(dispatcher_subscriber);

  public:
      dispatcher_subscriber (rx_security_handle_t identity = 0);

      virtual ~dispatcher_subscriber();


      static void propagate_timer ();

      virtual void timer_tick (rx_timer_ticks_t tick);

      void set_identity (rx_security_handle_t identity);

      static void deinitialize ();


      const rx_security_handle_t get_identity () const
      {
        return identity_;
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

      virtual int internal_connect_callback (sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status);

      int _internal_connect_callback (sockaddr_in* addr, sockaddr_in* local_addr, uint32_t status);

      virtual int internal_read_from_callback (size_t count, uint32_t status, struct sockaddr* addr, size_t addrsize);

      int _internal_read_from_callback (size_t count, uint32_t status, struct sockaddr* addr, size_t addrsize);



      static time_aware_subscribers_type time_aware_subscribers_;


      static locks::lockable time_aware_subscribers_lock_;

      rx_kernel_dispather_t dispatcher_handle_;

      rx_security_handle_t identity_;

	  friend int dispatcher_read_callback(void* data, uint32_t status, size_t size);
      friend int dispatcher_read_from_callback(void* data, uint32_t status, size_t size, struct sockaddr* addr, size_t addrsize);
	  friend int dispatcher_write_callback(void* data, uint32_t status);
	  friend int dispatcher_connect_callback(void* data, uint32_t status, struct sockaddr* addr, struct sockaddr* local_addr);
	  friend int dispatcher_shutdown_callback(void* data, uint32_t status);
	  friend int dispatcher_accept_callback(void* data, uint32_t status, sys_handle_t handle, struct sockaddr* addr, struct sockaddr* local_addr);
};






template <class headerT, class bufferT>
class stream_chuks_decoder 
{

  public:
      stream_chuks_decoder (std::function<bool(const bufferT&)> callback);


      bool push_bytes (const void* data, size_t count);


  protected:

  private:


      std::function<bool(const bufferT&)> chunk_callback_;

      bufferT receive_buffer_;

      headerT* header_;

      uint8_t* temp_byte_header_;

      int collected_header_;

      uint32_t collected_;

      uint32_t expected_;

      headerT temp_header_;


};






class net_command : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(net_command);
    DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for querying status of network interfaces");

  public:
      net_command();

      ~net_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:

      bool do_eth_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);

      bool do_list_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);



};


// Parameterized Class rx_internal::interfaces::io_endpoints::stream_chuks_decoder 

template <class headerT, class bufferT>
stream_chuks_decoder<headerT,bufferT>::stream_chuks_decoder (std::function<bool(const bufferT&)> callback)
      : chunk_callback_(callback),
        header_(nullptr),
        collected_header_(0),
        collected_(0),
        expected_(0)
{
	temp_byte_header_ = (uint8_t*)&temp_header_;
}



template <class headerT, class bufferT>
bool stream_chuks_decoder<headerT,bufferT>::push_bytes (const void* data, size_t count)
{
	uint8_t* buffer = (uint8_t*)data;
	if (header_ == NULL)
	{
		int head_size = (collected_header_ + (int)count < (int)sizeof(headerT)) ? (int)count : (int)sizeof(headerT) - collected_header_;
		memcpy(&temp_byte_header_[collected_header_], buffer, head_size);

		collected_header_ += head_size;
		if (collected_header_ < (int)sizeof(headerT))
		{
			return true;
		}

		collected_header_ = 0;

		header_ = &temp_header_;

		if (!header_->check_header())
			return false;

		expected_ = (uint32_t)(header_->get_data_size() - sizeof(headerT));

		receive_buffer_.reinit();

		receive_buffer_.push_data(temp_byte_header_, sizeof(headerT));

		count = count - head_size;

		buffer += head_size;
	}
	collected_ += (uint32_t)count;

	bool should_break = false;
	bool should_continue = false;
	while (collected_ > expected_)
	{

		uint32_t usefull = (uint32_t)count - (collected_ - expected_);
		receive_buffer_.push_data(buffer, usefull);

		if (!chunk_callback_(receive_buffer_))
		{
			should_break = true;
			break;
		}

		count = count - usefull;
		collected_ = 0;

		buffer += usefull;

		header_ = NULL;

		int head_size = (collected_header_ + count < (int)sizeof(headerT)) ? (int)count : (int)sizeof(headerT) - collected_header_;
		assert(head_size <= (int)sizeof(headerT));
		memcpy(&temp_byte_header_[collected_header_], buffer, head_size);

		collected_header_ += head_size;
		if (collected_header_ < (int)sizeof(headerT))
		{
			should_continue = true;
			break;
		}

		collected_header_ = 0;

		header_ = &temp_header_;

		if (!header_->check_header())
		{
			should_break = true;
			break;
		}

		expected_ = (uint32_t)(header_->get_data_size() - sizeof(headerT));

		receive_buffer_.reinit();
		receive_buffer_.push_data(temp_byte_header_, sizeof(headerT));

		count = count - head_size;

		assert(((uint32_t)count) < 0x80000000);

		collected_ = (uint32_t)count;

		buffer += head_size;

	}
	if (should_break)
		return false;

	if (should_continue)
	{
		return true;
	}

	receive_buffer_.push_data(buffer, count);

	if (collected_ == expected_)
	{

		if (!chunk_callback_(receive_buffer_))
			return false;

		collected_ = 0;
		expected_ = 0;
		header_ = NULL;
	}
	return true;
}


} // namespace io_endpoints
} // namespace interfaces
} // namespace rx_internal



#endif
