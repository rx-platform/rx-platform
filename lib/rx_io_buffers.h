

/****************************************************************************
*
*  lib\rx_io_buffers.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_io_buffers_h
#define rx_io_buffers_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "rx_lib.h"

// dummy
#include "dummy.h"



namespace rx {

namespace io {





class rx_const_io_buffer 
{

  public:
      rx_const_io_buffer (rx_const_packet_buffer* buffer);

      rx_const_io_buffer (rx_packet_buffer* buffer);


      rx_result read_string (string_type& val);

      rx_result read_chars (string_type& val);

      static rx_const_packet_buffer create_from_chars (const string_type& str);

      rx_const_packet_buffer* c_buffer ();

	  template<typename T>
	  rx_result read_from_buffer(T& val)
	  {
          static_assert(std::is_trivial<T>::value);

          rx_protocol_result_t result;
		  val = *((T*)rx_get_from_packet(buffer_, sizeof(T), &result));

		  if (result == RX_PROTOCOL_OK)
			  return true;
		  else
			  return rx_protocol_error_message(result);
	  }
  protected:

  private:


      rx_const_packet_buffer* buffer_;


};






class rx_io_buffer : public rx_packet_buffer  
{

  public:
      rx_io_buffer();

      rx_io_buffer (size_t initial_capacity, size_t initial_front_capacity);

      ~rx_io_buffer();


      bool valid () const;

      void attach (rx_packet_buffer* buffer);

      void detach (rx_packet_buffer* buffer);

      void release ();

      operator bool () const;

      rx_result write_string (const string_type& val);

      rx_result write_chars (const string_type& val);

      rx_result write (const void* data, size_t size);

      bool empty () const;

      void reinit ();

	  // disable copy semantics
	  rx_io_buffer(const rx_io_buffer&) = delete;
	  rx_io_buffer& operator=(const rx_io_buffer&) = delete;
	  // enable move semantics
	  rx_io_buffer(rx_io_buffer&& right) noexcept;
	  rx_io_buffer& operator=(rx_io_buffer&& right) noexcept;

	  template<typename T>
	  rx_result write_to_buffer(const T& val)
	  {
          static_assert(std::is_trivial<T>::value);

		  rx_protocol_result_t result;

		  result = rx_push_to_packet(this, &val, sizeof(val));

		  if (result == RX_PROTOCOL_OK)
			  return true;
		  else
			  return rx_protocol_error_message(result);
	  }

      template<typename T>
      T* alloc_from_buffer()
      {
          static_assert(std::is_trivial<T>::value);

          rx_protocol_result_t result;

          T* ptr = (T*)rx_alloc_from_packet(this, sizeof(T), &result);

          if (result == RX_PROTOCOL_OK)
              return ptr;
          else
              return nullptr;
      }
  protected:

  private:

      void zero_memory ();



};


} // namespace io
} // namespace rx



#endif
