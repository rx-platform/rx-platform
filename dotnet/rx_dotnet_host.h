

/****************************************************************************
*
*  dotnet\rx_dotnet_host.h
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


#ifndef rx_dotnet_host_h
#define rx_dotnet_host_h 1


#include "dotnet_api.h"
#include "protocols/ansi_c/common_c/rx_packet_decoder.h"

// rx_thread
#include "system/threads/rx_thread.h"
// rx_dotnet_runtime
#include "dotnet/rx_dotnet_runtime.h"
// rx_io_buffers
#include "lib/rx_io_buffers.h"

namespace rx_internal {
namespace dotnet {
class dotnet_wp_connection;
class dotnet_host;

} // namespace dotnet
} // namespace rx_internal




namespace rx_internal {

namespace dotnet {





class pipe_receiver : public rx_platform::threads::thread  
{
    typedef memory::page_aligned_buffer pipe_buffer_type;

  public:
      pipe_receiver();

      ~pipe_receiver();


      void start_receive (pipe_server_t* pipes);

      rx_result write_pipe (const rx_packet_buffer* buffer);


      void set_my_connection (dotnet_wp_connection * value);


  protected:

      uint32_t handler ();


  private:


      dotnet_wp_connection *my_connection_;


      pipe_server_t* pipes_;

      pipe_buffer_type buffer_;

      packed_decoder_type packet_decoder_;

      io::rx_io_buffer receive_buffer_;

      dotnet_wp_message_header_t header_buffer_;


};







class dotnet_wp_transaction 
{

  public:
      dotnet_wp_transaction();

      virtual ~dotnet_wp_transaction();


      virtual rx_result start (dotnet_wp_connection& connection) const = 0;

      virtual rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer) = 0;

      virtual void timeouted () = 0;

      virtual void response_error (rx_result result) = 0;

      virtual uint32_t get_transaction_id () const = 0;


  protected:

  private:


};






class dotnet_wp_connection 
{
	typedef std::map<uint32_t, std::shared_ptr<dotnet_wp_transaction>> pending_out_transactions_type;

    typedef std::stack<io::rx_io_buffer, std::vector<io::rx_io_buffer>> free_buffers_type;
public:
    template<typename T>
    void send_message(T message, uint32_t trans_id);

  public:
      dotnet_wp_connection();

      ~dotnet_wp_connection();


      rx_result initialize (dotnet_host* host, const rx_dotnet_config_data_t& config);

      void bind_object (rx_item_type type, const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void unbind_object (rx_item_type type, const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose);

      void send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose);

      rx_result execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value);

      rx_result source_write (uint32_t trans_id, void* whose, typed_value_type val);

      void initialize_completed (uint32_t trans_id, rx_result result, string_type hosted_version, string_type dotnet_host_version);

      rx_result do_build (const string_type& lib, uint32_t timeout);

      rx_result do_reload (const string_type& lib, uint32_t timeout);

      void do_unload (const string_type& lib, rx_result_callback callback, uint32_t timeout);

      rx_result received (io::rx_const_io_buffer& buffer);

      void pipe_broken (string_type msg);

      rx_result start ();

      void deinitialize (rx_void_callback callback, uint32_t timeout);

      void stop ();


      string_type hosted_version;

      string_type dotnet_host_version;


  protected:

  private:

      io::rx_io_buffer get_buffer ();

      void release_buffer (io::rx_io_buffer what);



      dotnet_host *my_host_;

      pipe_receiver pipe_server_;

      rx_platform::threads::physical_job_thread sender_;

      pending_out_transactions_type pending_out_transactions_;

      free_buffers_type free_buffers_;


      sys_handle_t process_;

      pipe_server_t pipes_;

      locks::event initialized_;

      uintptr_t free_border_;

      rx_result action_result_;


};






class dotnet_host 
{
public:
    dotnet_host(const dotnet_host&) = delete;
	dotnet_host& operator=(const dotnet_host&) = delete;
	dotnet_host(dotnet_host&&) noexcept = delete;
	dotnet_host& operator=(dotnet_host&&) noexcept = delete;    

	typedef std::map<uint32_t, rx_result_callback> result_callbacks_type;
    typedef std::map<uint32_t, rx_platform::callback::rx_any_callback<> > void_callbacks_type;

	typedef std::map<void*, rx_reference<dotnet_object_runtime> > runtime_objects_type;
    typedef std::map<void*, rx_reference<dotnet_source> > runtime_sources_type;
    typedef std::map<void*, rx_reference<dotnet_mapper> > runtime_mappers_type;
    typedef std::map<void*, rx_reference<dotnet_event> > runtime_events_type;
    typedef std::map<void*, rx_reference<dotnet_struct> > runtime_structs_type;

  public:
      dotnet_host (const configuration_data_t& data, const rx_dotnet_config_data_t& init_config);

      ~dotnet_host();


      rx_result initialize (const rx_dotnet_config_data_t& config);

      rx_result start ();

      void deinitialize (rx_void_callback callback);

      rx_result do_build (configuration_data_t& config, startup_create_data_type& data);

      rx_result do_reload (const string_type& lib);

      void do_unload (const string_type& lib, rx_result_callback callback);

      void received_result_callback (uint32_t transId, rx_result result);

      void received_void_callback (uint32_t transId);

      void received_write_value (rx_item_type type, uint32_t transId, void* inst, size_t idx, rx_simple_value val, std::function<void(rx_result&&)> callback);

      void bind_runtime (const rx_node_id& id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_object_runtime> ref);

      void unbind_runtime (const rx_node_id& id, const rx_node_id& type_id, void* ptr);

      void runtime_value_changed (size_t idx, rx_simple_value val, rx_item_type type, void* whose);

      void send_initial_values (string_array names, std::vector<rx_simple_value> val, rx_item_type type, void* whose);

      rx_result execute_method (uint32_t trans_id, void* whose, const string_type& method, const string_type& value);

      void received_execute_done (uint32_t transId, void* inst, const string_type& val, rx_result result);

      void bind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_struct> ref);

      void unbind_struct (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_source> ref);

      void unbind_source (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_mapper> ref);

      void unbind_mapper (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      void bind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr, rx_reference<dotnet_event> ref);

      void unbind_event (const rx_node_id id, const string_type& path, const rx_node_id& type_id, void* ptr);

      rx_result source_write (uint32_t trans_id, void* whose, typed_value_type val);

      void received_source_change (void* inst, rx_value val);

      void received_source_write_done (uint32_t trans_id, void* inst, rx_result result);

      void received_fire_event (void* inst, string_type val, string_type queue, bool state, bool remove);

      const string_type& get_hosted_dotnet_version () const;

      const string_type& get_dotnet_host_version () const;

      void stop ();


  protected:

  private:


      runtime_objects_type runtime_objects_;

      runtime_sources_type runtime_sources_;

      runtime_mappers_type runtime_mappers_;

      runtime_events_type runtime_events_;

      runtime_structs_type runtime_structs_;

      dotnet_wp_connection wp_;


      result_callbacks_type result_callbacks_;

      void_callbacks_type void_callbacks_;

      locks::slim_lock callbacks_lock_;

      string_type hosted_version_;

      string_type dotnet_host_version_;


};






class build_types_wp_transaction : public dotnet_wp_transaction  
{

  public:
      build_types_wp_transaction (const string_type& lib, uint32_t timeout);


      rx_result start (dotnet_wp_connection& connection) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void response_error (rx_result result);

      void timeouted ();

      uint32_t get_transaction_id () const;

      rx_result wait_result ();


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      locks::event trans_done_;

      rx_result result_;

      string_type library_;


};






class execute_method_wp_transaction : public dotnet_wp_transaction  
{

  public:
      execute_method_wp_transaction (uint32_t trans_id, void* whose, const string_type& method, const string_type& value);


      rx_result start (dotnet_wp_connection& connection) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void response_error (rx_result result);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      void* whose_;

      string_type method_;

      string_type value_;


};






class reload_wp_transaction : public dotnet_wp_transaction  
{

  public:
      reload_wp_transaction (const string_type& lib, uint32_t timeout);


      rx_result start (dotnet_wp_connection& connection) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void response_error (rx_result result);

      void timeouted ();

      uint32_t get_transaction_id () const;

      rx_result wait_result ();


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      locks::event trans_done_;

      rx_result result_;

      string_type library_;


};






class unload_wp_transaction : public dotnet_wp_transaction  
{

  public:
      unload_wp_transaction (const string_type& lib, rx_result_callback callback, uint32_t timeout);


      rx_result start (dotnet_wp_connection& connection) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void response_error (rx_result result);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      rx_result result_;

      string_type library_;

      rx_result_callback callback_;


};






class stop_wp_transaction : public dotnet_wp_transaction  
{

  public:
      stop_wp_transaction (rx_void_callback callback, uint32_t timeout);


      rx_result start (dotnet_wp_connection& connection) const;

      rx_result response (uint32_t msg_type, io::rx_const_io_buffer& buffer);

      void response_error (rx_result result);

      void timeouted ();

      uint32_t get_transaction_id () const;


  protected:

  private:


      uint32_t trans_id_;

      uint32_t timeout_;

      rx_result result_;

      rx_void_callback callback_;


};


} // namespace dotnet
} // namespace rx_internal



#endif
