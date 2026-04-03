

/****************************************************************************
*
*  sys_internal\rx_security\jwt_token_security.h
*
*  Copyright (c) 2020-2026 ENSACO Solutions doo
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


#ifndef jwt_token_security_h
#define jwt_token_security_h 1



// rx_platform_security
#include "sys_internal/rx_security/rx_platform_security.h"
// rx_security
#include "security/rx_security.h"



namespace rx_internal {

namespace rx_security {

namespace jwt_token {





class jwt_token_security_context : public rx_platform::security::security_context  
{

  public:
      jwt_token_security_context();

      ~jwt_token_security_context();


  protected:

  private:


};






class jwt_token_security_provider : public platform_security_provider  
{

  public:

  protected:

  private:


};


} // namespace jwt_token
} // namespace rx_security
} // namespace rx_internal



#endif
