

/****************************************************************************
*
*  interfaces\rx_transaction_limiter.h
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


#ifndef rx_transaction_limiter_h
#define rx_transaction_limiter_h 1


#include "lib/rx_const_size_vector.h"

// dummy
#include "dummy.h"
// rx_port_helpers
#include "system/runtime/rx_port_helpers.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"
// rx_io_buffers
#include "system/runtime/rx_io_buffers.h"
// rx_io_addr
#include "lib/rx_io_addr.h"

namespace rx_internal {
namespace interfaces {
namespace ports_lib {
class transaction_limiter_port;

} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal


using namespace rx_platform::runtime;


namespace rx_internal {

namespace interfaces {

namespace ports_lib {





class limiter_transaction_data 
{
    constexpr static int packet_sent = 0;
    constexpr static int has_buffer = 1;
    constexpr static int active_transaction = 2;

  public:
      limiter_transaction_data();

      limiter_transaction_data (const send_protocol_packet& to_send, transaction_limiter_port* port);


      void transaction_sent ();

      void reinit (const send_protocol_packet& to_send);

      void reinit ();

      bool is_write () const;


      rx_platform::runtime::io_types::rx_io_buffer buffer;

      rx::io::any_address to;

      rx::io::any_address from;


      send_protocol_packet packet;

      std::bitset<sizeof(int_fast8_t)*8> status;

      uint64_t arrived_tick;

      uint64_t sent_tick;


  protected:

  private:


};






class limiter_endpoint 
{
    typedef std::unique_ptr<limiter_transaction_data> transaction_ptr_t;
    typedef std::map<uint_fast8_t, std::queue<transaction_ptr_t> > transactions_queue_type;
    typedef std::vector<transaction_ptr_t> pending_transactions_type;
    typedef std::vector<transaction_ptr_t> empty_transactions_type;
    struct removed_transaction_data
    {
        uint64_t ticks = 0;
        bool write = false;
        bool found = false;
    };
    struct timeouts_type
    {
        uint64_t read;
        uint64_t write;        
    };

  public:
      limiter_endpoint (transaction_limiter_port* port);

      ~limiter_endpoint();


      rx_protocol_stack_endpoint* bind (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      runtime::items::port_runtime* get_port ();


  protected:

  private:

      rx_protocol_result_t connected (rx_session* session);

      rx_protocol_result_t send_packet (send_protocol_packet packet);

      rx_protocol_result_t received_packet (recv_protocol_packet packet);

      rx_protocol_result_t disconnected (rx_session* session, rx_protocol_result_t reason);

      void timer_fired ();

      size_t pending_count () const;

      void place_pending (transaction_ptr_t data, size_t new_limit);

      limiter_endpoint::removed_transaction_data remove_pending_packet_id (rx_packet_id_type id);

      limiter_endpoint::removed_transaction_data remove_pending ();

      void send_next ();

      int handle_timer ();

      int get_queue_size ();

      void read_timeouts ();

      limiter_endpoint::transaction_ptr_t create_transaction (send_protocol_packet packet);

      void release_transaction (transaction_ptr_t trans);

      limiter_endpoint(const limiter_endpoint&) = delete;
      limiter_endpoint(limiter_endpoint&&) noexcept = default;
      limiter_endpoint& operator=(const limiter_endpoint&) = delete;
      limiter_endpoint& operator=(limiter_endpoint&&) noexcept = default;

      rx_protocol_stack_endpoint stack_endpoint_;

      transaction_limiter_port *port_;

      transactions_queue_type transactions_queue_;

      pending_transactions_type pending_transactions_;

      empty_transactions_type empty_transactions_;


      locks::slim_lock lock_;

      rx_timer_ptr timer_;

      uint64_t next_timer_tick_;

      timeouts_type timeouts_;


};







typedef rx_platform::runtime::io_types::ports_templates::transport_port_impl< limiter_endpoint  > transaction_limiter_port_base;


struct limit_options_t
{
    bool ignore_zeros;
    bool use_packet_id;
    int limit;
};




class transaction_limiter_port : public transaction_limiter_port_base  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Transaction limiter port, limits the transactions sent by master/client.\r\n\
Use limit value 1 for enabling half-duplex connection, or 0 for no limit.");

    DECLARE_REFERENCE_PTR(transaction_limiter_port);

  public:
      transaction_limiter_port();


      rx_result initialize_runtime (runtime_init_context& ctx);

      void set_queue_size (int size);

      limit_options_t get_limit_options () const;

      uint32_t get_read_timeout () const;

      uint32_t get_write_timeout () const;


      rx_platform::runtime::io_types::master_port_status status;


  protected:

  private:


      remote_local_value<int> limit_;

      remote_local_value<bool> ignore_zeros_;

      remote_local_value<bool> use_packet_id_;

      remote_local_value<uint32_t> read_timeout_;

      remote_local_value<uint32_t> write_timeout_;

      remote_owned_value<int32_t> queue_size_;


};


} // namespace ports_lib
} // namespace interfaces
} // namespace rx_internal



#endif
