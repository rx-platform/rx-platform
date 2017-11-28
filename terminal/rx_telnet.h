

/****************************************************************************
*
*  terminal\rx_telnet.h
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


#ifndef rx_telnet_h
#define rx_telnet_h 1



/////////////////////////////////////////////////////////////
// logging macros for console library
#define CONSOLE_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Console",src,lvl,msg)
#define CONSOLE_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Console",src,lvl,msg)
#define CONSOLE_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Console",src,lvl,msg)
#define CONSOLE_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Console",src,lvl,msg)
#define CONSOLE_LOG_TRACE(src,lvl,msg) RX_TRACE("Console",src,lvl,msg)

// rx_cmds
#include "system/server/rx_cmds.h"
// rx_security
#include "lib/security/rx_security.h"
// rx_ptr
#include "lib/rx_ptr.h"
// rx_io
#include "lib/rx_io.h"
// rx_commands
#include "terminal/rx_commands.h"



#include <lib/rx_mem.h>
using namespace rx;
using namespace server;


namespace terminal {

namespace console {





class telnet_security_context : public rx::security::security_context, 
                                	public
{
	DECLARE_REFERENCE_PTR(telnet_security_context);

	DECLARE_DERIVED_FROM_INTERFACE;

  public:
      telnet_security_context();

      telnet_security_context (const sockaddr_in& addr, const sockaddr_in& local_addr);

      virtual ~telnet_security_context();


      bool has_console () const;

      bool is_system () const;


  protected:

  private:


};






class telnet_client : public server::prog::console_client, 
                      	public rx::io::
{
	DECLARE_REFERENCE_PTR(telnet_client);

	DECLARE_DERIVED_FROM_VIRTUAL_REFERENCE;

	typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
	typedef std::stack<buffer_ptr> buffers_type;
	typedef std::queue<buffer_ptr> running_buffers_type;

  public:
      telnet_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher);

      virtual ~telnet_client();


      bool on_startup (rx_thread_handle_t destination);

      void on_shutdown (rx_thread_handle_t destination);

      const string_type& get_console_name ();

      bool get_next_line (string_type& line);


  protected:

      void exit_console ();

      bool readed (const void* data, size_t count, rx_thread_handle_t destination);

      void release_buffer (buffer_ptr what);

      void process_result (bool result, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer);


  private:

      telnet_client::buffer_ptr get_free_buffer ();

      bool new_recive (const char* buff, size_t& idx);

      void send_string_response (const string_type& line, bool with_prompt = true);



      rx_reference<telnet_security_context> _security_context;


      locks::slim_lock _buffers_lock;

      buffers_type _buffers;

      string_type _receiving_string;

      bool _send_echo;

      bool _cancel_current;

      bool _verified;

      bool _exit;


};







class server_telnet_socket : public rx::io::tcp_listent_std_buffer  
{
	DECLARE_REFERENCE_PTR(server_telnet_socket);

  public:
      server_telnet_socket();

      virtual ~server_telnet_socket();


  protected:

      io::tcp_socket_std_buffer::smart_ptr make_client (sys_handle_t handle, sockaddr_in* addr, sockaddr_in* local_addr, threads::dispatcher_pool::smart_ptr& dispatcher, rx_thread_handle_t destination);


  private:


};


namespace console_commands {





class directory_command : public commands::server_command  
{
	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
list current directory on console ( dir, ls ");

  public:
      directory_command (const string_type& console_name);

      virtual ~directory_command();


      bool list_directory (std::ostream& out, std::ostream& err, const string_type& filter, bool list_attributes, bool list_qualities, bool list_timestamps, bool list_created, bool list_type, server_directory_ptr& directory);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class dir_command : public directory_command  
{
	DECLARE_REFERENCE_PTR(dir_command);

  public:
      dir_command();

      virtual ~dir_command();


  protected:

  private:


};






class ls_command : public directory_command  
{
	DECLARE_REFERENCE_PTR(ls_command);

  public:
      ls_command();

      virtual ~ls_command();


      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};






class cd_command : public commands::server_command  
{
	DECLARE_REFERENCE_PTR(cd_command);
	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
changes current active directory");

  public:
      cd_command();

      virtual ~cd_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class rx_name_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(rx_name_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
displays details of software and system information");

  public:
      rx_name_command();

      virtual ~rx_name_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class cls_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(cls_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
clears the console screen");

  public:
      cls_command();

      virtual ~cls_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class shutdown_command : public commands::server_command  
{
	DECLARE_REFERENCE_PTR(shutdown_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
all about shutdown of a server");

  public:
      shutdown_command();

      virtual ~shutdown_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class log_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(log_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
all about doing stuff with log");

  public:
      log_command();

      virtual ~log_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      bool do_test_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      bool do_hist_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class sec_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(sec_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
all about doing stuff with security");

  public:
      sec_command();

      virtual ~sec_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);

      bool do_active_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class time_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(time_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
time related stuff ( start time, current time... )");

  public:
      time_command();

      virtual ~time_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};


struct sleep_data_t : public pointers::struct_reference
{
	uint64_t started;
};




class sleep_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(sleep_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
sleeps for the amaount of time specified in miliseconds");

  public:
      sleep_command();

      virtual ~sleep_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class directory_aware_command : public commands::server_command  
{

  public:
      directory_aware_command (const string_type& console_name);

      virtual ~directory_aware_command();


  protected:

  private:


};






class info_command : public directory_aware_command  
{
	DECLARE_REFERENCE_PTR(info_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
displays details of selected item");

  public:
      info_command();

      virtual ~info_command();


      bool dump_dir_info (std::ostream& out, server_directory_ptr directory);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class code_command : public directory_aware_command  
{
	DECLARE_REFERENCE_PTR(code_command);
	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
intendend for console usage\r\n\
dumps information of C++ class of object on console\
uses fill_server_info function");

  public:
      code_command();

      virtual ~code_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class def_command : public directory_aware_command  
{
	DECLARE_REFERENCE_PTR(def_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
command that dumps specified object into a Json stream and write it on a console");

  public:
      def_command();

      virtual ~def_command();


      bool dump_object_definition (std::ostream& out, std::ostream& err, server_item_ptr item);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};






class phyton_command : public commands::server_command  
{
	DECLARE_REFERENCE_PTR(phyton_command);

	DECLARE_CONSOLE_CODE_INFO( 0,1,0, "\
pyhton command for interfacing python scripting");

  public:
      phyton_command();

      virtual ~phyton_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  private:


};


} // namespace console_commands
} // namespace console
} // namespace terminal



#endif
