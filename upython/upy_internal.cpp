

/****************************************************************************
*
*  upython\upy_internal.cpp
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


#include "pch.h"

#ifdef UPYTHON_SUPPORT

// upy_method
#include "upython/upy_method.h"
// upy_internal
#include "upython/upy_internal.h"


#include "upythonc.h"
#include "upy_method.h"
#include "model/rx_meta_internals.h"

host_data g_host_data{};

#include "upy_method.h"
using rx_platform::python::upy_method_execution_context;

std::vector<mp_obj_t> g_modules;

int get_modules(uint32_t timeout, size_t* count, mp_obj_t** objs)
{
    g_modules.clear();
    bool ret = rx_platform::python::upy_thread::instance().get_modules(timeout, g_modules);
    if (!ret)
        return 0;
    if (!g_modules.empty())
    {
        *objs = &g_modules[0];
        *count = g_modules.size();
    }
    else
    {
        *count = 0;
    }
    return 1;
}
void module_done(mp_obj_t result)
{
    uint32_t id = (uint32_t)mp_obj_get_int(result);
    bool ret = rx_platform::python::upy_thread::instance().transaction_ended(id, 0, 0);
}

mp_obj_t module_read(const char* path)
{
    rx_platform::python::upy_thread::instance().read(path);
    return mp_const_none;
}
mp_obj_t module_write(const char* path, mp_obj_t what)
{
    return mp_obj_new_bool(0);
}

extern "C"
{
    int c_get_modules(int timeout, size_t* count, mp_obj_t** objs)
    {
        return get_modules((uint32_t)timeout, count, objs);
    }

    void c_module_done(mp_obj_t result)
    {
        return module_done(result);
    }
    mp_obj_t c_module_read(const char* path)
    {
        return module_read(path);
    }
    mp_obj_t c_module_write(const char* path, mp_obj_t what)
    {
        return module_write(path, what);
    }
}

static char heap[65536];

struct python_string_printer
{
    mp_print_t print_;
    string_type buff_;
    python_string_printer()
    {
        print_.data = this;
        print_.print_strn = [](void* data, const char* str, size_t len)
        {
            python_string_printer* self = (python_string_printer*)data;
            self->buff_.append(str, len);
        };
    }
    mp_print_t* get_print_object()
    {
        return &print_;
    }
    const string_type& str()
    {
        return buff_;
    }
};

string_type get_exception_string(mp_obj_t exc)
{
    std::ostringstream ss;
    if (mp_obj_is_exception_instance(exc)) {

        size_t un, * values;
        mp_obj_exception_get_traceback(exc, &un, &values);
        int n = (int)un;// avoid warning!!!
        if (n > 0) {
            assert(n % 3 == 0);

            for (int i = n - 3; i >= 0; i -= 3) {
                ss << "Traceback (most recent call last):\n";
                ss << "File \"" << qstr_str(values[i]);
#if MICROPY_ENABLE_SOURCE_LINE
                ss << "  File \"" << qstr_str(values[i])
                    << "\", line " << (int)values[i + 1];
#else
                ss << "  File \"%s\"", qstr_str(values[i]));
#endif
                // the block name can be NULL if it's unknown
                qstr block = values[i + 2];
                if (block == MP_QSTRnull) {
                    ss << "\n";
                }
                else {
                    ss << ", in " << qstr_str(block) << "\n";
                }
            }
        }
        python_string_printer printer;
        mp_obj_print_helper(printer.get_print_object(), exc, PRINT_EXC);
        ss << printer.str();
        return ss.str();
    }
    else
    {
        return "Non specific error occurred!!!";
    }
}

rx_result_with<mp_obj_t> compile_from_str(const char* name, const char* str)
{
    //nlr_buf_t nlr;
    //if (nlr_push(&nlr) == 0) {
        mp_obj_t ret = nullptr;
        qstr src_name = qstr_from_str(name);// 1/*MP_QSTR_*/;//MP_QSTR__lt_string_gt_;
        mp_lexer_t* lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);// MP_PARSE_EVAL_INPUT);
        ret = mp_compile(&pt, src_name, true);
        return ret;
   // }
   // else {
        // uncaught exception
       /* mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        if (exc)
        {
            return get_exception_string(exc);
        }
        else
        {
            return "Internal error occurred";
        }*/
   // }
}


mp_obj_t execute_function(mp_obj_t py_module)
{
 //   nlr_buf_t nlr;
 //   if (nlr_push(&nlr) == 0) {
        mp_call_function_0(py_module);
        return 0;
 //   }
 //   else {
        // uncaught exception
  //      mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
 //       return exc;
 //   }
}


mp_obj_t eval_str(const char* name, const char* str) {

    qstr src_name = 1/*MP_QSTR_*/;
    if (name)
        qstr src_name = qstr_from_str(name);
    mp_lexer_t* lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_EVAL_INPUT);
    mp_obj_t module_fun = mp_compile(&pt, src_name, false);
    mp_call_function_0(module_fun);
    return 0;
}

