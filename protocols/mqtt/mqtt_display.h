

/****************************************************************************
*
*  protocols\mqtt\mqtt_display.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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


#ifndef mqtt_display_h
#define mqtt_display_h 1



// rx_http_displays
#include "http_server/rx_http_displays.h"
// mqtt_simple
#include "protocols/mqtt/mqtt_simple.h"

using rx_internal::rx_http_server::http_displays::http_display_custom_content;
using rx_internal::rx_http_server::http_displays::rx_http_static_display;


namespace protocols {

namespace mqtt {

namespace mqtt_simple {






class mqtt_http_display : public rx_internal::rx_http_server::http_displays::rx_http_static_display  
{

    DECLARE_REFERENCE_PTR(mqtt_http_display);
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
display that implements static change in HTML\r\n\
and connects via mqtt.");
private:
    class http_display_resolver_user : public relation_subscriber
    {
    public:
        mqtt_http_display* my_display;
        void relation_connected(const string_type& name, const platform_item_ptr& item)
        {
            my_display->internal_port_connected(item);
        }
        void relation_disconnected(const string_type& name)
        {
            my_display->internal_port_disconnected();
        }
    };
    http_display_resolver_user resolver_user_;
    friend class http_display_resolver_user;

    mqtt_simple_client_port::smart_ptr my_port_;

  public:
      mqtt_http_display();

      ~mqtt_http_display();


      rx_result initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path);

      rx_result deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path);

      rx_result start_display (runtime::runtime_start_context& ctx, const string_type& disp_path);

      rx_result stop_display (runtime::runtime_stop_context& ctx, const string_type& disp_path);

      virtual void point_changed ();

      void timer_tick ();

      const string_array& get_point_replace () const;


  protected:

      virtual void fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path);


  private:

      void port_connected (mqtt_simple_client_port::smart_ptr port);

      void port_disconnected (mqtt_simple_client_port::smart_ptr port);

      bool internal_port_connected (const platform_item_ptr& item);

      void internal_port_disconnected ();



      rx_timer_ptr timer_;

      bool changed_;

      string_type topic_;

      string_type broker_url_;


};


} // namespace mqtt_simple
} // namespace mqtt
} // namespace protocols



#endif
