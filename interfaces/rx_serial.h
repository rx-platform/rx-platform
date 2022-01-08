

/****************************************************************************
*
*  interfaces\rx_serial.h
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


#ifndef rx_serial_h
#define rx_serial_h 1



// dummy
#include "dummy.h"
// rx_ports_templates
#include "system/runtime/rx_ports_templates.h"
// rx_serial_io
#include "interfaces/rx_serial_io.h"

namespace rx_internal {
namespace interfaces {
namespace serial {
class serial_endpoint;
class serial_port;

} // namespace serial
} // namespace interfaces
} // namespace rx_internal


#include "sys_internal/rx_inf.h"
#include "system/runtime/rx_value_templates.h"


namespace rx_internal {

namespace interfaces {

namespace serial {





class serial_endpoint 
{
    enum class serial_state
    {
        not_active = 0,
        opened = 1,
        closed = 2
    };
    struct port_holder_t : public serial_comm_std_buffer
    {
        DECLARE_REFERENCE_PTR(serial_endpoint::port_holder_t);
        friend class serial_endpoint;
    private:
        serial_endpoint* whose = nullptr;
    protected:
        void release_buffer(buffer_ptr what);
        bool readed(const void* data, size_t count, rx_security_handle_t identity);
        void on_shutdown(rx_security_handle_t identity);
    public:
        port_holder_t(serial_endpoint* whose);
        port_holder_t(port_holder_t&& right) noexcept;
        void disconnect();
    };
    friend struct serial_endpoint::port_holder_t;
public:
    typedef rx_reference<port_holder_t> serial_ptr;

  public:
      serial_endpoint();

      ~serial_endpoint();


      rx_protocol_stack_endpoint* get_stack_endpoint ();

      runtime::items::port_runtime* get_port ();

      rx_protocol_result_t send_packet (send_protocol_packet packet);

      rx_result open (const serial_port_data_t& port_data, security::security_context_ptr identity, serial_port* port);

      rx_result close ();

      bool tick ();

      bool is_connected () const;

      void release_buffer (buffer_ptr what);


  protected:

  private:

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      void disconnected (rx_security_handle_t identity);

      bool readed (const void* data, size_t count, rx_security_handle_t identity);

      void start_timer (bool fire_now);

      void suspend_timer ();

      rx_result check_port_data ();



      rx_protocol_stack_endpoint stack_endpoint_;

      serial_port *my_port_;

      rx_reference<port_holder_t> serial_port_;


      serial_state current_state_;

      rx_timer_ptr timer_;

      locks::slim_lock state_lock_;

      serial_port_data_t port_data_;

      security::security_context_ptr identity_;


};







typedef rx_platform::runtime::io_types::ports_templates::extern_singleton_port_impl< serial_endpoint  > serial_port_base;






class serial_port : public serial_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 0, 1, "\
Serial port class. implementation of an serial (COM or TTY) port");

    typedef std::stack< buffer_ptr, std::vector<buffer_ptr> > free_buffers_type;
    DECLARE_REFERENCE_PTR(serial_port);

  public:
      serial_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      uint32_t get_reconnect_timeout () const;

      rx_result start_listen (const protocol_address* local_address, const protocol_address* remote_address);

      virtual rx_result_with<port_connect_result> start_connect (const protocol_address* local_address, const protocol_address* remote_address, rx_protocol_stack_endpoint* endpoint);

      rx_result stop_passive ();

      void release_buffer (buffer_ptr what);

      buffer_ptr get_buffer ();


  protected:

  private:


      std::unique_ptr<serial_endpoint> endpoint_;


      runtime::local_value<uint32_t> recv_timeout_;

      runtime::local_value<uint32_t> send_timeout_;

      runtime::local_value<uint32_t> reconnect_timeout_;

      free_buffers_type free_buffers_;

      locks::slim_lock free_buffers_lock_;

      serial_port_data_t port_data_;


};


} // namespace serial
} // namespace interfaces
} // namespace rx_internal



#endif
