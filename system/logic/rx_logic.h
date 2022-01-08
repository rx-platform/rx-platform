

/****************************************************************************
*
*  system\logic\rx_logic.h
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
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


#ifndef rx_logic_h
#define rx_logic_h 1


#include "system/meta/rx_meta_data.h"

// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
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
      program_runtime();

      program_runtime (const string_type& name, const rx_node_id& id);

      ~program_runtime();


      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);

      rx_result save_program (base_meta_writer& stream, uint8_t type) const;

      bool load_program (base_meta_reader& stream, uint8_t type);

      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      void process_program (runtime::runtime_process_context& ctx);


      sl_runtime::sl_program_holder& my_program ();


      string_type get_name () const
      {
        return name_;
      }



  protected:

  private:


      sl_runtime::sl_program_holder my_program_;


      string_type name_;


};






class ladder_program : public program_runtime  
{
	DECLARE_REFERENCE_PTR(ladder_program);


	DECLARE_CODE_INFO("rx", 0, 1, 0, "\
ladder program class.");

  public:
      ladder_program();

      ladder_program (const string_type& name, const rx_node_id& id);

      ~ladder_program();


  protected:

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);


  private:


};






class method_runtime : public rx::pointers::reference_object  
{

  public:
      method_runtime();

      method_runtime (const string_type& name, const rx_node_id& id);

      ~method_runtime();


      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);


      string_type get_name () const
      {
        return name_;
      }



  protected:

  private:


      string_type name_;


};

typedef method_runtime::smart_ptr method_runtime_ptr;

} // namespace logic
} // namespace rx_platform



#endif
