

/****************************************************************************
*
*  protocols\opcua\rx_opcua_attributes.h
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


#ifndef rx_opcua_attributes_h
#define rx_opcua_attributes_h 1


#include "protocols/opcua/rx_opcua_params.h"
using namespace protocols::opcua::common;

// rx_opcua_requests
#include "protocols/opcua/rx_opcua_requests.h"



namespace protocols {

namespace opcua {

namespace requests {

namespace opcua_attributes {





class opcua_read_request : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      double max_age;

      timestamps_return_type time_stamps;

      std::vector<read_value_id> to_read;


  protected:

  private:


};






class opcua_read_response : public opcua_response_base  
{

  public:
      opcua_read_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;


      std::vector<data_value> results;

      std::vector<diagnostic_info> diagnostics_info;

      opcua_read_response() = default;
  protected:

  private:


};






class opcua_write_request : public opcua_request_base  
{

  public:

      rx_node_id get_binary_request_id ();

      opcua_request_ptr create_empty () const;

      rx_result deserialize_binary (binary::ua_binary_istream& stream);

      opcua_response_ptr do_job (opcua_server_endpoint_ptr ep);


      std::vector<write_value> to_write;


  protected:

  private:


};






class opcua_write_response : public opcua_response_base  
{

  public:
      opcua_write_response (const opcua_request_base& req);


      rx_node_id get_binary_response_id ();

      opcua_response_ptr create_empty () const;

      rx_result serialize_binary (binary::ua_binary_ostream& stream) const;


      std::vector<opcua_result_t> results;

      std::vector<diagnostic_info> diagnostics_info;

      opcua_write_response() = default;

  protected:

  private:


};


} // namespace opcua_attributes
} // namespace requests
} // namespace opcua
} // namespace protocols



#endif