mp_obj_t execute_from_str(const char* name, const char* str) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0)
    {
        qstr src_name = 1/*MP_QSTR_*/;
        if(name)
            qstr src_name = qstr_from_str(name);
        mp_lexer_t* lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&pt, src_name, false);
        mp_call_function_0(module_fun);
        nlr_pop();
        return 0;
    }
    else
    {
        // uncaught exception
        mp_obj_t exc= MP_OBJ_FROM_PTR(nlr.ret_val);
        mp_obj_print_exception(&mp_plat_print, exc);
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

    if (execute_from_str(nullptr, str)) {
        printf("Error\n");
    }
    return 0;
}


void init_script_engine()
{
    mp_stack_ctrl_init();
    // Initialized stack limit
    mp_stack_set_limit(40000 * (sizeof(void*) / 4));
    // Initialize heap
    gc_init(heap, heap + sizeof(heap));

    static mp_obj_t pystack[1024];
    mp_pystack_init(pystack, &pystack[MP_ARRAY_SIZE(pystack)]);
    // Initialize interpreter
    mp_init();

}
rx_result execute_script(const char* str)
{
    mp_obj_t exc = execute_from_str(nullptr, str);
    if (exc)
    {
        std::ostringstream ss;
        if (mp_obj_is_exception_instance(exc)) {
            size_t n, * values;
            mp_obj_exception_get_traceback(exc, &n, &values);
            if (n > 0) {
                assert(n % 3 == 0);

                for (size_t i = n - 3; i >= 0; i -= 3) {
                    ss << "Traceback (most recent call last):\n";
                    ss << "File \"" << qstr_str(values[i]);
#if MICROPY_ENABLE_SOURCE_LINE
                    ss << (int)values[i + 1];
#else
#endif
                    // the block name can be NULL if it's unknown
                    qstr block = values[i + 2];
                    if (block == MP_QSTRnull) {
                        ss << "\n";
                    }
                    else {
                        ss << ", in " << qstr_str(block) << "\n";
                    }
                }
            }
            return ss.str();
        }
        else
        {
            return "Non specific error occurred!!!";
        }
    }
    else
    {
        return true;
    }
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

string_type get_python_value(const std::variant<rx_simple_value, std::vector<rx_simple_value> >& val)
{

    if (std::holds_alternative<rx_simple_value>(val))
    {
        const rx_simple_value& rx_val = std::get<rx_simple_value>(val);
        if (rx_val.is_string())
        {
            return "\""s + rx_val.get_string() + "\"";
        }
    }
    return "None";
}

// Class rx_platform::python::upy_thread 

upy_thread::upy_thread()
      : has_job_(false)
    , threads::thread("upython", RX_DOMAIN_PYTHON)
{
}


upy_thread::~upy_thread()
{
}



uint32_t upy_thread::handler ()
{
    register_host(&g_host_data);
    init_script_engine();

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0)
    {

        for (const auto& one : file_modules_)
        {
            // skip main for now
            if (one.first == "main")
                continue;

            execute_from_str(one.first.c_str(), one.second.c_str());
        }

        for (const auto& one : file_modules_)
        {
            // skip main for now
            if (one.first != "main")
                continue;

            execute_from_str(nullptr, one.second.c_str());
        }

    }
    else
    {
        // uncaught exception
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        mp_obj_print_exception(&mp_plat_print, exc);
        return -1;// (mp_obj_t)nlr.ret_val;
    }

    return 0;
}

void upy_thread::include (upy_module_ptr what)
{
    bool was_empty = false;
    {
        locks::auto_lock_t dummy(&jobs_lock_);
        was_empty = modules_.empty();
        modules_.push_back(what);
    }

    if (was_empty)
        has_job_.set();
}

void upy_thread::end ()
{
    include(upy_module_ptr::null_ptr);
    wait_handle();
}

rx_result upy_thread::start_script (const std::vector<std::pair<string_type, string_type> >& modules)
{
    file_modules_ = modules;

    g_host_data.hevent = CreateEvent(NULL, TRUE, FALSE, NULL);

    g_host_data.modules_callback = c_get_modules;
    g_host_data.module_done = c_module_done;
    g_host_data.module_read = c_module_read;
    g_host_data.module_write = c_module_write;
    register_host(&g_host_data);

    start();

    /*auto job = rx_create_job<std::function<void()> >()(rx_reference_ptr(), std::function<void()>([this]()
        {
        }));*/

    return true;
}

void upy_thread::stop_script ()
{
    end();
    if (g_host_data.hevent)
        CloseHandle(g_host_data.hevent);
}

bool upy_thread::wait (std::vector<upy_module_ptr>& queued, uint32_t timeout)
{
    if (RX_WAIT_0 != has_job_.wait_handle(timeout))
        return false;

    locks::auto_lock_t dummy(&jobs_lock_);

    RX_ASSERT(!modules_.empty());
    queued.assign(modules_.begin(), modules_.end());
    modules_.clear();

    return true;
}

