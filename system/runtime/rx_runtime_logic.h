

/****************************************************************************
*
*  system\runtime\rx_runtime_logic.h
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


#ifndef rx_runtime_logic_h
#define rx_runtime_logic_h 1


#include "rx_runtime_helpers.h"
#include "rx_rt_struct.h"

// rx_rt_struct
#include "system/runtime/rx_rt_struct.h"
// rx_logic
#include "system/logic/rx_logic.h"



namespace rx_platform {

namespace runtime {

namespace logic_blocks {





class program_data 
{
public:
    program_data() = default;
    ~program_data() = default;
    program_data(const program_data&) = delete;
    program_data(program_data&&) noexcept = default;
    program_data& operator=(const program_data&) = delete;
    program_data& operator=(program_data&&) noexcept = default;
    operator bool() const
    {
        return program_ptr;
    }

  public:
      program_data (structure::runtime_item::smart_ptr&& rt, program_runtime_ptr&& var, const program_data& prototype);


      program_runtime_ptr program_ptr;


      structure::runtime_item::smart_ptr item;

      string_type name;


  protected:

  private:


};






class method_data 
{
public:
    method_data() = default;
    ~method_data() = default;
    method_data(const method_data&) = delete;
    method_data(method_data&&) noexcept = default;
    method_data& operator=(const method_data&) = delete;
    method_data& operator=(method_data&&) noexcept = default;
    operator bool() const
    {
        return method_ptr;
    }

  public:
      method_data (structure::runtime_item::smart_ptr&& rt, method_runtime_ptr&& var, const method_data& prototype);


      method_runtime_ptr method_ptr;

      structure::block_data inputs;

      structure::block_data outputs;


      structure::runtime_item::smart_ptr item;

      string_type name;


  protected:

  private:


};






class logic_holder 
{
    typedef const_size_vector<program_data> runtime_programs_type;
    typedef const_size_vector<method_data> runtime_methods_type;

  public:

      rx_result get_value (const string_type& path, rx_value& val, runtime_process_context* ctx) const;

      virtual rx_result initialize_logic (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_logic (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_logic (runtime::runtime_start_context& ctx);

      virtual rx_result stop_logic (runtime::runtime_stop_context& ctx);

      void fill_data (const data::runtime_values_data& data, runtime_process_context* ctx);

      void collect_data (data::runtime_values_data& data, runtime_value_type type) const;

      rx_result browse (const string_type& prefix, const string_type& path, const string_type& filter, std::vector<runtime_item_attribute>& items, runtime_process_context* ctx);

      rx_result serialize (base_meta_writer& stream, uint8_t type) const;

      rx_result deserialize (base_meta_reader& stream, uint8_t type);

      bool is_this_yours (const string_type& path) const;

      void process_programs (runtime_process_context& ctx);

      rx_result get_value_ref (const string_type& path, rt_value_ref& ref);


  protected:

  private:


      runtime_programs_type runtime_programs_;

      runtime_methods_type runtime_methods_;


};


} // namespace logic_blocks
} // namespace runtime
} // namespace rx_platform



#endif
