

/****************************************************************************
*
*  system\runtime\rx_write_transaction.cpp
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


#include "pch.h"


// rx_write_transaction
#include "system/runtime/rx_write_transaction.h"



namespace rx_platform {

namespace runtime {

namespace algorithms {

// Class rx_platform::runtime::algorithms::write_item_transaction 

write_item_transaction::write_item_transaction (rx_result_callback&& callback, rx_thread_handle_t whose)
      : callback_(std::move(callback)),
        whose_(whose)
{
}



void write_item_transaction::items_changed (const std::vector<tag_blocks::update_item>& items)
{
}

void write_item_transaction::transaction_complete (runtime_transaction_id_t transaction_id, rx_result result, std::vector<tag_blocks::update_item>&& items)
{
}

rx_thread_handle_t write_item_transaction::get_target ()
{
    return whose_;
}

void write_item_transaction::write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, rx_result&& result)
{
    callback_.set_arguments(std::move(result));
    callback_.call();
}


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

