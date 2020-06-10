

/****************************************************************************
*
*  lib\rx_io_addr.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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


#ifndef rx_io_addr_h
#define rx_io_addr_h 1


#include "protocols/ansi_c/common_c/rx_protocol_base.h"
#include "lib/rx_lib.h"




namespace rx {

namespace io {
rx_result fill_ip4_addr(const string_type& addr, uint16_t port, sockaddr_in* addr_struct);
string_type get_ip4_addr_string(const sockaddr_in* addr_struct);





class ip4_address 
{
    struct endpoint_storage
    {
        size_t size;
        sockaddr_in addr;
    };

  public:
      ip4_address();

      ip4_address(const ip4_address &right);

      ip4_address (const sockaddr_in* addr);

      ~ip4_address();

      bool operator==(const ip4_address &right) const;

      bool operator!=(const ip4_address &right) const;

      bool operator<(const ip4_address &right) const;

      bool operator>(const ip4_address &right) const;

      bool operator<=(const ip4_address &right) const;

      bool operator>=(const ip4_address &right) const;


      rx_result parse (const protocol_address* ep);

      const sockaddr_in* get_address () const;

      bool is_null () const;

      bool is_valid () const;

      string_type to_string () const;

      rx_result parse (const string_type& what);

      const protocol_address* to_protocol_address () const;


  protected:

  private:


      sockaddr_in addr_;

      endpoint_storage storage_;


};






template <typename defT>
class numeric_address 
{
    struct endpoint_storage
    {
        size_t size;
        defT value;
    };

  public:
      numeric_address();

      numeric_address(const numeric_address< defT > &right);

      numeric_address (defT val);

      ~numeric_address();

      bool operator==(const numeric_address< defT > &right) const;

      bool operator!=(const numeric_address< defT > &right) const;

      bool operator<(const numeric_address< defT > &right) const;

      bool operator>(const numeric_address< defT > &right) const;

      bool operator<=(const numeric_address< defT > &right) const;

      bool operator>=(const numeric_address< defT > &right) const;


      rx_result parse (const protocol_address* ep);

      defT get_address () const;

      bool is_null () const;

      bool is_valid () const;

      string_type to_string () const;

      rx_result parse (const string_type& what);

      const protocol_address* to_protocol_address () const;


  protected:

  private:


      endpoint_storage storage_;


};


} // namespace io
} // namespace rx



#endif
