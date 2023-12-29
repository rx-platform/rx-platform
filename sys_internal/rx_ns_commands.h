

/****************************************************************************
*
*  sys_internal\rx_ns_commands.h
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


#ifndef rx_ns_commands_h
#define rx_ns_commands_h 1



// rx_commands
#include "terminal/rx_commands.h"

using rx_internal::terminal::console_context_ptr;


namespace rx_internal {

namespace internal_ns {

namespace namespace_commands {





class cd_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(cd_command);
	DECLARE_CONSOLE_CODE_INFO(0, 5, 0, "\
changes current active directory");

  public:
      cd_command();

      ~cd_command();


      virtual void register_suggestions (const string_type& line, suggestions_type& suggestions);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};


struct term_list_item_options
{
	bool list_attributes;
	bool list_qualities;
	bool list_timestamps;
	bool list_created;
	bool list_type;
	bool list_size;
};




class list_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(list_command);
	DECLARE_CONSOLE_CODE_INFO(0, 5, 0, "\
list current directory on console ( dir, ls ");

  public:
      list_command (const string_type& console_name);

      ~list_command();


      bool list_directory (std::ostream& out, std::ostream& err, const string_type& filter, const term_list_item_options& options, rx_directory_ptr directory);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);

      bool do_non_list_command (std::ostream& out, std::ostream& err, const string_type& filter, const term_list_item_options& options, rx_directory_ptr directory, console_context_ptr ctx);


  private:


};






class dir_command : public list_command  
{
	DECLARE_REFERENCE_PTR(dir_command);

  public:
      dir_command();

      ~dir_command();


  protected:

  private:


};






class ls_command : public list_command  
{
	DECLARE_REFERENCE_PTR(ls_command);

  public:
      ls_command();

      ~ls_command();


  protected:

  private:


};






class mkdir_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(mkdir_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
creates new directory in namespace");

  public:
      mkdir_command();

      ~mkdir_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class rmdir_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(rmdir_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
removes directory from namespace");

  public:
      rmdir_command();

      ~rmdir_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class clone_system_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(clone_system_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for cloning system types and objects of this rx-platform instance\r\n\
\
");

  public:
      clone_system_command();

      ~clone_system_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class move_command : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(move_command);
    DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for moving (or renaming if you like?) a directories of this rx-platform instance\r\n\
\
");

  public:
      move_command();

      ~move_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};






class clone_command : public terminal::commands::server_command  
{
    DECLARE_REFERENCE_PTR(clone_command);
    DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
    command for making copies of directories of this rx-platform instance\r\n\
\
");

  public:
      clone_command();

      ~clone_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};


} // namespace namespace_commands
} // namespace internal_ns
} // namespace rx_internal



#endif
