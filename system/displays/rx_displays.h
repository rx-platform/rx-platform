

/****************************************************************************
*
*  system\displays\rx_displays.h
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


#ifndef rx_displays_h
#define rx_displays_h 1



// rx_ptr
#include "lib/rx_ptr.h"

#include "system/runtime/rx_runtime_helpers.h"
#include "system/http_support/rx_http_request.h"


namespace rx_platform {

namespace displays {





class display_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(display_runtime);

  public:
      display_runtime();

      display_runtime (const string_type& name, const rx_node_id& id);

      ~display_runtime();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      bool save_display (base_meta_writer& stream, uint8_t type) const;

      bool load_display (base_meta_reader& stream, uint8_t type);

      virtual rx_result initialize_display (runtime::runtime_init_context& ctx, const string_type& disp_path);

      virtual rx_result deinitialize_display (runtime::runtime_deinit_context& ctx, const string_type& disp_path);

      virtual rx_result start_display (runtime::runtime_start_context& ctx, const string_type& disp_path);

      virtual rx_result stop_display (runtime::runtime_stop_context& ctx, const string_type& disp_path);

      virtual rx_result register_display (runtime::runtime_start_context& ctx, const string_type& disp_path);

      virtual rx_result unregister_display (runtime::runtime_stop_context& ctx, const string_type& disp_path);

      virtual rx_result handle_request (rx_platform::http::http_request& req, rx_platform::http::http_response& resp);


  protected:

  private:


};


} // namespace displays
} // namespace rx_platform



#endif
