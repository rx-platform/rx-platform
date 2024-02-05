

/****************************************************************************
*
*  model\rx_model_transactions.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/callbacks/rx_callback.h"


namespace rx_internal {

namespace model {

namespace transactions {
enum class executer_phase
{
    idle = 0,
    deleting_objects = 1,
    deleting_ports = 2,
    deleting_domains = 3,
    deleting_apps = 4,
    deleting_types = 5,
    deleting_directories = 6,
    building_directories = 7,
    building_types = 8,
    building_runtimes = 9,
    done = 10
};





class model_transaction_base 
{
public:
    

  public:
      model_transaction_base ();


      virtual rx_result do_step (executer_phase state, rx_platform::rx_result_callback callback) = 0;

      virtual rx_result do_step (executer_phase state) = 0;

      virtual void do_rollback (executer_phase state) = 0;

      virtual meta_data& meta_info () = 0;

      virtual bool is_remove (executer_phase state) const = 0;

      virtual bool is_create (executer_phase state, rx_item_type type) const = 0;

      virtual ~model_transaction_base() = default;
      model_transaction_base(const model_transaction_base&) = delete;
      model_transaction_base(model_transaction_base&&) = delete;
      model_transaction_base& operator=(const model_transaction_base&) = delete;
      model_transaction_base& operator=(model_transaction_base&&) = delete;
  protected:

  private:


};

typedef std::shared_ptr<model_transaction_base> meta_transaction_ptr_t;





template <class typeT>
class delete_type_transaction : public model_transaction_base  
{
public:
    typedef typename typeT::smart_ptr type_ptr_t;

  public:
      delete_type_transaction (const meta_data& info);


      rx_result do_step (executer_phase state, rx_platform::rx_result_callback callback);

      rx_result do_step (executer_phase state);

      void do_rollback (executer_phase state);

      meta_data& meta_info ();

      bool is_remove (executer_phase state) const;

      bool is_create (executer_phase state, rx_item_type type) const;


  protected:

  private:


      type_ptr_t type_ptr_;

      meta_data meta_info_;


};






class model_transactions_executer : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(model_transactions_executer);

    static constexpr size_t not_started_index = (size_t)(-1);
    static constexpr size_t done_index = (size_t)(-2);
        
    typedef std::vector<meta_transaction_ptr_t> transactions_type;
    typedef std::set<string_type> directory_list_type;
    typedef transactions_type::iterator current_it_type;

  public:
      model_transactions_executer (rx_platform::rx_result_callback callback);


      void add_transaction (meta_transaction_ptr_t what);

      void execute (rx_platform::rx_result_callback callback);

      void process (rx_result result);


  protected:

  private:

      rx_result consolidate_meta_data (meta_data& new_data, const meta_data& old_data);

      rx_result consolidate_meta_data (meta_data& data);

      rx_result do_delete_runtimes (executer_phase state);

      rx_result do_delete_types (executer_phase state);

      rx_result delete_directories ();

      rx_result build_directories ();

      rx_result do_build_types (executer_phase state);

      rx_result do_build_types (executer_phase state, rx_item_type type);

      rx_result do_build_runtimes (executer_phase state);

      rx_result do_build_runtimes (executer_phase state, rx_item_type type);



      transactions_type transactions_;


      rx_platform::rx_result_callback callback_;

      std::atomic<executer_phase> state_;

      rx_result result_;

      directory_list_type dirs_to_delete_;

      directory_list_type dirs_to_create_;

      current_it_type iterator_;


      template<typename T>
      void do_consolidate_for_types(T& container);
};


} // namespace transactions
} // namespace model
} // namespace rx_internal



#endif
