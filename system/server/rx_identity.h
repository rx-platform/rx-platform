

/****************************************************************************
*
*  system\server\rx_identity.h
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


#ifndef rx_identity_h
#define rx_identity_h 1



// rx_security
#include "lib/security/rx_security.h"



namespace rx_platform {

namespace runtime {

namespace items {





class security_context_holder 
{

  public:

      bool serialize (const string_type& name, base_meta_writer& stream) const;

      bool deserialize (const string_type& name, base_meta_reader& stream);

      rx_result create_context (const string_type& port, const string_type& location);

      void destory_context ();


      const rx_reference<rx::security::security_context>& get_context () const
      {
        return context_;
      }



  protected:

  private:


      rx_reference<rx::security::security_context> context_;


      uint8_t type_;

      byte_string data_;


};


} // namespace items
} // namespace runtime
} // namespace rx_platform



#endif
