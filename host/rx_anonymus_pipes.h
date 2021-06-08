

/****************************************************************************
*
*  host\rx_anonymus_pipes.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_anonymus_pipes_h
#define rx_anonymus_pipes_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"

// rx_objbase
#include "system/runtime/rx_objbase.h"
// dummy
#include "dummy.h"
// rx_thread
#include "lib/rx_thread.h"

namespace host {
namespace pipe {
class anonymus_pipe_client;
class local_pipe_port;
class rx_pipe_host;

} // namespace pipe
} // namespace host


#define RX_PIPE_BUFFER_SIZE 0x10000 //64 KiB for pipes


namespace host {

namespace pipe {





class anonymus_pipe_client 
{
	typedef memory::page_aligned_buffer pipe_buffer_type;

  public:
      anonymus_pipe_client (const pipe_client_t& pipes);

      ~anonymus_pipe_client();


      rx_result write_pipe (const rx_packet_buffer* buffer);

      rx_result read_pipe (rx_const_packet_buffer* buffer);

      void close_pipe ();

	  anonymus_pipe_client(const anonymus_pipe_client&) = delete;
	  anonymus_pipe_client(anonymus_pipe_client&&) = delete;
	  anonymus_pipe_client& operator=(const anonymus_pipe_client&) = delete;
	  anonymus_pipe_client& operator=(anonymus_pipe_client&&) = delete;
  protected:

  private:


      pipe_client_t handles_;

      pipe_buffer_type buffer_;


};






class anonymus_pipe_endpoint 
{

  public:
      anonymus_pipe_endpoint (rx_pipe_host* host, local_pipe_port* my_port);

      ~anonymus_pipe_endpoint();


      void receive_loop (std::function<void(int64_t)> received_func);

      rx_result open (const pipe_client_t& pipes, std::function<void(int64_t)> sent_func);

      void close ();


      bool binded;


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      std::unique_ptr<anonymus_pipe_client> pipes_;

      rx::threads::physical_job_thread pipe_sender_;

      rx_protocol_stack_endpoint stack_entry_;

      rx_pipe_host *host_;

      local_pipe_port *my_port_;


      std::function<void(int64_t)> sent_func_;

      friend class local_pipe_port;
};






class local_pipe_port : public rx_platform::runtime::items::port_runtime  
{
	DECLARE_CODE_INFO("rx", 0, 0, 1, "\
Local Pipe class. implementation of an local pipe port");

	DECLARE_REFERENCE_PTR(local_pipe_port);

  public:
      local_pipe_port (const pipe_client_t& pipes, rx_pipe_host* host);


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result receive_loop (rx_pipe_host* host);

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      void destroy_endpoint (rx_protocol_stack_endpoint* what);

      rx_result stop_passive ();

      void remove_endpoint(rx_protocol_stack_endpoint* what)
      {

      }
  protected:

  private:


      anonymus_pipe_endpoint pipes_;


      pipe_client_t pipe_handles_;

      std::atomic<bool> active_;


};


} // namespace pipe
} // namespace host



#endif
