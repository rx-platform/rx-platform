

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
// rx_cmds
#include "system/server/rx_cmds.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_security
#include "lib/security/rx_security.h"
// rx_ptr
#include "lib/rx_ptr.h"

using namespace rx;

/////////////////////////////////////////////////////////////
// logging macros for testing library
#define TEST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Test",src,lvl,msg)
#define TEST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Test",src,lvl,msg)
#define TEST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Test",src,lvl,msg)
#define TEST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Test",src,lvl,msg)
#define TEST_LOG_TRACE(src,lvl,msg) RX_TRACE("Test",src,lvl,msg)


namespace testing {
typedef uint_fast8_t test_status_t;





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

      bool do_info_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      bool do_run_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      bool do_status_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      bool do_list_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};


struct test_context_data
{
	rx_time time_stamp;
	string_type user;
};




class test_program_context : public server::prog::program_context_base  
{
	DECLARE_REFERENCE_PTR(test_program_context);

  public:
      test_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err);

      virtual ~test_program_context();


      void set_failed ();

      void set_passed ();


      const test_status_t get_status () const
      {
        return _status;
      }


      const test_context_data& get_data () const
      {
        return _data;
      }



  protected:

  private:

      void fill_data ();



      test_status_t _status;

      test_context_data _data;


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


      virtual bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx) = 0;

      bool test_start (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);

      void test_end (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      void get_value (values::rx_value& val) const;

      namespace_item_attributes get_attributes () const;

      const string_type& get_item_name () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      test_status_t get_status (test_context_data* data = nullptr);

      test_context_data get_data (test_context_data* data = nullptr) const;

      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


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

      test_status_t _status;

      test_context_data _data;

      locks::slim_lock _status_lock;


};






class test_category 
{
	typedef std::map<string_type, test_case::smart_ptr> cases_type;
public:
	typedef std::unique_ptr<test_category> smart_ptr;

  public:
      test_category (const string_type& category);

      virtual ~test_category();


      void collect_test_cases (std::vector<test_case::smart_ptr>& cases);

      void get_cases (string_array& cases);

      test_case::smart_ptr get_test_case (const string_type& test_name);


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
	typedef std::map<string_type, test_category::smart_ptr> categories_type;

  public:
      virtual ~testing_enviroment();


      static testing_enviroment& instance ();

      void collect_test_cases (const string_type& category, std::vector<test_case::smart_ptr>& cases);

      void get_categories (string_array& categories);

      void get_cases (const string_type& category, string_array& cases);

      test_case::smart_ptr get_test_case (const string_type& test_name);

      test_program_context::smart_ptr create_test_context (server::prog::console_program_context::smart_ptr console_ctx);


  protected:

  private:
      testing_enviroment();


      void register_code_test (test_category::smart_ptr test);



      categories_type _categories;


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


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


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