bool upy_thread::get_modules (uint32_t timeout, std::vector<mp_obj_t>& modules)
{
    std::vector<upy_module_ptr> queued;

    modules.clear();

    if (wait(queued, timeout))
    {
        for (auto& obj : queued)
        {
            if (!obj)
            {
                return false;
            }
            // process module
            auto module_result = obj->process_module();
            // add function for execution if exists
            if (module_result.to_execute)
                modules.push_back(module_result.to_execute);
            if (!module_result.trans_ids.empty())
            {
                for (auto id : module_result.trans_ids)
                {
                    auto res = waited_modules_.emplace(id, obj);
                    RX_ASSERT(res.second);// has to be unique
                }
            }
        }
    }
    return true;
}

bool upy_thread::transaction_ended (runtime_transaction_id_t id, mp_obj_t result, mp_obj_t exc)
{
    auto it = waited_modules_.find(id);
    if (it != waited_modules_.end())
    {
        bool ret_val = it->second->transaction_ended(id, result, exc);
        if (!ret_val)
        {
            this->include(it->second);
        }
        waited_modules_.erase(it);
        return true;
    }
    return false;
}

upy_thread& upy_thread::instance ()
{
    static upy_thread g_obj;
    return g_obj;
}

void upy_thread::append (job_ptr pjob)
{
}

void upy_thread::read (const string_type& path)
{
}


// Class rx_platform::python::upy_module 

upy_module::upy_module (const string_type& script, const string_type& eval_script)
      : script_(script),
        eval_script_(eval_script),
        py_module_(0)
{
}


upy_module::~upy_module()
{
}



upy_module_process_data upy_module::process_module ()
{
    rx_result result;
    if (!py_module_)
    {
        auto cp_result = compile_from_str("source", script_.c_str());
        if (cp_result)
        {
            py_module_ = cp_result.value();

             mp_obj_t exc = execute_function(py_module_);
            if (exc)
            {
                result = get_exception_string(exc);
                std::vector<upy_method_execution_context*> temp_array;
                {
                    locks::auto_lock_t _(&contexts_lock_);
                    temp_array = contexts_;
                    contexts_.clear();
                }
                for (auto ctx : temp_array)
                {
                    ctx->execution_complete(rx_result(result.errors()));
                }
            }
            //mp_map_t* map = &mp_obj_module_get_globals(py_module_)->map;
            //for (size_t i = 0; i < map->alloc; i++) {
            //    if (mp_map_slot_is_filled(map, i)) {
            //        // Entry in module global scope may be generated programmatically
            //        // (and thus be not a qstr for longer names). Avoid turning it in
            //        // qstr if it has '_' and was used exactly to save memory.
            //        const char* name = mp_obj_str_get_str(map->table[i].key);
            //        if (*name != '_') {
            //            qstr qname = mp_obj_str_get_qstr(map->table[i].key);
            //            mp_store_name(qname, map->table[i].value);
            //        }
            //    }
            //}

       //     mp_obj_t func = 0;
       //     mp_load_method(py_module_, qstr_from_str("py_process"), &func);
       //     auto gl = mp_obj_module_get_globals(py_module_);
       ////     auto fn = mp_obj_dict_get(gl, mp_obj_new_str_via_qstr("py_process", strlen("py_process")));
       //     mp_obj_t py_func =  mp_obj_dict_get(py_module_, mp_obj_new_str("py_process", strlen("py_process")));
        }
        else
        {
            result = cp_result.errors();
            std::vector<upy_method_execution_context*> temp_array;
            {
                locks::auto_lock_t _(&contexts_lock_);
                temp_array = contexts_;
                contexts_.clear();
            }
            for (auto ctx : temp_array)
            {
                ctx->execution_complete(rx_result(result.errors()));
            }
        }
    }
    upy_module_process_data ret_data(0);
   // return py_module_;
    if (py_module_)
    {
        std::vector<upy_method_execution_context*> temp_array;
        {
            locks::auto_lock_t _(&contexts_lock_);
            temp_array = contexts_;
            contexts_.clear();
        }
        for (auto ctx : temp_array)
        {
           auto id = ctx->get_id();
           pending_contexts_.emplace(id, ctx);
           eval_str("source1", ctx->get_eval_code().c_str());
           ret_data.trans_ids.push_back(id);
        }
    }
    return ret_data;
}

bool upy_module::transaction_ended (runtime_transaction_id_t id, mp_obj_t result, mp_obj_t exc)
{
    auto it = pending_contexts_.find(id);
    if (it != pending_contexts_.end())
    {
        it->second->execution_complete(true);
        pending_contexts_.erase(it);
    }
    return true;
}

void upy_module::push_context (upy_method_execution_context* ctx)
{
    locks::auto_lock_t _(&contexts_lock_);
    contexts_.push_back(ctx);
}


} // namespace python
} // namespace rx_platform

#endif
