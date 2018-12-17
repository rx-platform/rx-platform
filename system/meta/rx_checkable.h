

/****************************************************************************
*
*  system\meta\rx_checkable.h
*
*  Copyright (c) 2018 Dusan Ciric
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
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_checkable_h
#define rx_checkable_h 1


#include "system/server/rx_ns.h"

// rx_ser_lib
#include "lib/rx_ser_lib.h"

namespace rx_platform {
namespace objects {
namespace object_types {
class object_runtime;
} // namespace object_types

namespace blocks {
class complex_runtime_item;

} // namespace blocks
} // namespace objects
} // namespace rx_platform


using namespace rx_platform;
using namespace rx_platform::ns;


namespace rx_platform {

namespace meta {






class checkable_data 
{

  public:
      checkable_data();

      checkable_data (const string_type& name, const rx_node_id& id, const rx_node_id& parent, namespace_item_attributes attrs);


      bool serialize_node (base_meta_writer& stream, uint8_t type, const rx_value_union& value) const;

      bool deserialize_node (base_meta_reader& stream, uint8_t type, rx_value_union& value);

      bool check_in (base_meta_reader& stream);

      bool check_out (base_meta_writer& stream) const;

      bool serialize_checkable_definition (base_meta_writer& stream, uint8_t type) const;

      bool deserialize_checkable_definition (base_meta_reader& stream, uint8_t type);

      values::rx_value get_value () const;

      void construct (const string_type& name, const rx_node_id& id, rx_node_id type_id, bool system = false);

      bool get_system () const;


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