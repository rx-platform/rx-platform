

/****************************************************************************
*
*  protocols\xml\rx_xml_parsing.h
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


#ifndef rx_xml_parsing_h
#define rx_xml_parsing_h 1



/////////////////////////////////////////////////////////////
// logging macros for xml library
#define XML_LOG_INFO(src,lvl,msg) RX_LOG_INFO("XML",src,lvl,msg)
#define XML_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("XML",src,lvl,msg)
#define XML_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("XML",src,lvl,msg)
#define XML_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("XML",src,lvl,msg)
#define XML_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("XML",src,lvl,msg)
#define XML_LOG_TRACE(src,lvl,msg) RX_TRACE("XML",src,lvl,msg)

// dummy
#include "dummy.h"
// rx_protocol_templates
#include "system/runtime/rx_protocol_templates.h"
// rx_blocks_templates
#include "system/runtime/rx_blocks_templates.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace protocols {
namespace rx_xml {
class rx_xml_port;
class rx_xml_source;

} // namespace rx_xml
} // namespace protocols




namespace protocols {

namespace rx_xml {
typedef rx_reference<rx_xml_source> xml_source_ptr;





class rx_xml_endpoint : public rx::pointers::reference_object  
{

  public:
      rx_xml_endpoint (rx_reference<rx_xml_port> port);

      ~rx_xml_endpoint();


      rx_protocol_stack_endpoint* bind_endpoint ();

      void close_endpoint ();

      rx_reference<rx_xml_port> get_port ();


  protected:

  private:

      static rx_protocol_result_t received_function (rx_protocol_stack_endpoint* reference, recv_protocol_packet packet);

      static rx_protocol_result_t send_function (rx_protocol_stack_endpoint* reference, send_protocol_packet packet);



      rx_protocol_stack_endpoint stack_entry_;

      rx_reference<rx_xml_port> port_;


};







typedef rx_platform::runtime::io_types::ports_templates::slave_server_port_impl< rx_xml_endpoint  > rx_xml_port_base;






class rx_xml_port : public rx_xml_port_base  
{
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
XML parser port class. Implementation of XML parsing port");
    
    DECLARE_REFERENCE_PTR(rx_xml_port);

    typedef std::map<string_type, std::set<xml_source_ptr> > sources_type;
    typedef std::map<string_type, string_type> received_xmls_type;

  public:
      rx_xml_port();

      ~rx_xml_port();


      void stack_assembled ();

      rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      void XML_received (string_view_type data);

      void register_source (const string_type& addr, xml_source_ptr what);

      void unregister_source (const string_type& addr, xml_source_ptr what);


      static std::map<rx_node_id, rx_xml_port::smart_ptr> runtime_instances;


  protected:

  private:

      void parse_old_XML (string_view_type addr, string_view_type data, xml_source_ptr whose);



      sources_type sources_;


      string_array xml_addr_path_;

      string_type xml_addr_attr_;

      locks::slim_lock sources_lock_;

      received_xmls_type received_xmls_;


};







typedef rx_platform::runtime::blocks::blocks_templates::extern_source_impl< protocols::rx_xml::rx_xml_port  > rx_xml_source_base;






class rx_xml_source : public rx_xml_source_base  
{
    DECLARE_CODE_INFO("rx", 0, 5, 0, "\
Implementation of XML Parser Source");

    DECLARE_REFERENCE_PTR(rx_xml_source);

  public:
      rx_xml_source();

      ~rx_xml_source();


      rx_result initialize_source (runtime::runtime_init_context& ctx);

      void port_connected (port_ptr_t port);

      void port_disconnected (port_ptr_t port);

      const string_array& get_xml_path () const;

      void xml_changed (const string_type& val, rx_time now);

      void notify_error (rx_time now);


  protected:

  private:


      string_type xml_addr_;

      string_array xml_path_;


};


} // namespace rx_xml
} // namespace protocols



#endif
