

/****************************************************************************
*
*  sys_internal\rx_internal_protocol.h
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


#ifndef rx_internal_protocol_h
#define rx_internal_protocol_h 1



// dummy
#include "dummy.h"
// rx_protocol_messages
#include "sys_internal/rx_protocol_messages.h"
// rx_objbase
#include "system/runtime/rx_objbase.h"

namespace sys_internal {
namespace rx_protocol {
class rx_protocol_port;

} // namespace rx_protocol
} // namespace sys_internal




namespace sys_internal {

namespace rx_protocol {





class rx_json_protocol : public rx_protocol_stack_entry  
{
	friend class rx_protocol_port;

  public:
      rx_json_protocol();


      rx_result send_string (const string_type& what);


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_entry* reference, protocol_endpoint* end_point, rx_const_packet_buffer* buffer);



      rx_reference<rx_protocol_port> my_port_;


};






class rx_protocol_subscription 
{

  public:
      rx_protocol_subscription();


  protected:

  private:


};







class rx_protocol_port : public rx_platform::runtime::objects::port_runtime  
{
	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
system protocol port class. basic implementation of a rx-platform protocol");

	DECLARE_REFERENCE_PTR(rx_protocol_port);

	typedef std::vector<std::unique_ptr<rx_protocol_subscription> > subscriptions_type;

  public:
      rx_protocol_port();


      void data_received (const string_type& data);

      rx_protocol_stack_entry* get_stack_entry ();

      void data_processed (message_ptr result);

      rx_result set_current_directory (const string_type& path);


      const string_type& get_current_directory_path () const
      {
        return current_directory_path_;
      }



  protected:

  private:


      rx_json_protocol stack_entry_;

      subscriptions_type subscriptions_;


      rx_directory_ptr current_directory_;

      string_type current_directory_path_;


};


} // namespace rx_protocol
} // namespace sys_internal



#endif
