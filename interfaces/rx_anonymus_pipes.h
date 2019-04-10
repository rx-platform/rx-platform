

/****************************************************************************
*
*  interfaces\rx_anonymus_pipes.h
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


#ifndef rx_anonymus_pipes_h
#define rx_anonymus_pipes_h 1


#include "protocols/ansi_c/common_c/rx_protocol_base.h"

// dummy
#include "dummy.h"
// rx_thread
#include "lib/rx_thread.h"
// rx_endpoints
#include "interfaces/rx_endpoints.h"

namespace interfaces {
namespace anonymus_pipe {
class anonymus_pipe_client;

} // namespace anonymus_pipe
} // namespace interfaces


#define RX_PIPE_BUFFER_SIZE 0x10000 //64 KiB for pipes


namespace interfaces {

namespace anonymus_pipe {





class anonymus_pipe_client 
{
	typedef memory::page_aligned_buffer pipe_buffer_type;

  public:
      anonymus_pipe_client (const pipe_client_t& pipes);

      ~anonymus_pipe_client();


      rx_result write_pipe (const rx_packet_buffer* buffer);

      rx_result read_pipe (rx_const_packet_buffer* buffer);

      rx_result close_pipe ();

	  anonymus_pipe_client(const anonymus_pipe_client&) = delete;
	  anonymus_pipe_client(anonymus_pipe_client&&) = delete;
	  anonymus_pipe_client& operator=(const anonymus_pipe_client&) = delete;
	  anonymus_pipe_client& operator=(anonymus_pipe_client&&) = delete;
  protected:

  private:


      pipe_client_t handles_;

      pipe_buffer_type buffer_;


};







class anonymus_pipe_endpoint : public rx_protocol_stack_entry  
{

  public:
      anonymus_pipe_endpoint();


      void receive_loop ();

      rx_result open (const pipe_client_t& pipes);

      rx_result close ();


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer);



      std::unique_ptr<anonymus_pipe_client> pipes_;

      rx::threads::physical_job_thread pipe_sender_;


};


} // namespace anonymus_pipe
} // namespace interfaces



#endif
