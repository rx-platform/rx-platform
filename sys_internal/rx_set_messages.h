

/****************************************************************************
*
*  sys_internal\rx_set_messages.h
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


#ifndef rx_set_messages_h
#define rx_set_messages_h 1



// rx_query_messages
#include "sys_internal/rx_query_messages.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"

namespace sys_internal {
namespace rx_protocol {
namespace messages {
namespace set_messages {
class protocol_type_creator_base;
class protocol_runtime_creator_base;

} // namespace set_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal




namespace sys_internal {

namespace rx_protocol {

namespace messages {

namespace set_messages {





class protocol_type_creator_base 
{

  public:
      virtual ~protocol_type_creator_base();


      virtual message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create) = 0;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta) = 0;


  protected:

  private:


};






template <class itemT>
class protocol_simple_type_creator : public protocol_type_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


      typename itemT::smart_ptr item;


  protected:

  private:


};






template <class itemT>
class protocol_type_creator : public protocol_type_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create);

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

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

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






class update_type_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


      std::unique_ptr<protocol_type_creator_base> updater_;


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






class set_runtime_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


      std::unique_ptr<protocol_runtime_creator_base> creator_;


};






class update_runtime_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static uint16_t type_id;


  protected:

  private:


      std::unique_ptr<protocol_runtime_creator_base> updater_;


};






class protocol_runtime_creator_base 
{

  public:

      virtual message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create) = 0;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta) = 0;


  protected:

  private:


};






template <class itemT>
class protocol_runtime_creator : public protocol_runtime_creator_base  
{

  public:

      message_ptr do_job (api::rx_context ctx, rx_protocol_port_ptr port, rx_request_id_t request, bool create);

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream, const meta::meta_data& meta);


  protected:

  private:


      data::runtime_values_data values_;

      typename itemT::instance_data_t instance_data_;

      meta::meta_data meta_;


};


} // namespace set_messages
} // namespace messages
} // namespace rx_protocol
} // namespace sys_internal



#endif
