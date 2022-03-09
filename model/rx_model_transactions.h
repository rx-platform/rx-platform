

/****************************************************************************
*
*  model\rx_model_transactions.h
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


#ifndef rx_model_transactions_h
#define rx_model_transactions_h 1



// rx_job
#include "system/threads/rx_job.h"

#include "system/callbacks/rx_callback.h"


namespace rx_internal {

namespace model {

namespace transactions {





class model_transaction_base 
{

  public:
      model_transaction_base (rx_reference_ptr anchor, rx_thread_handle_t target);


      virtual rx_result do_transaction () = 0;

      virtual void do_rollback ();


      const rx_thread_handle_t get_target () const;

      virtual ~model_transaction_base() = default;
      model_transaction_base(const model_transaction_base&) = delete;
      model_transaction_base(model_transaction_base&&) = delete;
      model_transaction_base& operator=(const model_transaction_base&) = delete;
      model_transaction_base& operator=(model_transaction_base&&) = delete;
  protected:

  private:


      rx_reference_ptr anchor_;

      rx_thread_handle_t target_;


};

typedef std::shared_ptr<model_transaction_base> model_transaction_ptr_t;





template <class typeT>
class delete_type_transaction : public model_transaction_base  
{
public:
    typedef typename typeT::smart_ptr type_ptr_t;

  public:
      delete_type_transaction (const rx_node_id& id, rx_reference_ptr anchor);


      rx_result do_transaction ();

      void do_rollback ();


  protected:

  private:


      rx_node_id id_;

      type_ptr_t type_ptr_;


};






class model_transactions_executer : public rx_platform::jobs::job  
{
    DECLARE_REFERENCE_PTR(model_transactions_executer);

    typedef std::vector<model_transaction_ptr_t> transactions_type;
    static constexpr size_t not_started_index = (size_t)(-1);
    static constexpr size_t done_index = (size_t)(-2);
    enum class executer_state
    {
        not_started = 0,
        running = 1,
        rolling_back = 2,
        done = 3
    };

  public:

      void add_transaction (model_transaction_ptr_t what);

      void execute (rx_platform::rx_result_callback callback);

      void process ();


  protected:

  private:


      transactions_type transactions_;


      rx_platform::rx_result_callback callback_;

      size_t current_index_;

      std::atomic<executer_state> state_;

      rx_result result_;


};


} // namespace transactions
} // namespace model
} // namespace rx_internal



#endif
