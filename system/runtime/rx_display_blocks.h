

/****************************************************************************
*
*  system\runtime\rx_display_blocks.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef rx_display_blocks_h
#define rx_display_blocks_h 1



// rx_displays
#include "system/displays/rx_displays.h"

#include "rx_rt_struct.h"


namespace rx_platform {

namespace runtime {

namespace display_blocks {





class display_data 
{
public:
    display_data() = default;
    ~display_data() = default;
    display_data(const display_data&) = delete;
    display_data(display_data&&) noexcept = default;
    display_data& operator=(const display_data&) = delete;
    display_data& operator=(display_data&&) noexcept = default;
    operator bool() const
    {
        return display_ptr;
    }

  public:
      display_data (structure::runtime_item::smart_ptr&& rt, display_runtime_ptr&& var, const display_data& prototype);


      display_runtime_ptr display_ptr;


      structure::runtime_item::smart_ptr item;

      string_type name;


  protected:

  private:


};






class displays_holder 
{
    typedef const_size_vector<display_data> displays_type;

  public:

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      virtual rx_result initialize_displays (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_displays (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_displays (runtime::runtime_start_context& ctx);

      virtual rx_result stop_displays (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx);

      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      bool is_this_yours (const string_type& path) const;

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);


  protected:

  private:


      displays_type displays_;


};


} // namespace display_blocks
} // namespace runtime
} // namespace rx_platform



#endif
