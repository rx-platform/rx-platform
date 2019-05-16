

/****************************************************************************
*
*  runtime_internal\rx_runtime_commands.h
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


#ifndef rx_runtime_commands_h
#define rx_runtime_commands_h 1



// rx_commands
#include "terminal/rx_commands.h"

#include "terminal/rx_con_commands.h"
using terminal::console::console_commands::console_program_contex_ptr;


namespace sys_runtime {

namespace runtime_commands {





class read_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(read_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for reading values from various items");

  public:
      read_command();

      ~read_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class pull_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(pull_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for pulling values from various items");

  public:
      pull_command();

      ~pull_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class write_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(write_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for writing values to various items");
	struct write_data_t : public pointers::struct_reference
	{
		uint64_t started;
	};

  public:
      write_command();

      ~write_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};


} // namespace runtime_commands
} // namespace sys_runtime



#endif
