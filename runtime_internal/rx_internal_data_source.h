

/****************************************************************************
*
*  runtime_internal\rx_internal_data_source.h
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


#ifndef rx_internal_data_source_h
#define rx_internal_data_source_h 1



// rx_data_source
#include "runtime_internal/rx_data_source.h"
// rx_subscription
#include "runtime_internal/rx_subscription.h"



namespace sys_runtime {

namespace data_source {





class internal_data_subscription : public subscriptions::rx_subscription_callback  
{
  public:
	typedef std::unique_ptr<internal_data_subscription> smart_ptr;

  public:

  protected:

  private:


      rx_reference<subscriptions::rx_subscription> my_subscription_;


};






class internal_data_source : public data_source  
{
	typedef std::map<uint32_t, internal_data_subscription::smart_ptr > subscriptions_type;

  public:
      internal_data_source (const string_type& path);


      void add_item (const string_type& path, uint32_t rate, value_handle_extended& handle);

      void remove_item (const value_handle_extended& handle);

      bool is_empty () const;


  protected:

  private:


      subscriptions_type subscriptions_;


};


} // namespace data_source
} // namespace sys_runtime



#endif
