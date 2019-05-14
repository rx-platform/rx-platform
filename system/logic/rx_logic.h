

/****************************************************************************
*
*  system\logic\rx_logic.h
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


#ifndef rx_logic_h
#define rx_logic_h 1



// rx_meta_data
#include "system/meta/rx_meta_data.h"
// rx_ptr
#include "lib/rx_ptr.h"
// soft_plc
#include "soft_logic/soft_plc.h"



namespace rx_platform {

namespace logic {





class program_runtime : public rx::pointers::reference_object  
{
	DECLARE_REFERENCE_PTR(program_runtime);
public:
	typedef program_runtime RType;

  public:
      program_runtime (const string_type& name, const rx_node_id& id, bool system = false);

      ~program_runtime();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);

      namespace_item_attributes get_attributes () const;

      bool save_program (base_meta_writer& stream, uint8_t type) const;

      bool load_program (base_meta_reader& stream, uint8_t type);

      void get_value (values::rx_value& val) const;

      platform_item_ptr get_item_ptr () const;

      values::rx_value get_value () const;

      rx_time get_created_time () const;

      string_type get_name () const;

      size_t get_size () const;

      virtual void fill_code_info (std::ostream& info, const string_type& name);


      sl_runtime::sl_program_holder& my_program ();

      const meta::meta_data& meta_info () const;


      static rx_item_type get_type_id ()
      {
        return type_id;
      }



      static rx_item_type type_id;


  protected:
      program_runtime();


  private:

      static namespace_item_attributes create_attributes_from_flags (bool system);



      sl_runtime::sl_program_holder my_program_;

      meta::meta_data meta_info_;


};






class ladder_program : public program_runtime  
{
	DECLARE_REFERENCE_PTR(ladder_program);


	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
ladder program class.");

  public:
      ladder_program (const string_type& name, const rx_node_id& id, bool system = false);

      ~ladder_program();


  protected:
      ladder_program();


      bool serialize (base_meta_writer& stream, uint8_t type) const;

      bool deserialize (base_meta_reader& stream, uint8_t type);


  private:


};


} // namespace logic
} // namespace rx_platform

namespace rx_platform
{
typedef logic::program_runtime::smart_ptr program_runtime_ptr;
}


#endif
