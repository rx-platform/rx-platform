

/****************************************************************************
*
*  host\rx_anonymus_pipes.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
// rx_security
#include "lib/security/rx_security.h"
// rx_thread
#include "lib/rx_thread.h"
// rx_port_types
#include "system/runtime/rx_port_types.h"
// rx_endpoints
#include "interfaces/rx_endpoints.h"

namespace host {
namespace pipe {
class anonymus_pipe_client;

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







class anonymus_pipe_endpoint : public rx_protocol_stack_entry  
{

  public:
      anonymus_pipe_endpoint();


      void receive_loop (std::function<void(int64_t)> received_func);

      rx_result open (const pipe_client_t& pipes, std::function<void(int64_t)> sent_func);

      void close ();


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_packet_buffer* buffer);



      std::unique_ptr<anonymus_pipe_client> pipes_;

      rx::threads::physical_job_thread pipe_sender_;


      std::function<void(int64_t)> sent_func_;


};






class local_pipe_port : public rx_platform::runtime::io_types::physical_port  
{
	DECLARE_CODE_INFO("rx", 0, 0, 1, "\
Local Pipe class. implementation of an local pipe port port");

	DECLARE_REFERENCE_PTR(local_pipe_port);

  public:
      local_pipe_port (const pipe_client_t& pipes);


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      rx_result start_runtime (runtime::runtime_start_context& ctx);

      rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void receive_loop ();

      rx_result open ();

      void close ();

      rx_protocol_stack_entry* get_stack_entry ();


  protected:

  private:


      anonymus_pipe_endpoint pipes_;


      pipe_client_t pipe_handles_;


};






class local_pipe_security_context : public rx::security::built_in_security_context  
{
    DECLARE_REFERENCE_PTR(local_pipe_security_context);

  public:
      local_pipe_security_context();

      ~local_pipe_security_context();


      bool is_system () const;


  protected:

  private:


};


} // namespace pipe
} // namespace host



#endif
