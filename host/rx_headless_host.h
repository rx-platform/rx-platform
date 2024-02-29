

/****************************************************************************
*
*  host\rx_headless_host.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_headless_host_h
#define rx_headless_host_h 1



// rx_thread
#include "system/threads/rx_thread.h"
// rx_host
#include "system/hosting/rx_host.h"

#include "system/server/rx_server.h"
using rx_platform::hosting::hosts_type;


namespace host {

namespace headless {





class headless_security 
{

  public:

  protected:

  private:


};


typedef std::function<void(job_ptr)> synchronize_callback_t;




class rx_thread_synchronizer : public rx_platform::threads::job_thread  
{

  public:

      void append (job_ptr pjob);

      void init_callback (synchronize_callback_t callback);

      void deinit_callback ();


  protected:

  private:


      synchronize_callback_t synchronize_callback_;


};






class headless_platform_host : public rx_platform::hosting::rx_platform_host  
{

  public:
      headless_platform_host (const std::vector<storage_base::rx_platform_storage_type*>& storages);

      ~headless_platform_host();


      void get_host_info (hosts_type& hosts);

      void server_started_event ();

      bool shutdown (const string_type& msg);

      bool exit () const;

      bool do_host_command (const string_type& line, memory::buffer_ptr out_buffer, memory::buffer_ptr err_buffer, const security::security_context& ctx);

      bool break_host (const string_type& msg);

      int initialize_platform (int argc, char* argv[], const char* instance_name, const char* help_name, log::log_subscriber::smart_ptr log_subscriber, synchronize_callback_t sync_callback, std::vector<library::rx_plugin_base*>& plugins);

      static string_type get_headless_info ();

      bool is_canceling () const;

      bool read_stdin (std::array<char,0x100>& chars, size_t& count);

      bool write_stdout (const void* data, size_t size);

      int deinitialize_platform ();

      string_type just_parse_command_line (int argc, char* argv[], rx_platform::configuration_data_t& config);

      rx_result build_host (hosting::host_platform_builder& builder);

      string_type get_host_manual () const;

      int start_platform ();

      int stop_platform ();

      virtual rx_result register_constructors ();

      string_type get_host_instance ();


  protected:

  private:

      virtual rx_result set_headless_thread_security ();

      virtual rx_result remove_headless_thread_security ();



      rx_thread_synchronizer thread_synchronizer_;


      bool exit_;

      rx_platform::configuration_data_t config_;

      bool debug_break_;

      string_type host_instance_;


};


} // namespace headless
} // namespace host



#endif
