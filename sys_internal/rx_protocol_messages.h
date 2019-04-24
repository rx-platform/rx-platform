

/****************************************************************************
*
*  sys_internal\rx_protocol_messages.h
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_protocol_messages_h
#define rx_protocol_messages_h 1



// rx_queries
#include "system/meta/rx_queries.h"
// rx_meta_data
#include "system/meta/rx_meta_data.h"

namespace sys_internal {
namespace rx_protocol {
class rx_protocol_port;

} // namespace rx_protocol
} // namespace sys_internal


#include "api/rx_namespace_api.h"
using namespace rx;


namespace sys_internal {

namespace rx_protocol {
namespace messages
{
class rx_request_message;
class rx_message_base;
}
typedef std::unique_ptr<messages::rx_request_message> request_message_ptr;
typedef std::unique_ptr<messages::rx_message_base> message_ptr;
typedef reference<rx_protocol::rx_protocol_port> rx_protocol_port_ptr;

namespace messages {

typedef uint16_t rx_message_type_t;




class rx_message_base 
{

  public:
      rx_message_base();

      virtual ~rx_message_base();


      static rx_result init_messages ();

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream) = 0;

      virtual const string_type& get_type_name () = 0;

      virtual rx_message_type_t get_type_id () = 0;


      int requestId;


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


      uint32_t errorCode;

      string_type errorMessage;

      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class browse_response_message : public rx_message_base  
{
	typedef std::vector<std::pair<rx_item_type, meta::meta_data> > browse_result_items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      browse_result_items_type items;


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};







class rx_request_message : public rx_message_base  
{
	typedef std::map<rx_message_type_t, std::function<request_message_ptr()> > registered_messages_type;
	typedef std::map<string_type, std::function<request_message_ptr()> > registered_string_messages_type;

  public:

      virtual message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port) = 0;

      static rx_result_with<request_message_ptr> create_request_from_json (const string_type& data);

      static rx_result init_request_messages ();


  protected:

  private:

      static rx_result_with<request_message_ptr> create_request_message (const string_type& type);

      static rx_result_with<request_message_ptr> create_request_message (rx_message_type_t type);



      static rx_request_message::registered_string_messages_type registered_string_messages_;

      static rx_request_message::registered_messages_type registered_messages_;


};






class browse_request_message : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      string_type path;

      string_type filter;

      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class query_request_message : public rx_request_message  
{
	typedef std::vector<meta::query_ptr> queries_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      queries_type queries;


      static string_type type_name;

      bool intersection;

      static rx_message_type_t type_id;


  protected:

  private:


};






class query_response_message : public rx_message_base  
{
	typedef std::vector<std::pair<rx_item_type, meta::meta_data> > query_result_items_type;

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      query_result_items_type items;


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class get_type_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      meta_data meta;

      rx_item_type item_type;

      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:
	  template<typename T>
	  message_ptr do_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>);
	  template<typename T>
	  message_ptr do_simple_job(api::rx_context ctx, rx_protocol_port_ptr port, tl::type2type<T>);

};






template <class itemT>
class get_type_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      typename itemT::smart_ptr item;

      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};


} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal



#endif
