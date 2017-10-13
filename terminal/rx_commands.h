

/****************************************************************************
*
*  terminal\rx_commands.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef rx_commands_h
#define rx_commands_h 1



// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_objbase
#include "system/meta/rx_objbase.h"

using namespace server::prog;
using namespace server; 
using namespace server::ns;
using namespace rx;


namespace terminal {
namespace commands
{
class server_command;
}
typedef rx::pointers::reference<commands::server_command> server_command_ptr;

namespace commands {





class server_command : public server::prog::server_command_base, 
                       	public rx::poin
{
	DECLARE_REFERENCE_PTR(server_command);

  public:
      server_command (const string_type& console_name);

      virtual ~server_command();


      namespace_item_attributes get_attributes () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;


  protected:

      void virtual_bind ();

      void virtual_release ();


  private:


};






class server_command_manager : public server::objects::server_object  
{
	DECLARE_REFERENCE_PTR(server_command_manager);
	DECLARE_CODE_INFO("rx", "0.5.0", "\
server commands managment object\r\n\
bin folder in file hierarcyh\
")
	typedef std::map<string_type,server_command_base_ptr> registred_commands_type;

  public:
      server_command_manager();

      virtual ~server_command_manager();


      void register_command (server_command_base_ptr cmd);

      void register_internal_commands ();

      server_command_base_ptr get_command_by_name (const string_type& name);

      static server_command_manager::smart_ptr instance ();

      namespace_item_attributes get_attributes () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void get_commands (std::vector<command_ptr>& sub_items) const;

      bool get_help (std::ostream& out, std::ostream& err);


  protected:

  private:


      registred_commands_type _registred_commands;


};






class echo_server_command : public server_command  
{
	DECLARE_REFERENCE_PTR(echo_server_command);
	DECLARE_CODE_INFO("rx", "0.5.0", "\
basic echo class intendend for console usage\r\n\
displays string behid on console\r\n\
acctualiy first command class good for testing.)");

  public:
      echo_server_command();

      virtual ~echo_server_command();


      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};


} // namespace commands
} // namespace terminal



#endif