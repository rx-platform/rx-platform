

/****************************************************************************
*
*  enterprise\rx_enterprise.h
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


#ifndef rx_enterprise_h
#define rx_enterprise_h 1


#include "system/server/rx_server.h"



/////////////////////////////////////////////////////////////
// logging macros for http library
#define ENT_LOG_INFO(src,lvl,msg) RX_LOG_INFO("ENT",src,lvl,msg)
#define ENT_LOG_WARNING(src,lvl,msg) RX_LOG_WARNING("ENT",src,lvl,msg)
#define ENT_LOG_ERROR(src,lvl,msg) RX_LOG_ERROR("ENT",src,lvl,msg)
#define ENT_LOG_CRITICAL(src,lvl,msg) RX_LOG_CRITICAL("ENT",src,lvl,msg)
#define ENT_LOG_DEBUG(src,lvl,msg) RX_LOG_DEBUG("ENT",src,lvl,msg)
#define ENT_LOG_TRACE(src,lvl,msg) RX_TRACE("ENT",src,lvl,msg)


namespace rx_internal {

namespace enterprise {
typedef std::map<string_type, string_type> enterprise_args_t;





class enterprise_callback 
{

  public:
      virtual ~enterprise_callback();


      virtual void read_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data) = 0;

      virtual void write_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data) = 0;

      virtual void execute_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data) = 0;

      virtual void query_complete (uint64_t trans_id, uint32_t result, string_view_type cont_type, string_view_type data) = 0;


  protected:

  private:


};







class enterprise_interface 
{

  public:
      virtual ~enterprise_interface();


      virtual void begin_read (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, enterprise_callback* callback, rx_reference_ptr anchor) = 0;

      virtual void begin_write (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor) = 0;

      virtual void begin_execute (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor) = 0;

      virtual void begin_query (uint64_t trans_id, string_view_type path, const enterprise_args_t& args, string_view_type data, enterprise_callback* callback, rx_reference_ptr anchor) = 0;

      virtual string_view_type get_name () = 0;


  protected:

  private:


};






class enterprise_manager 
{
    typedef std::map<string_type, enterprise_interface*> interfaces_type;

  public:
      ~enterprise_manager();


      static enterprise_manager& instance ();

      rx_result init_interfaces (const configuration_data_t& config);

      rx_result deinit_interfaces ();

      enterprise_interface* get_interface (const string_type& name);

      string_array list_interfaces ();


  protected:

  private:
      enterprise_manager();



      interfaces_type interfaces_;


      static enterprise_manager* g_object_;


};


} // namespace enterprise
} // namespace rx_internal



#endif
