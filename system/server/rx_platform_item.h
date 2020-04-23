

/****************************************************************************
*
*  system\server\rx_platform_item.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_platform_item_h
#define rx_platform_item_h 1



// rx_ns
#include "system/server/rx_ns.h"
// rx_values
#include "lib/rx_values.h"



namespace rx_platform {
enum class runtime_value_type
{
    simple_runtime_value    = 0x00,

    ref_runtime_value       = 0x01,
    
    sources_runtime_value   = 0x02,
    filters_runtime_value   = 0x04,
    events_runtime_value    = 0x08,

    extended_runtime_value  = 0x0e,

    mappers_runtime_value   = 0x10,

    full_runtime_value      = 0x1f,

    resolved_runtime_value  = 0x20,
};

namespace ns {






class rx_platform_item 
{
public:
	typedef std::unique_ptr<rx_platform_item> smart_ptr;
	typedef std::unique_ptr<rx_platform_item> null_ptr;
private:
	rx_platform_item* smart_this() { return this; }
	const rx_platform_item* smart_this() const { return this; }

  public:
      rx_platform_item();

      ~rx_platform_item();


      virtual rx_item_type get_type_id () const = 0;

      virtual values::rx_value get_value () const = 0;

      virtual rx_result serialize (base_meta_writer& stream) const = 0;

      virtual rx_result deserialize (base_meta_reader& stream) = 0;

      virtual string_type get_name () const = 0;

      rx_result save () const;

      virtual const meta_data_t& meta_info () const = 0;

      virtual rx_result read_value (const string_type& path, rx_value& value) const = 0;

      virtual rx_result write_value (const string_type& path, rx_simple_value&& val, std::function<void(rx_result)> callback, api::rx_context ctx) = 0;

      virtual rx_result do_command (rx_object_command_t command_type) = 0;

      virtual rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items) = 0;

      virtual std::vector<rx_result_with<runtime_handle_t> > connect_items (const string_array& paths, runtime::operational::tags_callback_ptr monitor) = 0;

      virtual rx_result read_items (const std::vector<runtime_handle_t>& items, runtime::operational::tags_callback_ptr monitor, api::rx_context ctx) = 0;

      rx_result delete_item () const;

      virtual rx_platform_item::smart_ptr clone () const = 0;

      virtual string_type get_definition_as_json () const = 0;

      virtual rx_thread_handle_t get_executer () const = 0;

      virtual void fill_code_info (std::ostream& info, const string_type& name) = 0;

      virtual rx_result write_items (runtime_transaction_id_t transaction_id, const std::vector<std::pair<runtime_handle_t, rx_simple_value> >& items, runtime::operational::tags_callback_ptr monitor) = 0;

      virtual rx_result serialize_value (base_meta_writer& stream, runtime_value_type type) const = 0;

      virtual rx_result deserialize_value (base_meta_reader& stream, runtime_value_type type) = 0;


  protected:

  private:


      rx_reference<rx_platform_directory> parent_;


      locks::lockable item_lock_;


};


} // namespace ns
} // namespace rx_platform



#endif
