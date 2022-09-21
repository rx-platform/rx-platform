

/****************************************************************************
*
*  upython\upython.cpp
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


#include "pch.h"


extern "C"
{

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/stackctrl.h"

}

// upython
#include "upython/upython.h"


static char heap[16384];


mp_obj_t execute_from_str(const char* str) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr src_name = 1/*MP_QSTR_*/;
        mp_lexer_t* lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&pt, src_name, false);
        mp_call_function_0(module_fun);
        nlr_pop();
        return 0;
    }
    else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        return (mp_obj_t)nlr.ret_val;
    }
}

int test_main(const char* str) {
    mp_stack_ctrl_init();
    // Initialized stack limit
    mp_stack_set_limit(40000 * (sizeof(void*) / 4));
    // Initialize heap
    gc_init(heap, heap + sizeof(heap));

    static mp_obj_t pystack[1024];
    mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
    // Initialize interpreter
    mp_init();

    if (execute_from_str(str)) {
        printf("Error\n");
    }
    return 0;
}

//mp_import_stat_t mp_import_stat(const char* path) {
//    return MP_IMPORT_STAT_NO_EXIST;
//}

//void nlr_jump_fail(void* val) {
//    printf("FATAL: uncaught NLR %p\n", val);
//    exit(1);
//}




namespace rx_platform {

namespace python {

// Class rx_platform::python::upython 

upython::upython()
{
}


upython::~upython()
{
}



void upython::try_stuff (const char* str)
{
    test_main(str);
}


// Class rx_platform::python::upy_command 

upy_command::upy_command()
{
}

upy_command::upy_command (const string_type& name, const rx_node_id& id)
{
}


upy_command::~upy_command()
{
}



rx_result upy_command::initialize_runtime (runtime::runtime_init_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result upy_command::deinitialize_runtime (runtime::runtime_deinit_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result upy_command::start_runtime (runtime::runtime_start_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

rx_result upy_command::stop_runtime (runtime::runtime_stop_context& ctx)
{
    return RX_NOT_IMPLEMENTED;
}

logic::method_execution_context* upy_command::create_execution_context (execute_data data)
{
    return nullptr;
}

rx_result upy_command::execute (data::runtime_values_data args, logic::method_execution_context* context)
{
    return RX_NOT_IMPLEMENTED;
}


} // namespace python
} // namespace rx_platform

