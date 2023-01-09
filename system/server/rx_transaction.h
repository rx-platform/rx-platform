

/****************************************************************************
*
*  system\server\rx_transaction.h
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


#ifndef rx_transaction_h
#define rx_transaction_h 1


#include "system/runtime/rx_objbase.h"
#include "lib/rx_const_size_vector.h"




namespace rx_internal {

namespace infrastructure {





template <typename resultT, typename... Args>
class rx_transaction 
{
  public:
	typedef typename std::function<resultT(int& next, Args...)> do_job_function_t;
	typedef typename std::function<void(resultT&&)> callback_function_t;
	struct one_transaction_item
	{
		do_job_function_t function_ptr;
		rx_thread_handle_t executer;
	};

  public:
      rx_transaction (rx_reference_ptr ref, std::initializer_list<one_transaction_item> function_list, callback_function_t&& callback)
            : lifetime_ref_(ref)
		  , callback_(std::move(callback))
		  , function_list_(function_list)
      {
      }


      resultT execute (Args... args)
      {
		  int next = -1;
		  resultT ret_value = do_job(next, args...);
		  if (ret_value)
		  {

		  }
		  return ret_value;
      }


  protected:

  private:


      rx_reference_ptr lifetime_ref_;

      callback_function_t callback_;

      const_size_vector<one_transaction_item> function_list_;


};


} // namespace infrastructure
} // namespace rx_internal



#endif
