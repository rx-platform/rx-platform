

/****************************************************************************
*
*  protocols\tls\rx_tls_mapping.h
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


#ifndef rx_tls_mapping_h
#define rx_tls_mapping_h 1


#include "sys_internal/rx_security/rx_x509_security.h"

using rx_internal::rx_security::x509::x509_security_context;
/////////////////////////////////////////////////////////////
// logging macros for http library
#define TLS_LOG_INFO(src,lvl,msg) RX_LOG_INFO("TLS",src,lvl,msg)
#define TLS_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("TLS",src,lvl,msg)
#define TLS_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("TLS",src,lvl,msg)
#define TLS_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("TLS",src,lvl,msg)
#define TLS_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("TLS",src,lvl,msg)
#define TLS_LOG_TRACE(src,lvl,msg) RX_TRACE("TLS",src,lvl,msg)


// dummy
#include "dummy.h"
// rx_transport_templates
#include "system/runtime/rx_transport_templates.h"

namespace protocols {
namespace rx_tls {
class tls_transport_port;

} // namespace rx_tls
} // namespace protocols


using rx_internal::rx_security::x509::x509_certificate_ptr;


namespace protocols {

namespace rx_tls {






class tls_transport_endpoint 
{

  public:
      tls_transport_endpoint (tls_transport_port* port, x509_certificate_ptr& cert);

      ~tls_transport_endpoint();


      rx_protocol_stack_endpoint* bind_endpoint (std::function<void(int64_t)> sent_func, std::function<void(int64_t)> received_func);

      void close_endpoint ();


      tls_transport_port* get_port ()
      {
        return port_;
      }


      rx_thread_handle_t get_executer () const
      {
        return executer_;
      }

      void set_executer (rx_thread_handle_t value)
      {
        executer_ = value;
      }



  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);

      static rx_protocol_result_t transport_connected (rx_protocol_stack_endpoint* reference, const protocol_address* local_address, const protocol_address* remote_address);



      rx_protocol_stack_endpoint stack_entry_;

      security::security_context_ptr peer_context_;


      tls_transport_port* port_;

      rx_thread_handle_t executer_;

      bool connected_;

      rx_cred_t cred_;

      bool aquired_;

      rx_auth_context_t auth_ctx_;

      void* out_buffer_;


};







typedef rx_platform::runtime::io_types::ports_templates::connection_transport_port_impl< tls_transport_endpoint  > tls_transport_port_base;






class tls_transport_port : public tls_transport_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
TLS protocol port class. Implementation of a rx-platform TLS protocol");

    DECLARE_REFERENCE_PTR(tls_transport_port);

  public:
      tls_transport_port();

      ~tls_transport_port();


      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);


  protected:

  private:


      rx_cred_t cred_;

      x509_certificate_ptr cert_;


};


} // namespace rx_tls
} // namespace protocols



#endif
