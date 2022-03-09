

/****************************************************************************
*
*  sys_internal\rx_general_messages.h
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


#ifndef rx_general_messages_h
#define rx_general_messages_h 1



// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"



namespace rx_internal {

namespace rx_protocol {

namespace messages {

namespace general_messages {





class rx_system_info_response : public rx_message_base  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;

      string_type instance;

      rx_time start_time;

      rx_time current_time;

      string_type node;

      string_type platform;

      string_type library;

      string_type platform_host;

      string_type os_host;

      string_type os_itf;

      string_type os;

      string_type terminal;

      string_type compiler;

      string_type cpu;

      string_type memory;

      string_type http;


  protected:

  private:


};






class rx_system_info_request : public rx_request_message  
{

  public:

      rx_result serialize (base_meta_writer& stream) const;

      rx_result deserialize (base_meta_reader& stream);

      message_ptr do_job (api::rx_context ctx, rx_protocol_connection_ptr conn);

      const string_type& get_type_name ();

      rx_message_type_t get_type_id ();


      static string_type type_name;

      static rx_message_type_t type_id;


  protected:

  private:


};


} // namespace general_messages
} // namespace messages
} // namespace rx_protocol
} // namespace rx_internal



#endif
