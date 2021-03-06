

/****************************************************************************
*
*  system\runtime\rx_write_transaction.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_write_transaction_h
#define rx_write_transaction_h 1



// rx_operational
#include "system/runtime/rx_operational.h"



namespace rx_platform {

namespace runtime {

namespace algorithms {





class write_item_transaction : public tag_blocks::rx_tags_callback  
{
    DECLARE_REFERENCE_PTR(write_item_transaction);

  public:
      write_item_transaction (rx_result_callback&& callback, rx_thread_handle_t whose);


      void items_changed (const std::vector<tag_blocks::update_item>& items);

      void transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<tag_blocks::update_item>&& items);

      rx_thread_handle_t get_target ();

      void write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result);


  protected:

  private:


      rx_result_callback callback_;

      rx_thread_handle_t whose_;


};


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform



#endif
