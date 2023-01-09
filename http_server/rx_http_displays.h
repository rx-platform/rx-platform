

/****************************************************************************
*
*  http_server\rx_http_displays.h
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


#ifndef rx_http_displays_h
#define rx_http_displays_h 1



// rx_displays
#include "system/displays/rx_displays.h"
// rx_value_point
#include "runtime_internal/rx_value_point.h"

#include "system/runtime/rx_value_templates.h"
using namespace rx_platform::runtime;
using namespace rx_platform;


namespace rx_internal {

namespace rx_http_server {
class http_display_handler;

namespace http_displays {





class http_display_point : public sys_runtime::data_source::value_point_impl  
{

  public:
      http_display_point();


      const string_type& get_str_value () const;


  protected:

  private:

      void value_changed (const rx_value& val);



      string_type str_value_;


};






struct http_display_custom_content 
{


      string_type head_content;

      string_type body_begin_content;

      string_type body_end_content;

      std::map<string_type, string_type> mapped_content;

  public:

  protected:

  private:


};







class rx_http_display_base : public rx_platform::displays::display_runtime  
{
    DECLARE_REFERENCE_PTR(rx_http_display_base);

    struct one_point_data
    {
        string_type point_id;
        size_t start_idx;
        size_t end_idx;
        string_type point_path;
        std::unique_ptr<http_display_point> point;
    };
    typedef std::vector<one_point_data> connected_points_type;
    friend class ::rx_internal::rx_http_server::http_display_handler;

  public:
      rx_http_display_base();

      rx_http_display_base (const string_type& name, const rx_node_id& id);

      ~rx_http_display_base();


      rx_result handle_request (rx_platform::http::http_request& req, rx_platform::http::http_response& resp);

      virtual rx_result handle_request_internal (rx_platform::http::http_request& req, rx_platform::http::http_response& resp) = 0;

      rx_result register_display (runtime::runtime_start_context& ctx, const string_type& disp_path);

      rx_result unregister_display (runtime::runtime_stop_context& ctx, const string_type& disp_path);

      static void fill_globals ();


  protected:

      rx_result parse_display_data (runtime::runtime_init_context& ctx, const string_type& disp_path, string_type& html_data, const http_display_custom_content& custom);

      rx_result connect_points (runtime::runtime_start_context& ctx, const string_type& disp_path);

      rx_result disconnect_points (runtime::runtime_stop_context& ctx, const string_type& disp_path);

      string_type get_dynamic_content (const string_type& html_data);

      string_type collect_json_data ();


  private:

      string_type preprocess_static (const string_type& content, const http_displays::http_display_custom_content& custom, const meta::meta_data& meta_info);

      string_array get_paths_to_register (runtime::runtime_process_context* ctx, const string_type& disp_path);



      connected_points_type connected_points_;


      char points_buffer_[0x200];

      remote_owned_value<int64_t> req_;

      remote_owned_value<int64_t> failed_;

      remote_owned_value<float> max_req_time_;

      remote_owned_value<float> last_req_time_;

      static std::map<string_type, string_type> globals_;

      string_type version_check_;


};






class rx_http_static_display : public rx_http_display_base  
{
    DECLARE_REFERENCE_PTR(rx_http_static_display);
    DECLARE_CODE_INFO("rx", 0, 5, 0, "\
display that implements static change in html\r\n\
actuality first display implemented good for testing.)");


  public:
      rx_http_static_display();

      rx_http_static_display (const string_type& name, const rx_node_id& id);

      ~rx_http_static_display();


      rx_result initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path);

      rx_result deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path);

      rx_result start_display (runtime::runtime_start_context& ctx, const string_type& disp_path);

      rx_result stop_display (runtime::runtime_stop_context& ctx, const string_type& disp_path);

      rx_result handle_request_internal (rx_platform::http::http_request& req, rx_platform::http::http_response& resp);


  protected:

  private:

      virtual void fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path);



      string_type html_data_;


};






class http_displays_repository 
{
    struct display_data_t
    {
        rx_http_display_base::smart_ptr display_ptr;
        rx_thread_handle_t executer;
    };
    typedef std::map<string_type, display_data_t> registered_displays_type;

  public:
      http_displays_repository();

      ~http_displays_repository();


      rx_result register_display (const string_type& path, rx_http_display_base::smart_ptr who);

      rx_result unregister_display (const string_type& path, rx_http_display_base::smart_ptr who);

      rx_result_with<rx_http_display_base::smart_ptr> get_display (const string_type& path, rx_thread_handle_t& executer);


  protected:

  private:


      registered_displays_type registered_displays_;


      locks::slim_lock displays_lock_;


};






class rx_http_standard_display : public rx_http_static_display  
{
    DECLARE_REFERENCE_PTR(rx_http_standard_display);
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
display that implements standard runtime view\r\n\
this implementation is set by default.)");

  public:
      rx_http_standard_display();

      rx_http_standard_display (const string_type& name, const rx_node_id& id);

      ~rx_http_standard_display();


  protected:

  private:

      void fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path);

      void fill_div (std::ostream& stream, const string_type& rt_name, const string_type& path, const data::runtime_values_data& data);



};






class rx_http_simple_display : public rx_http_static_display  
{
    DECLARE_REFERENCE_PTR(rx_http_simple_display);
    DECLARE_CODE_INFO("rx", 0, 1, 0, "\
display that implements simple runtime view\r\n\
it enables quick and easy http implementation.)");

  public:
      rx_http_simple_display();

      rx_http_simple_display (const string_type& name, const rx_node_id& id);

      ~rx_http_simple_display();


  protected:

  private:

      void fill_contents (http_display_custom_content& content, runtime::runtime_init_context& ctx, const string_type& disp_path);



};


} // namespace http_displays
} // namespace rx_http_server
} // namespace rx_internal



#endif
