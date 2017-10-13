

/****************************************************************************
*
*  system\server\rx_cmds.h
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


#ifndef rx_cmds_h
#define rx_cmds_h 1



// rx_mngt
#include "system/server/rx_mngt.h"
// rx_ns
#include "system/server/rx_ns.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_mem
#include "lib/rx_mem.h"

namespace server {
namespace prog {
class server_program_holder;
class program_context_base;
} // namespace prog

namespace security {
class security_context;

} // namespace security
} // namespace server


using namespace rx;
using namespace server::ns;


namespace server {

namespace prog {
class program_executer_base;
class server_program_base;
class server_command_base;
typedef pointers::reference<program_context_base> program_context_ptr;
typedef pointers::reference<server_program_base> server_program_ptr;
typedef pointers::virtual_reference<server_command_base> command_ptr;
typedef pointers::virtual_reference<program_executer_base> program_executer_ptr;
typedef pointers::reference<server_program_holder> server_program_holder_ptr;
typedef pointers::reference<program_context_base> program_context_base_ptr;






class program_context_base : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(program_context_base);

  public:
      program_context_base (server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory);

      virtual ~program_context_base();


      bool is_postponed () const;


      server_directory_ptr get_current_directory ()
      {
        return _current_directory;
      }

      void set_current_directory (server_directory_ptr value)
      {
        _current_directory = value;
      }



  protected:

  private:


      program_context_base::smart_ptr _root;

      server_program_holder_ptr _holder;


      server_directory_ptr _current_directory;


};







class server_program_base : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(server_program_base);

  public:
      server_program_base();

      virtual ~server_program_base();


      virtual bool process_program (program_context_ptr context, const rx_time& now, bool debug) = 0;


  protected:

  private:


};






class program_executer_base : public rx::pointers::virtual_reference_object  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(program_executer_base);

  public:
      program_executer_base();

      virtual ~program_executer_base();


  protected:

  private:


      server_program_holder_ptr _holder;


    friend class server_program_holder;
};







class server_program_holder : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(server_program_holder);
	typedef std::map<string_type, server_program_ptr> sub_programs_type;

  public:
      server_program_holder (program_executer_ptr executer);

      virtual ~server_program_holder();


  protected:

  private:


      server_program_base::smart_ptr _main_program;

      sub_programs_type _sub_programs;

      program_executer_base::smart_ptr _executer;

      program_context_base_ptr _main_context;


};







class console_program_context : public program_context_base  
{
	DECLARE_REFERENCE_PTR(console_program_context);


	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;


  public:
      console_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err);

      virtual ~console_program_context();


      size_t next_line ();

      std::ostream& get_stdout ();

      std::ostream& get_stderr ();


      const size_t get_current_line () const
      {
        return _current_line;
      }



  protected:

  private:


      size_t _current_line;

      buffer_ptr _out;

      buffer_ptr _err;

      std::ostream _out_std;

      std::ostream _err_std;


};







class server_command_base : public ns::rx_server_item  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(server_command_base);

  public:
      server_command_base (const string_type& console_name, ns::namespace_item_attributes attributes);

      virtual ~server_command_base();


      virtual const string_type& get_console_name (std::istream& in, std::ostream& out, std::ostream& err);

      virtual bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_context::smart_ptr ctx) = 0;

      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      void get_value (values::rx_value& val) const;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      namespace_item_attributes get_attributes () const;

      const string_type& get_item_name () const;


      const string_type& get_console_name () const
      {
        return _console_name;
      }


	  virtual const char* get_help() const = 0;
  protected:

      rx_time _time_stamp;


  private:


      string_type _console_name;


};







class server_console_program : public server_program_base  
{
	DECLARE_REFERENCE_PTR(server_console_program);
	
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      server_console_program (std::istream& in);

      server_console_program (const string_vector& lines);

      server_console_program (const string_type& line);

      virtual ~server_console_program();


      bool process_program (prog::program_context_ptr context, const rx_time& now, bool debug);

      prog::program_context_ptr create_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err);


  protected:

  private:


      string_vector _lines;


};






class console_client : public rx::pointers::virtual_reference_object  
{

  public:
      console_client();

      virtual ~console_client();


      bool is_postponed () const;


  protected:

      bool do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      void get_prompt (string_type& prompt);

      void get_wellcome (string_type& wellcome);

      virtual const string_type& get_console_name () = 0;

      virtual void exit_console () = 0;


  private:

      server_directory_ptr _current_directory;



      program_context_base::smart_ptr _current;


      string_type _line;


};






class server_script_program : public server_program_base  
{
	DECLARE_REFERENCE_PTR(server_script_program);
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      server_script_program (std::istream& in);

      server_script_program (const string_vector& lines);

      server_script_program (const string_type& line);

      virtual ~server_script_program();


      bool process_program (prog::program_context_ptr context, const rx_time& now, bool debug);

      prog::program_context_ptr create_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err);


  protected:

  private:


};


struct script_def_t
{
	string_type name;
	string_type user_name;
	string_type version;
};





class server_script_host 
{

  public:
      server_script_host (const script_def_t& definition);

      virtual ~server_script_host();


      virtual void deinitialize () = 0;

      virtual bool initialize () = 0;

      virtual bool dump_script_information (std::ostream& out) = 0;

      virtual void do_testing_stuff () = 0;

      virtual bool init_thread () = 0;

      virtual bool deinit_thread () = 0;


      const script_def_t& get_definition () const
      {
        return _definition;
      }



  protected:

  private:


      script_def_t _definition;


};


} // namespace prog
} // namespace server



#endif
