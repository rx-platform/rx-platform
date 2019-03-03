

/****************************************************************************
*
*  terminal\rx_con_commands.h
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


#ifndef rx_con_commands_h
#define rx_con_commands_h 1



/////////////////////////////////////////////////////////////
// logging macros for console library
#define CONSOLE_LOG_INFO(src,lvl,msg) RX_LOG_INFO("Console",src,lvl,msg)
#define CONSOLE_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("Console",src,lvl,msg)
#define CONSOLE_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("Console",src,lvl,msg)
#define CONSOLE_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("Console",src,lvl,msg)
#define CONSOLE_LOG_TRACE(src,lvl,msg) RX_TRACE("Console",src,lvl,msg)

// rx_commands
#include "terminal/rx_commands.h"

#include <lib/rx_mem.h>
using namespace rx;
using namespace rx_platform;
using namespace rx_platform::ns;


namespace terminal {

namespace console {

namespace console_commands {
typedef rx_platform::prog::console_program_context::smart_ptr console_program_contex_ptr;





class rx_name_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(rx_name_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
displays details of software and system information");

  public:
      rx_name_command();

      ~rx_name_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class cls_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(cls_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
clears the console screen");

  public:
      cls_command();

      ~cls_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class shutdown_command : public commands::server_command  
{
	DECLARE_REFERENCE_PTR(shutdown_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
all about shutdown of a server");

  public:
      shutdown_command();

      ~shutdown_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class log_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(log_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
all about doing stuff with log");

  public:
      log_command();

      ~log_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);

      bool do_test_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);

      bool do_hist_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class sec_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(sec_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
all about doing stuff with security");

  public:
      sec_command();

      ~sec_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);

      bool do_active_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class time_command : public commands::server_command  
{

	DECLARE_REFERENCE_PTR(time_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
time related stuff ( start time, current time... )");

  public:
      time_command();

      ~time_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


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

      ~sleep_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class directory_aware_command : public commands::server_command  
{

  public:
      directory_aware_command (const string_type& console_name);

      ~directory_aware_command();


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

      ~info_command();


      bool dump_dir_info (std::ostream& out, rx_directory_ptr directory);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


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

      ~code_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class def_command : public directory_aware_command  
{
	DECLARE_REFERENCE_PTR(def_command);

	DECLARE_CONSOLE_CODE_INFO( 0,5,0, "\
command that dumps specified object into a Json stream and write it on a console");

  public:
      def_command();

      ~def_command();


      bool dump_object_definition (std::ostream& out, std::ostream& err, platform_item_ptr item);


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class phyton_command : public commands::server_command  
{
	DECLARE_REFERENCE_PTR(phyton_command);

	DECLARE_CONSOLE_CODE_INFO( 0,1,0, "\
pyhton command for interfacing python scripting");

  public:
      phyton_command();

      ~phyton_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};






class license_command : public commands::server_command  
{
	DECLARE_REFERENCE_PTR(license_command);
	DECLARE_CONSOLE_CODE_INFO(0, 1, 0, "\
displays license info");

  public:
      license_command();

      ~license_command();


  protected:

      bool do_console_command (std::istream& in, std::ostream& out, std::ostream& err, console_program_contex_ptr ctx);


  private:


};


} // namespace console_commands
} // namespace console
} // namespace terminal



#endif


