

/****************************************************************************
*
*  model\rx_meta_commands.h
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_meta_commands_h
#define rx_meta_commands_h 1



// rx_commands
#include "terminal/rx_commands.h"

#include "model/rx_meta.h"


namespace model {

namespace meta_commands {
typedef rx_platform::prog::console_program_context::smart_ptr console_program_contex_ptr;





class create_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(create_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for creating various objects in platform\r\n\
\
");

  public:
      create_command();

      virtual ~create_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:

      bool create_object (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);

      bool create_type (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);



};






class dump_types_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(dump_types_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for dumping types data\r\n\
\
");

  public:
      dump_types_command();

      virtual ~dump_types_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:
	  template<typename T>
	  bool dump_types_to_console(model::type_hash<T>& hash, std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);
	  
	  template<typename T>
	  bool dump_types_recursive(rx_node_id start, int indent, model::type_hash<T>& hash, std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);

};


} // namespace meta_commands
} // namespace model



#endif
