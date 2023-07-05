

/****************************************************************************
*
*  system\server\rx_identity.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_identity_h
#define rx_identity_h 1



// rx_security
#include "security/rx_security.h"



namespace rx_platform {

namespace runtime {

namespace items {






class security_context_holder 
{

  public:

      rx_result create_context (const string_type& port, const string_type& location, const byte_string& data, security::security_context_ptr& ctx);


  protected:

  private:


      uint8_t type_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
