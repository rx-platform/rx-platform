

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

namespace rx {
namespace security {
class security_context;
} // namespace security
} // namespace rx

namespace rx_platform {
namespace prog {
class server_program_holder;
class server_program_base;
class program_context_base;

} // namespace prog
} // namespace rx_platform


using namespace rx;
using namespace rx_platform::ns;


namespace rx_platform {

namespace prog {
class program_executer_base;
class server_program_base;
class server_command_base;
typedef pointers::virtual_reference<server_command_base> command_ptr;
typedef pointers::reference<program_context_base> program_context_ptr;
typedef pointers::reference<server_program_base> server_program_ptr;
typedef pointers::virtual_reference<program_executer_base> program_executer_ptr;
typedef pointers::reference<server_program_holder> server_program_holder_ptr;
typedef pointers::reference<program_context_base> program_context_base_ptr;






class program_context_base : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(program_context_base);
public:
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
	typedef std::map<size_t, rx_struct_ptr > instructions_data_type;
	typedef int pending_jobs_type;

  public:
      program_context_base (server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<server_program_base> program);

      virtual ~program_context_base();


      bool is_postponed () const;

      bool postpone (uint32_t interval);

      void set_instruction_data (rx_struct_ptr data);

      bool is_canceled ();

      void cancel_execution ();

      bool should_run_again ();


      const rx_reference<server_program_holder> get_holder () const
      {
        return holder_;
      }


      rx_reference<server_program_base> get_program ()
      {
        return program_;
      }



      server_directory_ptr get_current_directory ()
      {
        return current_directory_;
      }

      void set_current_directory (server_directory_ptr value)
      {
        current_directory_ = value;
      }


      const buffer_ptr get_out () const
      {
        return out_;
      }


      const buffer_ptr get_err () const
      {
        return err_;
      }


      platform_item_ptr get_current_object ()
      {
        return current_object_;
      }

      void set_current_object (platform_item_ptr value)
      {
        current_object_ = value;
      }


      platform_item_ptr get_current_item ()
      {
        return current_item_;
      }

      void set_current_item (platform_item_ptr value)
      {
        current_item_ = value;
      }


	  template<typename T>
	  pointers::reference<T> get_instruction_data()
	  {
		  auto it = instructions_data_.find(get_possition());
		  if (it != instructions_data_.end())
		  {
			  return it->second.cast_to<pointers::reference<T> >();
		  }
		  else
		  {
			  return pointers::reference<T>::null_ptr;
		  }
	  }
  protected:

      virtual void send_results (bool result);

      virtual size_t get_possition () const = 0;


  private:

      bool return_control (bool done = true);



      rx_reference<program_context_base> root_;

      rx_reference<server_program_holder> holder_;

      rx_reference<server_program_base> program_;


      server_directory_ptr current_directory_;

      buffer_ptr out_;

      buffer_ptr err_;

      bool postponed_;

      instructions_data_type instructions_data_;

      std::atomic_bool canceled_;

      pending_jobs_type pending_jobs_;

      platform_item_ptr current_object_;

      platform_item_ptr current_item_;


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


      rx_reference<server_program_holder> holder_;


    friend class server_program_holder;
};







class server_program_holder : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(server_program_holder);
	typedef std::map<string_type, server_program_ptr> sub_programs_type;

  public:
      server_program_holder (program_executer_ptr executer);

      virtual ~server_program_holder();


      rx_virtual<program_executer_base> get_executer ()
      {
        return executer_;
      }



  protected:

  private:


      rx_reference<server_program_base> main_program_;

      sub_programs_type sub_programs_;

      rx_virtual<program_executer_base> executer_;

      rx_reference<program_context_base> main_context_;


};






class console_client : public rx::pointers::virtual_reference_object  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(console_client);

  public:
      console_client (rx_thread_handle_t executer);

      virtual ~console_client();


      virtual const string_type& get_console_name () = 0;

      bool is_postponed () const;

      const string_type& get_console_terminal ();

      virtual bool get_next_line (string_type& line) = 0;

      void process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done);


  protected:

      bool do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void get_prompt (string_type& prompt);

      void get_wellcome (string_type& wellcome);

      virtual void exit_console () = 0;

      virtual void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer) = 0;

      bool cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


  private:

      void synchronized_do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void synchronized_cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


      server_directory_ptr current_directory_;



      rx_reference<program_context_base> current_;


      string_type line_;

      string_type name_;

      rx_thread_handle_t executer_;

      platform_item_ptr current_object_;

      platform_item_ptr current_item_;


};







class console_program_context : public program_context_base  
{
	DECLARE_REFERENCE_PTR(console_program_context);

  public:
      console_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<server_program_base> program, rx_virtual<console_client> client);

      virtual ~console_program_context();


      size_t next_line ();

      std::ostream& get_stdout ();

      std::ostream& get_stderr ();

      console_program_context::smart_ptr create_console_sub_context ();


      const size_t get_current_line () const
      {
        return current_line_;
      }



  protected:

      void send_results (bool result);

      size_t get_possition () const;


  private:


      rx_virtual<console_client> client_;


      size_t current_line_;

      std::ostream out_std_;

      std::ostream err_std_;


};







class server_command_base : public ns::rx_platform_item  
{
	DECLARE_VIRTUAL_REFERENCE_PTR(server_command_base);

  public:
      server_command_base (const string_type& console_name, ns::namespace_item_attributes attributes);

      virtual ~server_command_base();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      void get_value (values::rx_value& val) const;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      namespace_item_attributes get_attributes () const;

      const string_type& get_item_name () const;

      bool console_execute (std::istream& in, std::ostream& out, std::ostream& err, console_program_context::smart_ptr ctx);


      const string_type& get_console_name () const
      {
        return console_name_;
      }


	  virtual const char* get_help() const = 0;
  protected:

      virtual bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_context::smart_ptr ctx) = 0;

      bool dword_check_premissions (security::security_mask_t mask, security::extended_security_mask_t extended_mask);


      rx_time time_stamp_;


  private:


      string_type console_name_;

      security::security_guard_ptr security_guard_;


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

      prog::program_context_ptr create_program_context (prog::server_program_holder_ptr holder, prog::program_context_ptr root_context, server_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_virtual<console_client> client);


  protected:

  private:


      string_vector lines_;


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
        return definition_;
      }



  protected:

  private:


      script_def_t definition_;


};


} // namespace prog
} // namespace rx_platform



#endif
