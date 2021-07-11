

/****************************************************************************
*
*  sys_internal\rx_protocol_messages.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_protocol_messages_h
#define rx_protocol_messages_h 1




namespace rx_internal {
namespace rx_protocol {
class rx_protocol_connection;

} // namespace rx_protocol
} // namespace rx_internal


#include "api/rx_namespace_api.h"
using namespace rx;


namespace rx_internal {

namespace rx_protocol {
typedef rx_reference<rx_protocol_connection> rx_protocol_connection_ptr;
namespace messages
{
class rx_request_message;
class rx_message_base;
}
typedef std::unique_ptr<messages::rx_request_message> request_message_ptr;
typedef std::unique_ptr<messages::rx_message_base> message_ptr;

namespace messages {

typedef uint16_t rx_message_type_t;
typedef int32_t rx_request_id_t;

const rx_message_type_t rx_get_type_request_id = 0x0001;
const rx_message_type_t rx_get_type_response_id = 0x8001;

const rx_message_type_t rx_query_request_id = 0x0002;
const rx_message_type_t rx_query_response_id = 0x8002;

const rx_message_type_t rx_browse_request_id = 0x0003;
const rx_message_type_t rx_browse_response_id = 0x8003;

const rx_message_type_t rx_get_runtime_request_id = 0x0004;
const rx_message_type_t rx_get_runtime_response_id = 0x8004;

const rx_message_type_t rx_connection_context_request_id = 0x0005;
const rx_message_type_t rx_connection_context_response_id = 0x8005;

const rx_message_type_t rx_set_type_request_id = 0x0006;
const rx_message_type_t rx_set_type_response_id = 0x8006;

const rx_message_type_t rx_update_type_request_id = 0x0007;
const rx_message_type_t rx_update_type_response_id = 0x8007;

const rx_message_type_t rx_set_runtime_request_id = 0x0008;
const rx_message_type_t rx_set_runtime_response_id = 0x8008;

const rx_message_type_t rx_update_runtime_request_id = 0x0009;
const rx_message_type_t rx_update_runtime_response_id = 0x8009;

const rx_message_type_t rx_browse_runtime_request_id = 0x0010;
const rx_message_type_t rx_browse_runtime_response_id = 0x8010;

const rx_message_type_t rx_make_directory_request_id = 0x0011;
const rx_message_type_t rx_make_directory_response_id = 0x8011;

const rx_message_type_t rx_remove_directory_request_id = 0x0012;
const rx_message_type_t rx_remove_directory_response_id = 0x8012;

const rx_message_type_t rx_delete_type_request_id = 0x0013;
const rx_message_type_t rx_delete_type_response_id = 0x8013;

const rx_message_type_t rx_delete_runtime_request_id = 0x0014;
const rx_message_type_t rx_delete_runtime_response_id = 0x8014;

const rx_message_type_t rx_proto_runtime_request_id = 0x0015;
const rx_message_type_t rx_proto_runtime_response_id = 0x8015;

const rx_message_type_t rx_code_request_id = 0x0016;
const rx_message_type_t rx_code_response_id = 0x8016; 

const rx_message_type_t rx_info_request_id = 0x0017;
const rx_message_type_t rx_info_response_id = 0x8017;

const rx_message_type_t rx_read_runtime_request_id = 0x0018;
const rx_message_type_t rx_read_runtime_response_id = 0x8018;

const rx_message_type_t rx_write_runtime_request_id = 0x0019;
const rx_message_type_t rx_write_runtime_response_id = 0x8019;


////////////////////////////////////////////////////////////////////////////////////////////////
// subscription messages constants
const rx_message_type_t rx_create_subscription_request_id = 0x0081;
const rx_message_type_t rx_create_subscription_response_id = 0x8081;

const rx_message_type_t rx_update_subscription_request_id = 0x0082;
const rx_message_type_t rx_update_subscription_response_id = 0x8082;

const rx_message_type_t rx_delete_subscription_request_id = 0x0083;
const rx_message_type_t rx_delete_subscription_response_id = 0x8083;

////////////////////////////////////////////////////////////////////////////////////////////////
// subscription items messages constants
const rx_message_type_t rx_add_items_request_id = 0x0084;
const rx_message_type_t rx_add_items_response_id = 0x8084;

const rx_message_type_t rx_modify_items_request_id = 0x0085;
const rx_message_type_t rx_modify_items_response_id = 0x8085;

const rx_message_type_t rx_remove_items_request_id = 0x0086;
const rx_message_type_t rx_remove_items_response_id = 0x8086;

const rx_message_type_t rx_read_items_request_id = 0x0087;
const rx_message_type_t rx_read_items_response_id = 0x8087;

const rx_message_type_t rx_write_items_request_id = 0x0088;
const rx_message_type_t rx_write_items_response_id = 0x8088;

const rx_message_type_t rx_execute_item_request_id = 0x0089;
const rx_message_type_t rx_execute_item_response_id = 0x8089;


/////////////////////////////////////////////////////////////////////////////////////////////////
// Async messages constants
const rx_message_type_t rx_subscription_items_notification_id = 0x7001;
const rx_message_type_t rx_subscription_write_done_id = 0x7002;
const rx_message_type_t rx_connection_notify_id = 0x7003;
////////////////////////////////////////////////////////////////////////////////////////////////
// keep alive message constant
const rx_message_type_t rx_keep_alive_message_id = 0xfffe;
////////////////////////////////////////////////////////////////////////////////////////////////
// error message constant
const rx_message_type_t rx_error_message_id = 0xffff;





class rx_message_base 
{

  public:
      rx_message_base();

      virtual ~rx_message_base();


      static rx_result init_messages ();

      static void deinit_messages ();

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream) = 0;

      virtual const string_type& get_type_name () = 0;

      virtual rx_message_type_t get_type_id () = 0;


      rx_request_id_t request_id;


  protected:

  private:


};






class error_message : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      uint32_t error_code;

      string_type error_text;

      static string_type type_name;

      static rx_message_type_t type_id;

	  error_message(const string_type& message, uint32_t code, rx_request_id_t request_id)
	  {
		  error_text = message;
		  error_code = code;
		  this->request_id = request_id;
	  }
	  error_message(const rx_result& result, uint32_t code, rx_request_id_t request_id)
	  {
		  bool first = true;
		  for (const auto& one : result.errors())
		  {
			  if (first)
				  first = false;
			  else
				  error_text += ", ";
			  error_text += one;
		  }
		  error_code = code;
		  this->request_id = request_id;
	  }
	  template<typename resT>
	  error_message(const rx_result_with<resT>& result, uint32_t code, rx_request_id_t request_id)
	  {
		  bool first = true;
		  for (const auto& one : result.errors())
		  {
			  if (first)
				  first = false;
			  else
				  error_text += ", ";
			  error_text += one;
		  }
		  error_code = code;
		  this->request_id = request_id;
	  }
  protected:

  private:


};







class rx_request_message : public rx_message_base  
{
	typedef std::map<rx_message_type_t, std::function<request_message_ptr()> > registered_messages_type;
	typedef std::map<string_type, std::function<request_message_ptr()> > registered_string_messages_type;

  public:

      virtual message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn) = 0;

      static rx_result_with<request_message_ptr> create_request_from_stream (rx_request_id_t& request_id, base_meta_reader& reader);

      static rx_result init_request_messages ();

      static void deinit_request_messages ();


  protected:

  private:

      static rx_result_with<request_message_ptr> create_request_message (const string_type& type);

      static rx_result_with<request_message_ptr> create_request_message (rx_message_type_t type);



      static rx_request_message::registered_string_messages_type registered_string_messages_;

      static rx_request_message::registered_messages_type registered_messages_;


};






class rx_connection_context_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      string_type directory;

      string_type application;

      string_type domain;

      uint32_t stream_version;


  protected:

  private:


};






class rx_connection_context_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      string_type directory;

      string_type application;

      string_type domain;

      rx_node_id application_id;

      rx_node_id domain_id;

      uint32_t stream_version;


  protected:

  private:


};






class rx_keep_alive_message : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class rx_connection_notify_message : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();

      rx_result deserialize (base_meta_reader& stream);


      static string_type type_name;

      static rx_message_type_t type_id;

      rx_node_id changed_id;

      string_type changed_path;


  protected:

  private:


};


} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal



#endif
