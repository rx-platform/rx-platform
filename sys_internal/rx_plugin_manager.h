

/****************************************************************************
*
*  sys_internal\rx_plugin_manager.h
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


#ifndef rx_plugin_manager_h
#define rx_plugin_manager_h 1



// rx_plugin
#include "system/libraries/rx_plugin.h"
// rx_commands
#include "terminal/rx_commands.h"

using rx_internal::terminal::console_context_ptr;


namespace rx_internal {

namespace plugins {

typedef std::vector<rx_platform::library::rx_plugin_base*> plugins_type;




class plugins_manager 
{
	typedef std::set<rx::pointers::code_behind_definition_t*> definitions_type;

  public:
      virtual ~plugins_manager();


      static plugins_manager& instance ();

      bool check_class (rx::pointers::code_behind_definition_t* cd);

      rx_result register_plugin (rx_platform::library::rx_plugin_base* what);

      void deinitialize ();


      plugins_type& get_plugins ()
      {
        return plugins_;
      }


	  template<class clsT>
	  rx::pointers::code_behind_definition_t* _internal_read_class(tl::type2type<clsT>)
	  {
		  return clsT::get_code_behind();
	  }
	  template<class clsT>
	  rx::pointers::code_behind_definition_t* register_class_for_usage()
	  {
		  rx::pointers::code_behind_definition_t* cd = _internal_read_class<clsT>(tl::type2type<clsT>());
		  auto& it = this->definitions_.find(cd);
		  if (it == this->definitions_.end())
		  {
			  this->definitions_.insert(cd);
		  }
		  return cd;

	  }
  protected:

  private:
      plugins_manager();

      plugins_manager(const plugins_manager &right);

      plugins_manager & operator=(const plugins_manager &right);



      plugins_type plugins_;


      definitions_type definitions_;


};






class plugin_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(plugin_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
command for working with platform plug-ins");

  public:
      plugin_command();

      ~plugin_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx);


  private:


};


} // namespace plugins
} // namespace rx_internal



#endif
