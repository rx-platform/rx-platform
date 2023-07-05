

/****************************************************************************
*
*  upython\upy_internal.h
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


#ifndef upy_internal_h
#define upy_internal_h 1



#ifdef UPYTHON_SUPPORT

// rx_ptr
#include "lib/rx_ptr.h"
// rx_thread
#include "system/threads/rx_thread.h"

namespace rx_platform {
namespace python {
class upy_method_execution_context;

} // namespace python
} // namespace rx_platform


#include "system/logic/rx_logic.h"


#pragma warning(push)
// zero sized array is used!!!
#pragma warning(disable:4200)

extern "C"
{

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"

}

#pragma warning(pop)


namespace rx_platform {

namespace python {

string_type get_python_value(const std::variant<rx_simple_value, std::vector<rx_simple_value> >& val);


struct upy_module_process_data
{
    upy_module_process_data(mp_obj_t exec)
        : to_execute(exec)
    {
    }
    mp_obj_t to_execute;
    std::vector<runtime_transaction_id_t> trans_ids;
};




class upy_module : public rx::pointers::reference_object  
{
    DECLARE_REFERENCE_PTR(upy_module);

    typedef std::vector<upy_method_execution_context*> contexts_type;
    typedef std::map<runtime_transaction_id_t, upy_method_execution_context*> pending_contexts_type;

  public:
      upy_module (const string_type& script, const string_type& eval_script);

      ~upy_module();


      virtual upy_module_process_data process_module ();

      virtual bool transaction_ended (runtime_transaction_id_t id, mp_obj_t result, mp_obj_t exc);

      void push_context (upy_method_execution_context* ctx);


  protected:

  private:


      string_type script_;

      string_type eval_script_;

      mp_obj_t py_module_;

      contexts_type contexts_;

      pending_contexts_type pending_contexts_;

      locks::slim_lock contexts_lock_;


};

typedef upy_module::smart_ptr upy_module_ptr;






class upy_thread : public threads::thread, 
                   	public threads::job_thread  
{
    typedef std::vector<upy_module_ptr> modules_type;
    typedef std::map<runtime_transaction_id_t, upy_module_ptr> waited_modules_type;

  public:
      ~upy_thread();


      void include (upy_module_ptr what);

      void end ();

      rx_result start_script (const std::vector<std::pair<string_type, string_type> >& modules);

      void stop_script ();

      bool get_modules (uint32_t timeout, std::vector<mp_obj_t>& modules);

      bool transaction_ended (runtime_transaction_id_t id, mp_obj_t result, mp_obj_t exc);

      static upy_thread& instance ();

      void append (job_ptr pjob);

      void read (const string_type& path);


  protected:

      uint32_t handler ();

      bool wait (std::vector<upy_module_ptr>& queued, uint32_t timeout = RX_INFINITE);


  private:
      upy_thread();



      modules_type modules_;

      waited_modules_type waited_modules_;


      locks::slim_lock jobs_lock_;

      locks::event has_job_;

      std::vector<std::pair<string_type, string_type> > file_modules_;


};


} // namespace python
} // namespace rx_platform

#endif


#endif
