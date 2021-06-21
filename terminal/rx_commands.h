

/****************************************************************************
*
*  terminal\rx_commands.h
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


#ifndef rx_commands_h
#define rx_commands_h 1



// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_logic
#include "system/logic/rx_logic.h"

namespace rx_internal {
namespace terminal {
namespace console {
namespace script {
class console_program_context;

} // namespace script
} // namespace console
} // namespace terminal
} // namespace rx_internal


// adding command line parsing library
// see <https://github.com/jarro2783/cxxopts>
#include "third-party/cxxopts/include/cxxopts.hpp"
#include "terminal/rx_terminal_style.h"
using namespace rx_platform; 
using namespace rx_platform::ns;
using namespace rx;


namespace rx_internal {

template<typename valType>
void rx_dump_value(const valType& value, std::ostream& out, bool colorized)
{
    if (colorized)
    {
        if (value.is_good())
            out << (value.is_test() ? ANSI_RX_TEST_COLOR :  ANSI_RX_GOOD_COLOR);
        else if (value.is_uncertain())
            out << ANSI_RX_UNCERTAIN_COLOR;
        else
            out << ANSI_RX_BAD_COLOR;
    }
    value.dump_to_stream(out);
    if (colorized)
        out << ANSI_COLOR_RESET;
}

namespace terminal {
namespace commands
{
class server_command;
}
namespace console
{
namespace script
{
class console_program_context;
}
}
typedef console::script::console_program_context* console_context_ptr;
typedef rx::pointers::reference<commands::server_command> server_command_ptr;

namespace commands {
typedef pointers::reference<server_command> command_ptr;

using rx_platform::ns::suggestions_type;





class server_command : public rx_platform::logic::program_runtime  
{
	DECLARE_REFERENCE_PTR(server_command);

  public:
      server_command (const string_type& name);

      ~server_command();


      string_type get_type_name () const;

      values::rx_value get_value () const;

      namespace_item_attributes get_attributes () const;

      bool console_execute (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);

      rx_time get_created_time () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      string_type get_name () const;

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);

      virtual string_type get_help () const;

      virtual void register_suggestions (const string_type& line, suggestions_type& suggestions);


      const string_type& get_console_name () const
      {
        return console_name_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }



  protected:

      virtual bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx) = 0;

      bool dword_check_premissions (security::security_mask_t mask, security::extended_security_mask_t extended_mask);


      rx_time time_stamp_;


  private:


      string_type console_name_;

      security::security_guard_ptr security_guard_;

      rx_time modified_time_;


};






class server_command_manager : public rx_platform::runtime::items::object_runtime  
{
	DECLARE_REFERENCE_PTR(server_command_manager);
	DECLARE_CODE_INFO("rx", 0, 5, 0, "\
server commands managment object\r\n\
bin folder in file hierarcyh\
")
	typedef std::map<string_type, server_command_base_ptr > registered_commands_type;

  public:
      server_command_manager();

      ~server_command_manager();


      void register_command (server_command_base_ptr cmd);

      void register_internal_commands ();

      server_command_base_ptr get_command_by_name (const string_type& name);

      static server_command_manager::smart_ptr instance ();

      namespace_item_attributes get_attributes () const;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      void get_commands (std::vector<command_ptr>& sub_items) const;

      void register_suggestions (const string_type& line, suggestions_type& suggestions);

      void clear ();


  protected:

  private:


      registered_commands_type registered_commands_;


      locks::lockable lock_;

      registered_commands_type other_commands_;
};






class echo_server_command : public server_command  
{
	DECLARE_REFERENCE_PTR(echo_server_command);
	DECLARE_CONSOLE_CODE_INFO(0, 5, 0, "\
displays string behid on console\r\n\
acctualiy first command class good for testing.)");

  public:
      echo_server_command();

      ~echo_server_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};


} // namespace commands
} // namespace terminal
} // namespace rx_internal



#endif
