

/****************************************************************************
*
*  terminal\rx_console.h
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


#ifndef rx_console_h
#define rx_console_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "interfaces/rx_endpoints.h"

// dummy
#include "dummy.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_con_programs
#include "terminal/rx_con_programs.h"
// soft_plc
#include "soft_logic/soft_plc.h"

namespace rx_internal {
namespace terminal {
namespace console {
class console_runtime_program_executer;
class console_runtime;

} // namespace console
} // namespace terminal
} // namespace rx_internal




namespace rx_internal {

namespace terminal {

namespace console {





class console_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(console_runtime)

  public:
      console_runtime();

      console_runtime (runtime::items::port_runtime* port);

      ~console_runtime();


      string_type get_console_terminal ();

      void process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done);

      static string_type get_terminal_info ();

      rx_result check_validity ();

      rx_protocol_stack_endpoint* bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      void close_endpoint ();


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }


      runtime::items::port_runtime* get_port ()
      {
        return port_;
      }



  protected:

      bool do_command (string_type&& line, security::security_context_ptr ctx);

      void get_prompt (string_type& prompt);

      void get_wellcome (string_type& wellcome);

      bool cancel_command (security::security_context_ptr ctx);

      void get_security_error (string_type& txt, sec_error_num_t err_number);

      bool do_commands (string_array&& lines, security::security_context_ptr ctx);


  private:

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done);

      void synchronized_do_command (const string_type& line, security::security_context_ptr ctx);

      void synchronized_cancel_command (security::security_context_ptr ctx);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);


      rx_directory_ptr current_directory_;



      memory::buffer_ptr create_buffer ();


      rx_protocol_stack_endpoint stack_entry_;

      std::unique_ptr<console_runtime_program_executer> program_executer_;

      script::console_program_context *program_context_;

      sl_runtime::sl_program_holder program_holder_;

      std::unique_ptr<sl_runtime::program_context> context_ownership_;


      string_type line_;

      string_type name_;

      rx_thread_handle_t executer_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< console_runtime  > console_port_base;






class console_port : public console_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Console port. implementation of an console port");

    DECLARE_REFERENCE_PTR(console_port);


  public:
      console_port();


      void stack_assembled ();


  protected:

  private:


};






class console_runtime_program_executer : public sl_runtime::program_executer  
{

  public:
      console_runtime_program_executer (sl_runtime::sl_program_holder* program, rx_reference<console_runtime> host, security::security_context_ptr sec_context);


      void start_program (uint32_t rate, std::unique_ptr<sl_runtime::program_context>&& context);

      std::unique_ptr<sl_runtime::program_context> stop_program ();

      void schedule_scan (uint32_t interval);


  protected:

  private:

      void do_scan ();



      rx_reference<console_runtime> host_;

      script::console_program_context *program_context_;


      security::security_context_ptr sec_context_;


};






class console_runtime_program_context : public script::console_program_context  
{

  public:
      console_runtime_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_runtime> runtime);

      console_runtime_program_context (console_runtime_program_context&& right);

      ~console_runtime_program_context();


      std::ostream& get_stdout ();

      std::ostream& get_stderr ();

      api::rx_context create_api_context ();


      const buffer_ptr get_out () const
      {
        return out_;
      }


      const buffer_ptr get_err () const
      {
        return err_;
      }



  protected:

  private:

      void send_results (bool result, bool done);



      rx_reference<console_runtime> host_;


      buffer_ptr out_;

      buffer_ptr err_;

      std::ostream out_std_;

      std::ostream err_std_;


};


} // namespace console
} // namespace terminal
} // namespace rx_internal



#endif
