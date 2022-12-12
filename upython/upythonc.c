

/****************************************************************************
*
*  upython\upythonc.c
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



#include <Windows.h>


// upythonc
#include "upythonc.h"


static host_data* host_obj;

bool runing_tasks = false;

void register_host(host_data* data)
{
    host_obj = data;
}



typedef struct rxplatform_task
{
    int a;

} rxplatform_task_str;
//
//void loop_handler(void) 
//{
// 
//    if (1)//runing_tasks)
//    {
//        return; // still running!!!
//    }
//    else
//    {
//        size_t count, i;
//        mp_obj_t* objs;
//
//        printf("\r\n**POzvan***\r\n");
//        printf("Cekam...\r\n");
//        WaitForSingleObject(host_obj->hevent, INFINITE);
//        host_obj->modules_callback(&count, &objs);
//        if (count > 0)
//        {
//            runing_tasks = true;
//            for (i = 0; i < count; i++)
//            {
//                mp_call_function_0(objs[i]);
//            }
//            runing_tasks = false;
//        }
//    }
//}
//HANDLE hevent = NULL;
//
//void loop_init(void) {
//    // Don't call ourselves recursively.
//
//    printf("\r\n**POzvan Init***\r\n");
//}
//void loop_deinit(void) {
//    // Don't call ourselves recursively.
//
//    printf("\r\n**POzvan Deinit***\r\n");
//}


STATIC mp_obj_t example_awaited_add_ints(mp_obj_t a_obj, mp_obj_t b_obj) 
{
    // Extract the ints from the micropython input objects
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);

    printf("Usao u funkciju\r\n");

    int result = a + b;
    // Calculate the addition and convert to MicroPython object.
    return mp_obj_new_int(a + b);
}
STATIC mp_obj_t execution_done(mp_obj_t result_obj)
{
    host_obj->module_done(result_obj);
    return mp_obj_new_bool(1);
}
// This is the function which will be called from Python as cexample.add_ints(a, b).
STATIC mp_obj_t loop_function(mp_obj_t timeout_obj)
{
    // Extract the ints from the micropython input objects.
    int timeout = mp_obj_get_int(timeout_obj);
    size_t count, i;
    mp_obj_t* objs;
    //WaitForSingleObject(host_obj->hevent, INFINITE);
    if(!host_obj->modules_callback(timeout, &count, &objs))
    {
        return mp_obj_new_bool(0);
    }
    else
    {
        runing_tasks = true;
        for (i = 0; i < count; i++)
        {
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0)
            {
                //mp_sched_schedule(objs[i], 0);
                mp_call_function_0(objs[i]);
                nlr_pop();
            }
            else
            { // uncaught exception
                mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
            }
        }
        runing_tasks = false;

        return mp_obj_new_bool(1);
    }
}


// This is the function which will be called from Python as cexample.add_ints(a, b).
STATIC mp_obj_t rx_read_function(mp_obj_t ppath)
{
    const char* path = mp_obj_str_get_str(ppath);
    printf("****Primio %s\r\n", path);
    return host_obj->module_read(path);

}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_1(loop_main_obj, loop_function);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(example_awaited_add_ints_obj, example_awaited_add_ints);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(execution_done_obj, execution_done);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rx_read_function_obj, rx_read_function);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// OBJECT example and testing

// this is the actual C-structure for our new object
typedef struct _test_hello_obj_t
{
    mp_obj_base_t base;         // base represents some basic information, like type
    uint8_t hello_number;       // a member created by us
} test_hello_obj_t;


STATIC const mp_obj_type_t test_hello_type;

mp_obj_t test_hello_make_new(const mp_obj_type_t* type, size_t n_args, size_t n_kw, const mp_obj_t* args)
{
    // this checks the number of arguments (min 1, max 1);
    // on error -> raise python exception
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    // create a new object of our C-struct type
    test_hello_obj_t* self = m_new_obj(test_hello_obj_t);
    // give it a type
    self->base.type = &test_hello_type;
    // set the member number with the first argument of the constructor
    self->hello_number = mp_obj_get_int(args[0]);
    return MP_OBJ_FROM_PTR(self);
}


STATIC void test_hello_print(const mp_print_t* print, mp_obj_t self_in, mp_print_kind_t kind)
{
    // get a ptr to the C-struct of the object
    test_hello_obj_t* self = MP_OBJ_TO_PTR(self_in);
    // print the number
    printf("Hello(%u)", self->hello_number);
}


STATIC mp_obj_t test_increment(mp_obj_t self_in) {
    test_hello_obj_t* self = MP_OBJ_TO_PTR(self_in);
    self->hello_number++;
    return mp_obj_new_int(self->hello_number);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(test_increment_obj, test_increment);


STATIC mp_obj_t test_decrement(mp_obj_t self_in) {
    test_hello_obj_t* self = MP_OBJ_TO_PTR(self_in);
    self->hello_number--;
    return mp_obj_new_int(self->hello_number);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(test_decrement_obj, test_decrement);



STATIC const mp_rom_map_elem_t hello_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_TestInc), MP_ROM_PTR(&test_increment_obj) },
    { MP_ROM_QSTR(MP_QSTR_TestDec), MP_ROM_PTR(&test_decrement_obj) }
};
STATIC MP_DEFINE_CONST_DICT(hello_locals_dict, hello_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    test_hello_type,
    MP_QSTR_TestHello,
    MP_TYPE_FLAG_NONE,
    make_new, test_hello_make_new,
    locals_dict, &hello_locals_dict
);


// OBJECT testing example end
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t rxplatform_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rxplatform) },
    { MP_ROM_QSTR(MP_QSTR_loop_function), MP_ROM_PTR(&loop_main_obj) },
    { MP_ROM_QSTR(MP_QSTR_example_awaited_add_ints), MP_ROM_PTR(&example_awaited_add_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_execution_done), MP_ROM_PTR(&execution_done_obj) },
    { MP_ROM_QSTR(MP_QSTR_rx_read), MP_ROM_PTR(&rx_read_function_obj) },
    { MP_ROM_QSTR(MP_QSTR_TestHello), MP_ROM_PTR(&test_hello_type) },
};
STATIC MP_DEFINE_CONST_DICT(rxplatform_module_globals, rxplatform_module_globals_table);

// Define module object.
const mp_obj_module_t rxplatform_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&rxplatform_module_globals,
};

    // Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_rxplatform, rxplatform_user_cmodule);




