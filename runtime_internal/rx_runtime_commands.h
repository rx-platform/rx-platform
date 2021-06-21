

/****************************************************************************
*
*  runtime_internal\rx_runtime_commands.h
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


#ifndef rx_runtime_commands_h
#define rx_runtime_commands_h 1



// rx_commands
#include "terminal/rx_commands.h"

using rx_internal::terminal::console_context_ptr;


namespace rx_internal {

namespace sys_runtime {

namespace runtime_commands {





class pull_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(pull_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for pulling values from various items");

  public:
      pull_command();

      ~pull_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class turn_on_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(turn_on_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for turning on any object, domain, port or application");

  public:
      turn_on_command();

      ~turn_on_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class turn_off_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(turn_off_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for turning off any object, domain, port or application");

  public:
      turn_off_command();

      ~turn_off_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class runtime_command_base : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(runtime_command_base);

  public:
      runtime_command_base (const string_type& name);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);

      virtual bool do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err, rx_thread_handle_t executer) = 0;


  private:


};






class read_command : public runtime_command_base  
{
	DECLARE_REFERENCE_PTR(read_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for reading values from various items");

  public:
      read_command();

      ~read_command();


  protected:

      bool do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err, rx_thread_handle_t executer);


  private:


};






class write_command : public runtime_command_base  
{
	DECLARE_REFERENCE_PTR(write_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for writing values to various items");
	

  public:
      write_command();

      ~write_command();


  protected:

      bool do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err, rx_thread_handle_t executer);


  private:


};






class browse_command : public runtime_command_base  
{
	DECLARE_REFERENCE_PTR(browse_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for browsing inside of object, domain, port or application");

  public:
      browse_command();

      ~browse_command();


  protected:

      bool do_with_item (platform_item_ptr&& rt_item, string_type sub_item, rx_simple_value&& value, console_context_ptr ctx, std::ostream& out, std::ostream& err, rx_thread_handle_t executer);


  private:


};


} // namespace runtime_commands
} // namespace sys_runtime
} // namespace rx_internal



#endif
