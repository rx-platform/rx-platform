

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

/////////////////////////////////////////////////////////////
// logging macros for testing library
#define TEST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Test",src,lvl,msg)
#define TEST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Test",src,lvl,msg)
#define TEST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Test",src,lvl,msg)
#define TEST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Test",src,lvl,msg)
#define TEST_LOG_TRACE(src,lvl,msg) RX_TRACE("Test",src,lvl,msg)


namespace testing {





class test_case 
{
public:
	typedef std::unique_ptr<test_case> smart_ptr;

  public:
      test_case (const string_type& name);

      virtual ~test_case();


      virtual bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx) = 0;

      bool test_start (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      void test_end (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


      const string_type& get_name () const
      {
        return _name;
      }



  protected:

  private:
      test_case(const test_case &right);

      test_case & operator=(const test_case &right);



      string_type _name;

      qword _start_tick;


};






class test_category 
{
	typedef std::map<string_type, test_case::smart_ptr> cases_type;
public:
	typedef std::unique_ptr<test_category> smart_ptr;

  public:
      test_category (const string_type& category);

      virtual ~test_category();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


      const string_type& get_category () const
      {
        return _category;
      }



  protected:

      void register_test_case (test_case::smart_ptr test);


  private:
      test_category(const test_category &right);

      test_category & operator=(const test_category &right);



      cases_type _cases;


      string_type _category;


};






class test_command : public terminal::commands::server_command, 
                     	public server::secu
{
	DECLARE_REFERENCE_PTR(test_command);
	DECLARE_DERIVED_FROM_INTERFACE;
	DECLARE_CODE_INFO("rx", "0.1.0", "\
class intendend for console or script usage\r\n\
responsable of executing the test cases for rx-platform or for your plugis.\r\n\
test cases are devided into several categories. you can use test command to explore this test cases\
");

public:
	typedef std::map<string_type, test_category::smart_ptr> registered_tests_type;

  public:
      test_command();

      virtual ~test_command();


      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:

      void register_code_test (test_category::smart_ptr test);



      registered_tests_type _registered_tests;


};






class basic_test_case_test : public test_case  
{

  public:
      basic_test_case_test();

      virtual ~basic_test_case_test();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};







class test_test : public test_category  
{

  public:
      test_test();

      virtual ~test_test();


  protected:

  private:


};


} // namespace testing



#endif
