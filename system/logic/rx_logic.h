

/****************************************************************************
*
*  system\logic\rx_logic.h
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


#ifndef rx_logic_h
#define rx_logic_h 1



// rx_process_context
#include "system/runtime/rx_process_context.h"
// rx_runtime_helpers
#include "system/runtime/rx_runtime_helpers.h"
// rx_ptr
#include "lib/rx_ptr.h"

namespace rx_platform {
namespace logic {
class program_context;

} // namespace logic
} // namespace rx_platform


using rx_platform::runtime::execute_data;


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

      virtual void process_program (logic::program_context* context, runtime::runtime_process_context& rt_context) = 0;

      virtual std::unique_ptr<program_context> create_program_context (program_context* parent_context, security::security_guard_ptr guard) = 0;


      string_type get_name () const
      {
        return name_;
      }



  protected:

  private:


      string_type name_;


};

typedef program_runtime::smart_ptr program_runtime_ptr;





class method_execution_context 
{
    friend class runtime::logic_blocks::method_data;

  public:
      method_execution_context (execute_data data, security::security_guard_ptr guard);


      void execution_complete (rx_result result, values::rx_simple_value data);

      void execution_complete (rx_result result);

      void execution_complete (values::rx_simple_value data);

      security::security_guard_ptr get_security_guard ();


  protected:

  private:


      runtime::execute_data data_;


      runtime::runtime_process_context* context_;

      runtime::logic_blocks::method_data* method_data_;

      security::security_guard_ptr security_guard_;


};







class method_runtime : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(method_runtime);

  public:
      method_runtime();

      method_runtime (const string_type& name, const rx_node_id& id);

      ~method_runtime();


      virtual rx_result initialize_runtime (runtime::runtime_init_context& ctx);

      virtual rx_result deinitialize_runtime (runtime::runtime_deinit_context& ctx);

      virtual rx_result start_runtime (runtime::runtime_start_context& ctx);

      virtual rx_result stop_runtime (runtime::runtime_stop_context& ctx);

      virtual method_execution_context* create_execution_context (execute_data data, security::security_guard_ptr guard);

      virtual rx_result execute (values::rx_simple_value args, method_execution_context* context);


      string_type get_name () const
      {
        return name_;
      }



  protected:

  private:


      string_type name_;


};

typedef method_runtime::smart_ptr method_runtime_ptr;





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






class program_context 
{

  public:
      program_context (program_context* parent, program_runtime_ptr runtime, security::security_guard_ptr guard);


      virtual void init_scan ();

      virtual bool schedule_scan (uint32_t interval);

      virtual void continue_scan () = 0;

      virtual void set_waiting () = 0;

      virtual void reset_waiting () = 0;

      security::security_guard_ptr get_security_guard ();

      virtual ~program_context() = default;
      program_context(const program_context&) = delete;
      program_context(program_context&&) = delete;
      program_context& operator=(const program_context&) = delete;
      program_context& operator=(program_context&&) = delete;
  protected:

  private:


      program_context *parent_;

      program_runtime_ptr runtime_;


      security::security_guard_ptr security_guard_;


};


} // namespace logic
} // namespace rx_platform



#endif
