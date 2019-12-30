

/****************************************************************************
*
*  testing\rx_test.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_test_h
#define rx_test_h 1



// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_console
#include "terminal/rx_console.h"
// rx_commands
#include "terminal/rx_commands.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace testing {
class test_case;

} // namespace testing


using namespace rx;

/////////////////////////////////////////////////////////////
// logging macros for testing library
#define TEST_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Test",src,lvl,msg)
#define TEST_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Test",src,lvl,msg)
#define TEST_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Test",src,lvl,msg)
#define TEST_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("Test",src,lvl,msg)
#define TEST_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Test",src,lvl,msg)
#define TEST_LOG_TRACE(src,lvl,msg) RX_TRACE("Test",src,lvl,msg)


namespace testing {
typedef uint_fast8_t test_status_t;





class test_command : public terminal::commands::server_command  
{
	DECLARE_REFERENCE_PTR(test_command);
	DECLARE_CONSOLE_CODE_INFO(0,1,0, "\
responsible of executing the test cases for rx-platform or for your plug-ins.\r\n\
test cases are divided into several categories. you can use test command to explore this test cases\
");


  public:
      test_command();

      ~test_command();


      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);

      bool do_info_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);

      bool do_run_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);

      bool do_status_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);

      bool do_list_command (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);


  protected:

  private:


};


struct test_context_data
{
	rx_time time_stamp;
	string_type user;
};




class test_program_context : public terminal::console::console_program_context  
{
  public:
	typedef test_program_context* smart_ptr;

  public:
      test_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<terminal::console::console_runtime> client);

      ~test_program_context();


      void set_failed ();

      void set_passed ();

      void async_test_end ();


      void set_current_test_case (rx_reference<test_case> value)
      {
        current_test_case_ = value;
      }



      const test_status_t get_status () const
      {
        return status_;
      }


      const test_context_data& get_data () const
      {
        return data_;
      }



  protected:

      size_t get_possition () const;


  private:

      void fill_data ();



      rx_reference<test_case> current_test_case_;


      test_status_t status_;

      test_context_data data_;


};







class test_case : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(test_case);

	DECLARE_CODE_INFO("rx", 0,5,0, "\
class intendend for console or script usage\r\n\
all about doing stuff with security");
public:

  public:
      test_case (const string_type& name);

      virtual ~test_case();


      virtual bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context* ctx) = 0;

      bool test_start (std::istream& in, std::ostream& out, std::ostream& err, test_program_context* ctx);

      void test_end (std::ostream& out, std::ostream& err, test_program_context* ctx);

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      rx_item_type get_type_id () const;

      values::rx_value get_value () const;

      namespace_item_attributes get_attributes () const;

      test_status_t get_status (test_context_data* data = nullptr);

      test_context_data get_data (test_context_data* data = nullptr) const;

      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, terminal::console_context_ptr ctx);

      rx_time get_created_time () const;

      platform_item_ptr get_item_ptr () const;

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      size_t get_size () const;

      void async_test_end (test_program_context* ctx);


      const rx_platform::meta::meta_data& meta_info () const;


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }


      string_type get_name () const
      {
        return name_;
      }



  protected:
	  template<typename T>
	  void dump_error_result(std::ostream& out, const rx_result_with<T>& result) const
	  {
		  for (const auto& one : result.errors())
			  out << ANSI_RX_ERROR_LIST ">>" ANSI_COLOR_RESET << one << "\r\n";
	  }
  private:
      test_case(const test_case &right);

      test_case & operator=(const test_case &right);



      rx_platform::meta::meta_data meta_info_;


      uint64_t start_tick_;

      test_status_t status_;

      test_context_data data_;

      locks::slim_lock status_lock_;

      rx_time modified_time_;

      string_type name_;


};






class test_category 
{
	typedef std::map<string_type, test_case::smart_ptr> cases_type;
public:
	typedef std::unique_ptr<test_category> smart_ptr;

  public:
      test_category (const string_type& category);

      virtual ~test_category();


      void collect_test_cases (std::vector<std::pair<string_type, test_case::smart_ptr> >& cases);

      void get_cases (string_array& cases);

      test_case::smart_ptr get_test_case (const string_type& test_name);


      const string_type& get_category () const
      {
        return category_;
      }



  protected:

      void register_test_case (test_case::smart_ptr test);


  private:
      test_category(const test_category &right);

      test_category & operator=(const test_category &right);



      cases_type cases_;


      string_type category_;


};






class testing_enviroment 
{

public:
	typedef std::map<string_type, test_category::smart_ptr> categories_type;

  public:
      ~testing_enviroment();


      void register_test_category (test_category::smart_ptr test);

      static testing_enviroment& instance ();

      void collect_test_cases (const string_type& category, std::vector<std::pair<string_type, test_case::smart_ptr> >& cases);

      void get_categories (string_array& categories);

      void get_cases (const string_type& category, string_array& cases);

      test_case::smart_ptr get_test_case (const string_type& test_name);

      test_program_context* create_test_context (terminal::console_context_ptr console_ctx);


  protected:

  private:
      testing_enviroment();



      categories_type categories_;


};






class basic_test_case_test : public test_case  
{
	DECLARE_REFERENCE_PTR(basic_test_case_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
This test creates dummy test case and is used for testing this mechanism\
");

  public:
      basic_test_case_test();

      ~basic_test_case_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};







class test_test : public test_category  
{

  public:
      test_test();

      ~test_test();


  protected:

  private:


};


} // namespace testing



#endif
