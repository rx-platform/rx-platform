

/****************************************************************************
*
*  system\server\rx_cmds.h
*
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_cmds_h
#define rx_cmds_h 1


#include "lib/security/rx_security.h"

// rx_objbase
#include "system/runtime/rx_objbase.h"
// rx_logic
#include "system/logic/rx_logic.h"
// rx_mem
#include "lib/rx_mem.h"
// sl_script
#include "soft_logic/sl_script.h"

namespace rx_platform {
namespace prog {
class console_client;
class server_console_program;
class console_program_context;

} // namespace prog
} // namespace rx_platform


using namespace rx;
using namespace rx_platform::ns;


namespace rx_platform {

namespace prog {
typedef std::uint_fast16_t sec_error_num_t;
class server_command_base;
typedef pointers::reference<server_command_base> command_ptr;






class console_program_context : public sl_runtime::sl_script::script_program_context  
{
public:
	typedef console_program_context* smart_ptr;
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
	typedef std::map<size_t, rx_struct_ptr > instructions_data_type;
	typedef int pending_jobs_type;

  public:
      console_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, rx_directory_ptr current_directory, buffer_ptr out, buffer_ptr err, rx_reference<console_client> client);

      ~console_program_context();


      std::ostream& get_stdout ();

      std::ostream& get_stderr ();

      void send_results (bool result);

      bool is_postponed () const;

      bool postpone (uint32_t interval);

      void set_instruction_data (rx_struct_ptr data);

      bool is_canceled ();

      void postpone_done ();

      void set_waiting ();


      rx_reference<console_client> get_client ()
      {
        return client_;
      }



      rx_directory_ptr get_current_directory ()
      {
        return current_directory_;
      }

      void set_current_directory (rx_directory_ptr value)
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


      bool get_result () const
      {
        return result_;
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

      size_t get_possition () const;


  private:


      rx_reference<console_client> client_;


      std::ostream out_std_;

      std::ostream err_std_;

      rx_directory_ptr current_directory_;

      buffer_ptr out_;

      buffer_ptr err_;

      std::atomic<int> postponed_;

      instructions_data_type instructions_data_;

      std::atomic_bool canceled_;

      bool result_;


};







class server_command_base : public logic::program_runtime  
{

  public:
      server_command_base (const string_type& name, const rx_node_id& id);

      ~server_command_base();


      void get_class_info (string_type& class_name, string_type& console, bool& has_own_code_info);

      string_type get_type_name () const;

      values::rx_value get_value () const;

      void item_lock ();

      void item_unlock ();

      void item_lock () const;

      void item_unlock () const;

      namespace_item_attributes get_attributes () const;

      bool console_execute (std::istream& in, std::ostream& out, std::ostream& err, console_program_context::smart_ptr ctx);

      rx_time get_created_time () const;

      bool generate_json (std::ostream& def, std::ostream& err) const;

      platform_item_ptr get_item_ptr ();

      string_type get_name () const;

      size_t get_size () const;

      bool serialize_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_definition (base_meta_reader& stream, uint8_t type);


      const string_type& get_console_name () const
      {
        return console_name_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }


	  virtual const char* get_help() const = 0;
  protected:

      virtual bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_context::smart_ptr ctx) = 0;

      bool dword_check_premissions (security::security_mask_t mask, security::extended_security_mask_t extended_mask);


      rx_time time_stamp_;


  private:


      string_type console_name_;

      security::security_guard_ptr security_guard_;

      rx_time modified_time_;


};






class console_client : public runtime::objects::port_runtime  
{
	DECLARE_REFERENCE_PTR(console_client);

  public:
      console_client (runtime::objects::port_creation_data&& data);

      ~console_client();


      virtual const string_type& get_console_name () = 0;

      bool is_postponed () const;

      const string_type& get_console_terminal ();

      virtual void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer) = 0;

      void process_event (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, bool done);


  protected:

      bool do_command (string_type&& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void get_prompt (string_type& prompt);

      void get_wellcome (string_type& wellcome);

      virtual void exit_console () = 0;

      bool cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void get_security_error (string_type& txt, sec_error_num_t err_number);

      bool do_commands (string_array&& lines, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


  private:

      void synchronized_do_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);

      void synchronized_cancel_command (memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, security::security_context_ptr ctx);


      rx_directory_ptr current_directory_;



      rx_reference<server_console_program> current_program_;

      console_program_context *current_context_;


      string_type line_;

      string_type name_;


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

      ~server_script_host();


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







class console_program : public sl_runtime::sl_script::sl_script_program  
{
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      console_program();

      ~console_program();


      sl_runtime::program_context* create_program_context (sl_runtime::program_context* parent_context, sl_runtime::sl_program_holder* holder);


  protected:

      bool parse_line (const string_type& line, std::ostream& out, std::ostream& err, sl_runtime::program_context* context);


  private:


};







class server_console_program : public logic::program_runtime  
{
	DECLARE_REFERENCE_PTR(server_console_program);
	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      server_console_program (console_client::smart_ptr client, const string_type& name, const rx_node_id& id, bool system = false);

      ~server_console_program();


      void cancel_execution ();


  protected:

  private:


      console_program program_;

      rx_reference<console_client> console_;


      string_vector lines_;


};


} // namespace prog
} // namespace rx_platform



#endif
