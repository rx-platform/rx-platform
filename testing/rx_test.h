

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



// rx_security
#include "lib/security/rx_security.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_commands
#include "terminal/rx_commands.h"
// rx_ns
#include "system/server/rx_ns.h"

using namespace rx;

/////////////////////////////////////////////////////////////
// logging macros for testing library
#define TEST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Test",src,lvl,msg)
#define TEST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Test",src,lvl,msg)
#define TEST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Test",src,lvl,msg)
#define TEST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Test",src,lvl,msg)
#define TEST_LOG_TRACE(src,lvl,msg) RX_TRACE("Test",src,lvl,msg)


namespace testing {





class test_command : public terminal::commands::server_command, 
                     	public rx::secu
{
	DECLARE_REFERENCE_PTR(test_command);
	DECLARE_DERIVED_FROM_INTERFACE;
	DECLARE_CODE_INFO("rx", 0,1,0, "\
class intendend for console or script usage\r\n\
responsable of executing the test cases for rx-platform or for your plugis.\r\n\
test cases are devided into several categories. you can use test command to explore this test cases\
");


  public:
      test_command();

      virtual ~test_command();


      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};






class test_case : public rx::pointers::reference_object, 
                  	public server::n
{
	DECLARE_REFERENCE_PTR(test_case);
	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;

	DECLARE_CODE_INFO("rx", 0,5,0, "\
class intendend for console or script usage\r\n\
all about doing stuff with security");
public:

  public:
      test_case (const string_type& name);

      virtual ~test_case();


      virtual bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx) = 0;

      bool test_start (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      void test_end (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      void get_value (values::rx_value& val) const;

      namespace_item_attributes get_attributes () const;

      const string_type& get_item_name () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;


      const string_type& get_name () const
      {
        return _name;
      }



  protected:

  private:
      test_case(const test_case &right);

      test_case & operator=(const test_case &right);



      string_type _name;

      uint64_t _start_tick;


};






class test_category 
{
	typedef std::map<string_type, test_case::smart_ptr> cases_type;
public:
	typedef std::unique_ptr<test_category> smart_ptr;

  public:
      test_category (const string_type& category);

      virtual ~test_category();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx, bool code);

      void collect_test_cases (std::vector<rx_server_item::smart_ptr>& cases);


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






class testing_enviroment 
{

public:
	typedef std::map<string_type, test_category::smart_ptr> registered_tests_type;

  public:
      virtual ~testing_enviroment();


      static testing_enviroment& instance ();

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      void collect_test_cases (std::vector<rx_server_item::smart_ptr>& cases);


  protected:

  private:
      testing_enviroment();


      void register_code_test (test_category::smart_ptr test);



      registered_tests_type _registered_tests;


};






class basic_test_case_test : public test_case  
{
	DECLARE_REFERENCE_PTR(basic_test_case_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
This test creates dummy test case and is used for testing this mechanism\
");

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
