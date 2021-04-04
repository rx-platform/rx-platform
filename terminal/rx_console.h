

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

// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// dummy
#include "dummy.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_mem
#include "lib/rx_mem.h"
// sl_script
#include "soft_logic/sl_script.h"

namespace rx_internal {
namespace terminal {
namespace console {
class console_program_context;
class console_runtime;

} // namespace console
} // namespace terminal
} // namespace rx_internal




namespace rx_internal {

namespace terminal {

namespace console {
typedef pointers::reference<console_runtime> console_runtime_ptr;






class console_program_context : public sl_runtime::sl_script::script_program_context  
{
public:
	typedef console_program_context* smart_ptr;
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
	typedef std::map<size_t, rx_struct_ptr > instructions_data_type;
	typedef int pending_jobs_type;

  public:
      console_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_runtime> client);

      ~console_program_context();


      std::ostream& get_stdout ();

      std::ostream& get_stderr ();

      void send_results (bool result);

      bool is_postponed () const;

      bool postpone (uint32_t interval);

      void set_instruction_data (rx_struct_ptr data);

      bool is_canceled ();

      void postpone_done ();

      void set_waiting ();

      void cancel_execution ();

      api::rx_context create_api_context ();

      bool one_more_time ();

      bool should_next_line ();

      rx_thread_handle_t get_executer () const;


      rx_reference<console_runtime> get_client ()
      {
        return client_;
      }



      rx_directory_ptr get_current_directory ()
      {
        return current_directory_;
      }

      void set_current_directory (rx_directory_ptr value)
      {
        current_directory_ = value;
      }


      const buffer_ptr get_out () const
      {
        return out_;
      }


      const buffer_ptr get_err () const
      {
        return err_;
      }


	  template<typename T>
	  pointers::reference<T> get_instruction_data()
	  {
		  auto it = instructions_data_.find(get_possition());
		  if (it != instructions_data_.end())
		  {
			  return it->second.cast_to<pointers::reference<T> >();
		  }
		  else
		  {
			  return pointers::reference<T>::null_ptr;
		  }
	  }
  protected:

      size_t get_possition () const;


  private:


      rx_reference<console_runtime> client_;


      std::ostream out_std_;

      std::ostream err_std_;

      rx_directory_ptr current_directory_;

      buffer_ptr out_;

      buffer_ptr err_;

      std::atomic<int> postponed_;

      instructions_data_type instructions_data_;

      std::atomic_bool canceled_;

      bool one_more_time_;


};







class console_program : public sl_runtime::sl_script::sl_script_program  
{
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      console_program();

      ~console_program();


      sl_runtime::program_context* create_program_context (sl_runtime::program_context* parent_context, sl_runtime::sl_program_holder* holder);


  protected:

      bool parse_line (const string_type& line, std::ostream& out, std::ostream& err, sl_runtime::program_context* context);


  private:


};






class console_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(console_runtime)

  public:
      console_runtime();

      console_runtime (runtime::items::port_runtime* port);

      ~console_runtime();


      const string_type& get_console_terminal ();

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer);

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

      bool do_command (string_type&& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void get_prompt (string_type& prompt);

      void get_wellcome (string_type& wellcome);

      bool cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void get_security_error (string_type& txt, sec_error_num_t err_number);

      bool do_commands (string_array&& lines, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


  private:

      void synchronized_do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void synchronized_cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t connected_function (rx_protocol_stack_endpoint* reference, rx_session* session);


      rx_directory_ptr current_directory_;



      console_program_context *current_context_;

      console_program program_;

      rx_protocol_stack_endpoint stack_entry_;


      string_type line_;

      string_type name_;

      sl_runtime::sl_program_holder program_holder_;

      rx_thread_handle_t executer_;

      runtime::items::port_runtime* port_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< console_runtime  > console_port_base;






class console_port : public console_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 3, "\
Console port. implementation of an console port");

    DECLARE_REFERENCE_PTR(console_port);


  public:
      console_port();


      void stack_assembled ();


  protected:

  private:


};


} // namespace console
} // namespace terminal
} // namespace rx_internal



#endif
