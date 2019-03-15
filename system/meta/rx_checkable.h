

/****************************************************************************
*
*  system\meta\rx_checkable.h
*
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


#ifndef rx_checkable_h
#define rx_checkable_h 1



// initial version of an item
#define RX_INITIAL_ITEM_VERSION 0x10000

#include "system/server/rx_ns.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"

namespace rx_platform {
namespace runtime {
namespace objects {
class object_runtime;

} // namespace objects
} // namespace runtime
} // namespace rx_platform


using namespace rx_platform;
using namespace rx_platform::ns;


namespace rx_platform {

namespace meta {






class checkable_data 
{

  public:
      checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, namespace_item_attributes attrs, rx_time now = rx_time::now());

      checkable_data (namespace_item_attributes attrs = namespace_item_null, rx_time now = rx_time::now());


      bool check_in (base_meta_reader& stream);

      bool check_out (base_meta_writer& stream) const;

      bool serialize_checkable_definition (base_meta_writer& stream, uint8_t type, const string_type& object_type) const;

      bool deserialize_checkable_definition (base_meta_reader& stream, uint8_t type, string_type& object_type);

      values::rx_value get_value () const;

      void construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, ns::namespace_item_attributes& attributes);

      bool get_system () const;

      static rx_result_with<platform_item_ptr> deserialize_runtime_item (base_meta_reader& stream, uint8_t type);


      const rx_node_id& get_parent () const
      {
        return parent_;
      }


      uint32_t get_version () const
      {
        return version_;
      }


      rx_time get_created_time () const
      {
        return created_time_;
      }


      const rx_time get_modified_time () const
      {
        return modified_time_;
      }


      string_type get_name () const
      {
        return name_;
      }


      const rx_node_id& get_id () const
      {
        return id_;
      }


      namespace_item_attributes get_attributes () const
      {
        return attributes_;
      }



  protected:

  private:


      rx_node_id parent_;

      uint32_t version_;

      rx_time created_time_;

      rx_time modified_time_;

      string_type name_;

      rx_node_id id_;

      namespace_item_attributes attributes_;


};


} // namespace meta
} // namespace rx_platform



#endif
