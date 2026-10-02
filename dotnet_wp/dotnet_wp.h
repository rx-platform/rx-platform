

/****************************************************************************
*
*  dotnet_wp\dotnet_wp.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


#ifndef dotnet_wp_h
#define dotnet_wp_h 1


#include "rx_dotnet_os.h"
#include "dotnet_wp_api.h"

// rx_job
#include "system/threads/rx_job.h"
// rx_thread
#include "system/threads/rx_thread.h"
// rx_io_buffers
#include "lib/rx_io_buffers.h"

namespace rx_internal {
namespace dotnet {
class dotnet_wp_host;
class pipe_client;
class dotnet_wp;

} // namespace dotnet
} // namespace rx_internal




namespace rx_internal {

namespace dotnet {





class pipe_client 
{
    typedef memory::page_aligned_buffer pipe_buffer_type;
	typedef std::stack<io::rx_io_buffer, std::vector<io::rx_io_buffer>> free_buffers_type;

  public:
      pipe_client (const pipe_client_t& pipes);

      ~pipe_client();


      rx_result write_pipe (const rx_packet_buffer* buffer);

      rx_result read_pipe (rx_const_packet_buffer* buffer);

      void close_pipe ();

      io::rx_io_buffer get_buffer ();

      void release_buffer (io::rx_io_buffer what);


  protected:

  private:


      free_buffers_type free_buffers_;


      pipe_client_t handles_;

      pipe_buffer_type buffer_;

      uintptr_t free_border_;


};






class dotnet_wp_send_job : public rx_platform::jobs::job  
{

  public:
      dotnet_wp_send_job();

      ~dotnet_wp_send_job();


      virtual rx_result process_start (rx_packet_buffer& buffer) = 0;


  protected:

  private:


};

typedef rx_reference<dotnet_wp_send_job> wp_send_job_ptr;






class dotnet_host_transaction 
{

  public:
      dotnet_host_transaction();

      virtual ~dotnet_host_transaction();


      virtual rx_result start (dotnet_wp& wp) const = 0;

      virtual rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer) = 0;

      virtual void timeouted () = 0;

      virtual uint32_t get_transaction_id () const = 0;


  protected:

  private:


};







class dotnet_wp 
{
	typedef std::map<uint32_t, std::shared_ptr<dotnet_host_transaction> > pending_wp_transactions_type;
public:
	template<typename T>
    void send_message(T message, uint32_t trans_id);

  public:
      ~dotnet_wp();


      static dotnet_wp& instance ();

      int run (int argc, char* argv[]);

      void append_trans (std::shared_ptr<dotnet_host_transaction> trans);

      void append_send (wp_send_job_ptr job);

      void log_event_fast (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, const string_type& message);

      void log_event (log_event_type event_type, const char* library, const string_type& source, uint16_t level, const char* code, const char* message, ... );

      void send_result_response (uint32_t trans_id, rx_result result);

      void send_void_response (uint32_t trans_id);

      void send_fire_event (void* inst, string_type value, string_type queue, bool state, bool remove);


      std::unique_ptr<pipe_client>& get_pipe_client ()
      {
        return pipe_client_;
      }



  protected:

  private:
      dotnet_wp();


      bool parse_command_line (int argc, char* argv[], const char* help_name, rx_dotnet_config_data_t& config, uint64_t& read_handle, uint64_t& write_handle);

      rx_result received_packet (rx_const_packet_buffer& received);

      rx_result initialize (rx_dotnet_config_data_t& config);



      rx_platform::threads::physical_job_thread sender_;

      std::unique_ptr<pipe_client> pipe_client_;

      std::unique_ptr<dotnet_wp_host> host_;

      pending_wp_transactions_type pending_wp_transactions_;


      locks::slim_lock transactions_lock_;

      string_type dotnet_host_version_;

      string_type worker_version_;


};






class build_type_transaction : public dotnet_host_transaction  
{

  public:
      build_type_transaction (build_one_type_message arg, uint32_t timeout);


      rx_result start (dotnet_wp& wp) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void timeouted ();

      uint32_t get_transaction_id () const;

      rx_result wait_result ();


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      locks::event trans_done_;

      rx_result result_;

      build_one_type_message arg_;


};






class build_runtime_transaction : public dotnet_host_transaction  
{

  public:
      build_runtime_transaction (uint64_t dotnet_trans, build_runtime_message arg, dotnetRuntimeCallback_t callback, uint32_t timeout);


      rx_result start (dotnet_wp& wp) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      build_runtime_message arg_;

      dotnetRuntimeCallback_t callback_;

      uint64_t dotnet_trans_id_;


};






class wp_write_transaction : public dotnet_host_transaction  
{

  public:
      wp_write_transaction (uint64_t dotnet_trans, wp_write_value_message arg, dotnetRuntimeCallback_t callback, uint32_t timeout);


      rx_result start (dotnet_wp& wp) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      wp_write_value_message arg_;

      dotnetRuntimeCallback_t callback_;

      uint64_t dotnet_trans_id_;


};






class delete_runtime_transaction : public dotnet_host_transaction  
{

  public:
      delete_runtime_transaction (uint64_t dotnet_trans, delete_one_runtime_message arg, dotnetRuntimeCallback_t callback, uint32_t timeout);


      rx_result start (dotnet_wp& wp) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      locks::event trans_done_;

      rx_result result_;

      delete_one_runtime_message arg_;

      dotnetRuntimeCallback_t callback_;

      uint64_t dotnet_trans_id_;


};






class delete_wp_type_transaction : public dotnet_host_transaction  
{

  public:
      delete_wp_type_transaction (delete_one_type_message arg, uint32_t timeout);


      rx_result start (dotnet_wp& wp) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void timeouted ();

      uint32_t get_transaction_id () const;

      rx_result wait_result ();


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      locks::event trans_done_;

      rx_result result_;

      delete_one_type_message arg_;


};






class wp_execute_transaction : public dotnet_host_transaction  
{

  public:
      wp_execute_transaction (uint64_t dotnet_trans, wp_execute_value_message arg, dotnetExecuteRuntimeCallback_t callback, uint32_t timeout);


      rx_result start (dotnet_wp& wp) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      wp_execute_value_message arg_;

      dotnetExecuteRuntimeCallback_t callback_;

      uint64_t dotnet_trans_id_;


};


} // namespace dotnet
} // namespace rx_internal



#endif
