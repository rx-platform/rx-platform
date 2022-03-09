

/****************************************************************************
*
*  model\rx_model_transactions.cpp
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


#include "pch.h"


// rx_model_transactions
#include "model/rx_model_transactions.h"

#include "model/rx_meta_internals.h"
#include "model/rx_model_algorithms.h"
#include "sys_internal/rx_inf.h"


namespace rx_internal {

namespace model {

namespace transactions {

// Class rx_internal::model::transactions::model_transaction_base 

model_transaction_base::model_transaction_base (rx_reference_ptr anchor, rx_thread_handle_t target)
      : anchor_(anchor),
        target_(target)
{
}



void model_transaction_base::do_rollback ()
{
}


const rx_thread_handle_t model_transaction_base::get_target () const
{
  return target_;
}


// Parameterized Class rx_internal::model::transactions::delete_type_transaction 

template <class typeT>
delete_type_transaction<typeT>::delete_type_transaction (const rx_node_id& id, rx_reference_ptr anchor)
      : id_(id)
    , model_transaction_base(anchor, RX_DOMAIN_META)
{
}



template <class typeT>
rx_result delete_type_transaction<typeT>::do_transaction ()
{
    auto type_result = platform_types_manager::instance().get_type_repository<typeT>().get_type_definition(id_);
    if (!type_result)
        return type_result.errors();
    type_ptr_ = type_result.move_value();
    string_type path = type_ptr_->meta_info.path;
    rx_directory_ptr dir = rx_gate::instance().get_directory(path);
    if (!dir)
        return RX_INVALID_PATH;
    auto result = platform_types_manager::instance().get_type_repository<typeT>().delete_type(id_);
    if (result)
    {
        result = dir->delete_item(type_ptr_->meta_info.name);
        if (!result)
            platform_types_manager::instance().get_type_repository<typeT>().register_type(type_ptr_);
    }

    return result;
}

template <class typeT>
void delete_type_transaction<typeT>::do_rollback ()
{
}

template class delete_type_transaction<object_type>;
template class delete_type_transaction<port_type>;
template class delete_type_transaction<domain_type>;
template class delete_type_transaction<application_type>;
// Class rx_internal::model::transactions::model_transactions_executer 


void model_transactions_executer::add_transaction (model_transaction_ptr_t what)
{
    transactions_.emplace_back(what);
}

void model_transactions_executer::execute (rx_platform::rx_result_callback callback)
{
    if (state_ != executer_state::not_started)
    {
        callback(RX_INVALID_STATE);
        return;
    }
    callback_ = std::move(callback);
    current_index_ = 0;
    state_ = executer_state::running;
    process();
}

void model_transactions_executer::process ()
{
    auto current_thread = rx_thread_context();
    if (state_ == executer_state::running)
    {
        if (current_index_ < transactions_.size())
        {// go on with this
            rx_result result;
            while (current_index_ < transactions_.size()
                && current_thread == transactions_[current_index_]->get_target())
            {
                result = transactions_[current_index_]->do_transaction();
                if (!result)
                    break;
                current_index_++;
            }
            if (result)
            {
                if (current_index_ >= transactions_.size())
                {// we are done
                    RX_ASSERT(current_index_ >= transactions_.size());
                    state_ = executer_state::done;
                    callback_(true);
                }
                else
                {// more transactions to do
                    auto pool = infrastructure::server_runtime::instance().get_executer(transactions_[current_index_]->get_target());
                    if (pool)
                    {
                        pool->append(smart_this());
                    }
                    else
                    {
                        RX_ASSERT(false);
                        state_ = executer_state::done;
                        callback_("Invalid target parameter for transaction.");
                    }
                }
            }
            else // if(!result)
            {
                if (current_index_ == 0)
                {// we just started
                    state_ = executer_state::done;
                    callback_(std::move(result));
                }
                else
                {//  do the rollback
                    state_ = executer_state::rolling_back;
                    process();
                }
            }
        }
        else
        {
            // this is the only case where this code is executed
            RX_ASSERT(transactions_.empty());
            state_ = executer_state::done;
            callback_(true);
        }
    }
    else if (state_ == executer_state::rolling_back)
    {

    }
    else
    {
        // i don't think this should be ignored for now
        RX_ASSERT(false);
    }
}


} // namespace transactions
} // namespace model
} // namespace rx_internal

