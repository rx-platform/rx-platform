

/****************************************************************************
*
*  system\runtime\rx_write_transaction.cpp
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


#include "pch.h"


// rx_write_transaction
#include "system/runtime/rx_write_transaction.h"



namespace rx_platform {

namespace runtime {

namespace algorithms {

// Class rx_platform::runtime::algorithms::write_item_transaction 

write_item_transaction::write_item_transaction (write_result_callback_t&& callback)
      : callback_(std::move(callback))
{
}



void write_item_transaction::items_changed (const std::vector<update_item>& items)
{
}

void write_item_transaction::execute_complete (runtime_transaction_id_t transaction_id, uint32_t signal_level, runtime_handle_t item, rx_result result, data::runtime_values_data data)
{
}

void write_item_transaction::write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, uint32_t signal_level, rx_result&& result)
{
    callback_(signal_level, std::move(result));
}


// Class rx_platform::runtime::algorithms::execute_method_transaction 

execute_method_transaction::execute_method_transaction (execute_method_callback_t&& callback)
      : callback_(std::move(callback))
{
}



void execute_method_transaction::items_changed (const std::vector<update_item>& items)
{
}

void execute_method_transaction::execute_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, uint32_t signal_level, rx_result result, data::runtime_values_data data)
{
    callback_(signal_level, std::move(result), std::move(data));
}

void execute_method_transaction::write_complete (runtime_transaction_id_t transaction_id, runtime_handle_t item, uint32_t signal_level, rx_result&& result)
{
}


} // namespace algorithms
} // namespace runtime
} // namespace rx_platform

