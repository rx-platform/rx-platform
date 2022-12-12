

/****************************************************************************
*
*  lib\rx_io_addr.h
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


#ifndef rx_io_addr_h
#define rx_io_addr_h 1


#include "protocols/ansi_c/common_c/rx_protocol_handlers.h"
#include "lib/rx_lib.h"

// dummy
#include "dummy.h"



namespace rx {

namespace io {
rx_result fill_ip4_addr(const string_type& addr, uint16_t port, sockaddr_in* addr_struct);
string_type get_ip4_addr_string(const sockaddr_in* addr_struct);
string_type get_ip4_addr_string(const string_type& addr, uint16_t port);





class ip4_address : public protocol_address  
{

  public:
      ip4_address();

      ip4_address(const ip4_address &right);

      ip4_address (const sockaddr_in* addr);

      ip4_address (const sockaddr* addr);

      ip4_address (const string_type& addr, uint16_t port);

      ~ip4_address();

      bool operator==(const ip4_address &right) const;

      bool operator!=(const ip4_address &right) const;

      bool operator<(const ip4_address &right) const;

      bool operator>(const ip4_address &right) const;

      bool operator<=(const ip4_address &right) const;

      bool operator>=(const ip4_address &right) const;


      rx_result parse (const protocol_address* ep);

      const sockaddr_in* get_ip4_address () const;

      bool is_null () const;

      bool is_empty_ip4 () const;

      bool is_valid () const;

      string_type to_string () const;

      rx_result parse (const string_type& what);

      rx_result parse (const string_type& addr, uint16_t port);

      const sockaddr* get_address () const;

      ip4_address(ip4_address&& right) noexcept;
      ip4_address& operator=(const ip4_address& right);
      ip4_address& operator=(ip4_address&& right) noexcept;
  protected:

  private:


};






template <typename defT>
class numeric_address : public protocol_address  
{

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

      numeric_address(numeric_address&& right) noexcept;
      numeric_address& operator=(const numeric_address& right);
      numeric_address& operator=(numeric_address&& right) noexcept;
  protected:

  private:


};






class any_address : public protocol_address  
{

  public:
      any_address();

      any_address (const protocol_address* ep);

      ~any_address();


      bool is_null () const;

      string_type to_string () const;

      any_address(any_address&& right) noexcept;
      any_address(const any_address& right);
      any_address& operator=(const any_address& right);
      any_address& operator=(any_address&& right) noexcept;

      bool operator<(const any_address& right) const;
  protected:

  private:


};






class string_address : public protocol_address  
{

  public:
      string_address();

      string_address(const string_address &right);

      string_address (const string_type& val);

      ~string_address();

      bool operator==(const string_address &right) const;

      bool operator!=(const string_address &right) const;

      bool operator<(const string_address &right) const;

      bool operator>(const string_address &right) const;

      bool operator<=(const string_address &right) const;

      bool operator>=(const string_address &right) const;


      rx_result parse (const protocol_address* ep);

      string_type get_address () const;

      bool is_null () const;

      bool is_valid () const;

      string_type to_string () const;

      rx_result parse (const string_type& what);

      string_address(string_address&& right) noexcept;
      string_address& operator=(const string_address& right);
      string_address& operator=(string_address&& right) noexcept;
  protected:

  private:


};






class bytes_address : public protocol_address  
{

  public:
      bytes_address();

      bytes_address(const bytes_address &right);

      bytes_address (const byte_string& val);

      bytes_address (const void* pdata, size_t count);

      ~bytes_address();

      bool operator==(const bytes_address &right) const;

      bool operator!=(const bytes_address &right) const;

      bool operator<(const bytes_address &right) const;

      bool operator>(const bytes_address &right) const;

      bool operator<=(const bytes_address &right) const;

      bool operator>=(const bytes_address &right) const;


      rx_result parse (const protocol_address* ep);

      byte_string get_address () const;

      bool is_null () const;

      bool is_valid () const;

      string_type to_string () const;

      rx_result parse (const string_type& what);

      bytes_address(bytes_address&& right) noexcept;
      bytes_address& operator=(const bytes_address& right);
      bytes_address& operator=(bytes_address&& right) noexcept;
  protected:

  private:


};






class mac_address : public protocol_address  
{

  public:
      mac_address();

      mac_address(const mac_address &right);

      mac_address (const byte_string& val);

      mac_address (const uint8_t* pdata, size_t count);

      ~mac_address();

      bool operator==(const mac_address &right) const;

      bool operator!=(const mac_address &right) const;

      bool operator<(const mac_address &right) const;

      bool operator>(const mac_address &right) const;

      bool operator<=(const mac_address &right) const;

      bool operator>=(const mac_address &right) const;


      rx_result parse (const protocol_address* ep);

      byte_string get_address () const;

      bool is_null () const;

      bool is_valid () const;

      string_type to_string () const;

      rx_result parse (const string_type& what);


  protected:

  private:


};


} // namespace io
} // namespace rx



#endif
