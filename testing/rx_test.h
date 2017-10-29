

/****************************************************************************
*
*  testing\rx_test.h
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


#ifndef rx_test_h
#define rx_test_h 1



// rx_commands
#include "terminal/rx_commands.h"
// rx_security
#include "system/security/rx_security.h"

using namespace rx;


namespace testing {





class code_test 
{
public:
	typedef std::unique_ptr<code_test> smart_ptr;

  public:
      code_test (const string_type& category);

      virtual ~code_test();


      virtual bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx) = 0;


      const string_type& get_category () const
      {
        return _category;
      }



  protected:

  private:
      code_test(const code_test &right);

      code_test & operator=(const code_test &right);



      string_type _category;


};






class test_command : public terminal::commands::server_command, 
                     	public server::secu
{
	DECLARE_REFERENCE_PTR(test_command);

	DECLARE_DERIVED_FROM_INTERFACE;

	DECLARE_CODE_INFO("rx", "0.1.0", "\
class intendend for console or script usage\r\n\
this is command responsable of executing the code from test part of project");

public:
	typedef std::map<string_type, code_test::smart_ptr> registred_tests_type;

  public:
      test_command();

      virtual ~test_command();


      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:

      void register_code_test (code_test::smart_ptr test);



      registred_tests_type _registred_tests;


};


} // namespace testing



#endif
