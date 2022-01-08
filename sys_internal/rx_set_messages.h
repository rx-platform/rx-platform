

/****************************************************************************
*
*  sys_internal\rx_set_messages.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_set_messages_h
#define rx_set_messages_h 1



// rx_query_messages
#include "sys_internal/rx_query_messages.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"

namespace rx_internal {
namespace rx_protocol {
namespace messages {
namespace set_messages {
class protocol_type_creator_base;
class protocol_runtime_creator_base;

} // namespace set_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal




namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace set_messages {





class delete_type_request : public rx_request_message  
{
	template<typename T>
	message_ptr do_job(api::rx_context ctx, rx_protocol_connection_ptr conn, tl::type2type<T>);
	template<typename T>
	message_ptr do_simple_job(api::rx_context ctx, rx_protocol_connection_ptr conn, tl::type2type<T>);
    message_ptr do_relation_job(api::rx_context ctx, rx_protocol_connection_ptr port);
    message_ptr do_data_job(api::rx_context ctx, rx_protocol_connection_ptr port);

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      rx_item_reference reference;

      rx_item_type item_type;

      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






class delete_type_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class protocol_type_creator_base 
{

  public:
      virtual ~protocol_type_creator_base();


      virtual message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, bool create, const rx_update_type_data* data) = 0;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta) = 0;


  protected:

  private:


};






template <class itemT>
class protocol_type_creator : public protocol_type_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, bool create, const rx_update_type_data* data);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


      typename itemT::smart_ptr item;


  protected:

  private:


};






class set_type_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


      std::unique_ptr<protocol_type_creator_base> creator_;


};






template <class itemT>
class set_type_response : public query_messages::type_response_message<itemT>  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






class update_type_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;

      rx_update_type_data update_data;


  protected:

  private:


      std::unique_ptr<protocol_type_creator_base> updater_;


};






template <class itemT>
class update_type_response : public query_messages::type_response_message<itemT>  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






template <class itemT>
class protocol_simple_type_creator : public protocol_type_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, bool create, const rx_update_type_data* data);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


      typename itemT::smart_ptr item;


  protected:

  private:


};






class protocol_relation_type_creator : public protocol_type_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, bool create, const rx_update_type_data* data);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


      object_types::relation_type::smart_ptr item;


  protected:

  private:


};






class protocol_data_type_creator : public protocol_type_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, bool create, const rx_update_type_data* data);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


      data_type_ptr item;


  protected:

  private:


};






class delete_runtime_request : public rx_request_message  
{
	template<typename T>
	message_ptr do_job(api::rx_context ctx, rx_protocol_connection_ptr conn, tl::type2type<T>);

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      rx_item_reference reference;

      rx_item_type item_type;

      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






class delete_runtime_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class protocol_runtime_creator_base 
{

  public:

      //	create_type values:
      //	0 - prototype runtime
      //	1 - create runtime
      //	2 - update runtime
      virtual message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, int create_type, const rx_update_runtime_data* data) = 0;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta) = 0;


  protected:

  private:


};






template <class itemT>
class protocol_runtime_creator : public protocol_runtime_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn, rx_request_id_t request, int create_type, const rx_update_runtime_data* data);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


      typename itemT::instance_data_t item;


  protected:

  private:


};






class set_runtime_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


      std::unique_ptr<protocol_runtime_creator_base> creator_;


};






template <class itemT>
class set_runtime_response : public query_messages::runtime_response_message<itemT>  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






class update_runtime_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;

      rx_update_runtime_data update_data;


  protected:

  private:


      std::unique_ptr<protocol_runtime_creator_base> updater_;


};






template <class itemT>
class update_runtime_response : public query_messages::runtime_response_message<itemT>  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






template <class itemT>
class prototype_runtime_response : public query_messages::runtime_response_message<itemT>  
{

  public:

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};






class prototype_runtime_request : public rx_request_message  
{
	template<typename T>
	message_ptr do_job(api::rx_context ctx, rx_protocol_connection_ptr conn, tl::type2type<T>);

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      rx_item_type item_type;

      static string_type type_name;

      static rx_message_type_t type_id;

      meta_data info;


  protected:

  private:


      std::unique_ptr<protocol_runtime_creator_base> creator_;


};






class read_runtime_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      rx_item_reference reference;

      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


};






class read_runtime_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};


} // namespace set_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal



#endif
