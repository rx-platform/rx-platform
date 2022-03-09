

/****************************************************************************
*
*  terminal\rx_console.h
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


#ifndef rx_console_h
#define rx_console_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "interfaces/rx_endpoints.h"


/////////////////////////////////////////////////////////////
// logging macros for console library
#define CONSOLE_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Console",src,lvl,msg)
#define CONSOLE_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Console",src,lvl,msg)
#define CONSOLE_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Console",src,lvl,msg)
#define CONSOLE_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Console",src,lvl,msg)
#define CONSOLE_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Console",src,lvl,msg)
#define CONSOLE_LOG_TRACE(src,lvl,msg) RX_TRACE("Console",src,lvl,msg)

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

typedef std::function<void(bool, memory::buffer_ptr, memory::buffer_ptr, bool)> console_runtime_callback_t;




class console_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(console_runtime);

  public:
      console_runtime (rx_thread_handle_t executer, console_runtime_callback_t callback);

      ~console_runtime();


      void do_command (const string_type& line, security::security_context_ptr ctx);

      void process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done);

      bool cancel_command (security::security_context_ptr ctx);

      rx_result check_validity ();

      void reset ();

      void get_prompt (string_type& prompt);

      void set_terminal_size (int width, int height);


      const string_type get_current_directory () const
      {
        return current_directory_;
      }



      string_type current_directory_;


  protected:

  private:


      std::unique_ptr<console_runtime_program_executer> program_executer_;

      script::console_program_context *program_context_;

      sl_runtime::sl_program_holder program_holder_;

      std::unique_ptr<sl_runtime::program_context> context_ownership_;


      string_type line_;

      string_type name_;

      rx_thread_handle_t executer_;

      console_runtime_callback_t callback_;

      int term_width_;

      int term_height_;


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
      console_runtime_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, const string_type& current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_runtime> runtime);

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
