

/****************************************************************************
*
*  runtime_internal\rx_subscription.h
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


#ifndef rx_subscription_h
#define rx_subscription_h 1



// rx_ptr
#include "lib/rx_ptr.h"



namespace sys_runtime {

namespace subscriptions {





class rx_subscription : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(rx_subscription);

  public:
      rx_subscription();


  protected:

  private:


};


} // namespace subscriptions
} // namespace sys_runtime



#endif
