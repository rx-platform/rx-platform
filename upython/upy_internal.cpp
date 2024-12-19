

/****************************************************************************
*
*  upython\upy_internal.cpp
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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
#include "upy_values.h"
#include "sys_internal/rx_async_functions.h"
#include "upython.h"

host_data g_host_data{};

#include "upy_method.h"
using rx_platform::python::upy_method_execution_context;


std::vector<mp_obj_t> g_modules;
std::vector<mp_obj_t> g_results;

int get_modules(uint32_t timeout, size_t* count, mp_obj_t** objs, size_t* ccaunt, mp_obj_t** cobjs)
{
    g_modules.clear();
    g_results.clear();
    bool ret = rx_platform::python::upy_thread::instance().get_modules(timeout, g_modules, g_results);
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
    if (!g_results.empty())
    {
        *cobjs = &g_results[0];
        *ccaunt = g_results.size()/3;
    }
    else
    {
        *ccaunt = 0;
    }
    return 1;
}
void module_done(uint32_t id, mp_obj_t result)
{
    if(mp_obj_is_exception_instance(result))
        bool ret = rx_platform::python::upy_thread::instance().transaction_ended(id, 0, result);
    else
        bool ret = rx_platform::python::upy_thread::instance().transaction_ended(id, result, 0);
}

mp_obj_t module_read(uint32_t id, const char* path, mp_obj_t iter)
{
    return rx_platform::python::upy_thread::instance().read(id, path, iter);
}
mp_obj_t module_write(uint32_t id, const char* path, mp_obj_t what, mp_obj_t iter)
{
    return rx_platform::python::upy_thread::instance().write(id, path, what, iter);
}
void log_write(const char* data, size_t size)
{
    bool found = false;
    string_type str_data;
    while (size > 0)
    {
        if (found || *data > ' ')
        {
            if (str_data.empty())
                str_data.reserve(size);
            found = true;
            str_data += *data;
        }
        size--;
        data++;
    }
    if (!str_data.empty())
    {
        string_type temp("upyhton:");
        temp += str_data;
        rx_platform::log::log_object::instance().log_event_fast(log::log_event_type::trace, "upython", "python", 100, "", nullptr, temp);
    }
}

extern "C"
{
    int c_get_modules(int timeout, size_t* count, mp_obj_t** objs, size_t* ccaunt, mp_obj_t** cobjs)
    {
        return get_modules((uint32_t)timeout, count, objs, ccaunt, cobjs);
    }

    void c_module_done(uint32_t id, mp_obj_t result)
    {
        return module_done(id, result);
    }
    mp_obj_t c_module_read(uint32_t id, const char* path, mp_obj_t iter)
    {
        return module_read(id, path, iter);
    }
    mp_obj_t c_module_write(uint32_t id, const char* path, mp_obj_t what, mp_obj_t iter)
    {
        return module_write(id, path, what, iter);
    }
    void c_log_write(const char* data, size_t size)
    {
        return log_write(data, size);
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
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0)
    {
        qstr src_name = 1/*MP_QSTR_*/;
        if (name)
            qstr src_name = qstr_from_str(name);
        mp_lexer_t* lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
        mp_obj_t module_fun = mp_compile(&pt, src_name, false);
        
        nlr_pop();
        return module_fun;
    }
    else
    {
        // uncaught exception
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        return get_exception_string(exc);
    }
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
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0)
    {
        qstr src_name = 1/*MP_QSTR_*/;
        if (name)
            qstr src_name = qstr_from_str(name);
        mp_lexer_t* lex = mp_lexer_new_from_str_len(src_name, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_EVAL_INPUT);
        mp_obj_t module_fun = mp_compile(&pt, src_name, false);
        mp_call_function_0(module_fun);
        nlr_pop();
        return 0;
    }
    else
    {
        // uncaught exception
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        mp_obj_print_exception(&mp_plat_print, exc);
        return (mp_obj_t)nlr.ret_val;
    }
}

mp_obj_t execute_from_str(const char* name, const char* str) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0)
    {
       /* mp_lexer_t* lex = mp_lexer_new_from_file("C:\\RX\\Native\\Storage\\upy_app\\cvejo.py");
	    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
	    mp_obj_t module_fun = mp_compile(&pt, lex->source_name, false);
	    mp_call_function_0(module_fun);*/
        mp_lexer_t* lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, str, strlen(str), false);
        mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
        qstr src_name = lex->source_name;
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
        else if (rx_val.get_type() == RX_BOOL_TYPE)
        {
            return rx_val.get_bool() ? "True" : "False";
        }
        else
        {
            return rx_val.to_string();
        }
    }
    return "None";
}

// Class rx_platform::python::upy_thread 

upy_thread::upy_thread()
      : has_job_(false),
        single_result_callback_(0)
    , threads::thread("upython", RX_DOMAIN_PYTHON)
{
}


upy_thread::~upy_thread()
{
}



uint32_t upy_thread::handler ()
{

    UPYTHON_LOG_INFO("upy_thread", 900, "Starting micropython thread.");

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
        single_result_callback_ = mp_load_global(qstr_from_str("internal_set_rx_platform_result"));

        for (const auto& one : file_modules_)
        {
            // skip main for now
            if (one.first != "main")
                continue;

            execute_from_str(nullptr, one.second.c_str());
        }
        nlr_pop();
    }
    else
    {
        // uncaught exception
        mp_obj_t exc = MP_OBJ_FROM_PTR(nlr.ret_val);
        string_type err = get_exception_string(exc);
        mp_obj_print_exception(&mp_plat_print, exc);
        return -1;// (mp_obj_t)nlr.ret_val;
    }

    UPYTHON_LOG_INFO("upy_thread", 900, "Exiting micropython thread.");


    return 0;
}

void upy_thread::include (upy_module_ptr what)
{
    bool was_empty = false;
    {
        locks::auto_lock_t dummy(&jobs_lock_);
        was_empty = modules_.empty() && results_.empty();
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
    g_host_data.write_log = c_log_write;
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

bool upy_thread::wait (modules_type& queued, results_type& results, uint32_t timeout)
{
    if (RX_WAIT_0 != has_job_.wait_handle(timeout))
        return false;

    locks::auto_lock_t dummy(&jobs_lock_);

    RX_ASSERT(!(modules_.empty() && results_.empty()));
    queued.assign(modules_.begin(), modules_.end());
    modules_.clear();
    results = std::move(results_);
    results_ = results_type();

    return true;
}

bool upy_thread::get_modules (uint32_t timeout, std::vector<mp_obj_t>& modules, std::vector<mp_obj_t>& results)
{

    modules_type queued;
    results_type queued_results;

    modules.clear();

    if (wait(queued, queued_results, timeout))
    {

        for (auto& obj : queued_results)
        {
            auto module_result = obj->process_module();
            if (module_result.func && module_result.iter)
            {
                results.push_back(module_result.func);
                results.push_back(module_result.iter);
                results.push_back(module_result.value);
            }
            else
            {
                RX_ASSERT(false);
            }
        }
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

void upy_thread::append_result (upy_callback_ptr data)
{
    bool was_empty = false;
    {
        locks::auto_lock_t dummy(&jobs_lock_);
        was_empty = modules_.empty() && results_.empty();
        results_.push_back(std::move(data));
    }

    if (was_empty)
        has_job_.set();
}

mp_obj_t upy_thread::read (runtime_transaction_id_t id, const string_type& path, mp_obj_t iter)
{
    auto it = waited_modules_.find(id);
    if (it != waited_modules_.end())
    {
        it->second->read(path, iter, single_result_callback_);
    }
    return 0;
}

mp_obj_t upy_thread::write (runtime_transaction_id_t id, const string_type& path, mp_obj_t val, mp_obj_t iter)
{
    auto it = waited_modules_.find(id);
    if (it != waited_modules_.end())
    {
        it->second->write(path, val, iter, single_result_callback_);
    }
    return 0;
}


// Class rx_platform::python::upy_module 

upy_module::upy_module (const string_type& script, const string_type& eval_script, platform_item_ptr item)
      : script_(script),
        eval_script_(eval_script),
        py_module_(0),
        item_(std::move(item))
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
                contexts_type temp_array;
                {
                    locks::auto_lock_t _(&contexts_lock_);
                    temp_array = std::move(contexts_);
                    contexts_ = contexts_type();
                }
                for (auto& ctx : temp_array)
                {
                    ctx->execution_complete(rx_result(result.errors()));
                }
            }
        }
        else
        {
            result = cp_result.errors();
            exception_text_ = result.errors_line();
            contexts_type temp_array;
            {
                locks::auto_lock_t _(&contexts_lock_);
                temp_array = std::move(contexts_);
                contexts_ = contexts_type();
            }
            for (auto& ctx : temp_array)
            {
                ctx->execution_complete(rx_result(result.errors()));
            }
        }
    }
    upy_module_process_data ret_data(0);
   // return py_module_;
    if (py_module_)
    {
        contexts_type temp_array;
        {
            locks::auto_lock_t _(&contexts_lock_);
            temp_array = std::move(contexts_);
            contexts_ = contexts_type();
        }
        for (auto& ctx : temp_array)
        {
           auto id = ctx->get_id();
           auto ret = pending_contexts_.emplace(id, std::move(ctx));
           eval_str("source1", ret.first->second->get_eval_code().c_str());
           ret_data.trans_ids.push_back(id);
        }
    }
    else
    {
        contexts_type temp_array;
        {
            locks::auto_lock_t _(&contexts_lock_);
            temp_array = std::move(contexts_);
            contexts_ = contexts_type();
        }
        for (auto& ctx : temp_array)
        {
            ctx->execution_complete(rx_result(exception_text_));
        }
    }
    return ret_data;
}

bool upy_module::transaction_ended (runtime_transaction_id_t id, mp_obj_t result, mp_obj_t exc)
{

    auto it = pending_contexts_.find(id);
    if (it != pending_contexts_.end())
    {
        if (exc)
        {
            string_type ex = get_exception_string(exc);
            it->second->execution_complete(rx_result(ex));

        }
        else if (result)
        {
            if (result == mp_const_none)
            {
                rx_simple_value ret_val;
                ret_val.assign_static(std::vector<rx_simple_value>());
                it->second->execution_complete(std::move(ret_val));
            }
            else if (mp_obj_is_type(result, &mp_type_tuple) || mp_obj_is_type(result, &mp_type_list))
            {
                size_t len;
                mp_obj_t* items;
                mp_obj_get_array(result, &len, &items);
                std::vector<rx_simple_value> vals;
                rx_result conv_result(true);
                for (int i = 0; i < len; i++)
                {
                    rx_simple_value temp_val;
                    conv_result = upy_convertor::upy_to_simple(items[i], temp_val);
                    if (conv_result)
                    {
                        vals.push_back(std::move(temp_val));
                    }
                    else
                    {
                        conv_result.register_error(rx_create_string("Error converting argument ", i + 1));
                        break;
                    }
                }
                if (conv_result)
                {
                    rx_simple_value ret_val;
                    ret_val.assign_static(std::move(vals));
                    it->second->execution_complete(std::move(ret_val));
                }
                else
                {
                    it->second->execution_complete(std::move(conv_result));
                }
            }
            else if (mp_obj_is_dict_or_ordereddict(result))
            {
                size_t len = mp_obj_dict_len(result);
                mp_map_t* map = mp_obj_dict_get_map(result);

                data::runtime_values_data data;
                rx_result conv_result(true);
                for (int i = 0; i < len; i++)
                {
                    string_type key_str;
                    string_type val_str;

                    mp_obj_t key = map->table[i].key;
                    mp_obj_t value = map->table[i].value;

                    if (mp_obj_is_qstr(key) || mp_obj_is_str(key))
                    {
                        key_str = mp_obj_str_get_str(key);
                    }
                    else
                    {
                        conv_result = "Dictionary is supported only with string keys!";
                        break;
                    }
                    rx_simple_value temp_val;
                    conv_result = upy_convertor::upy_to_simple(value, temp_val);
                    if (conv_result)
                    {
                        data.add_value(key_str, std::move(temp_val));
                    }
                    else
                    {
                        conv_result.register_error(rx_create_string("Error converting argument ", key_str));
                        break;
                    }

                }
                if (conv_result)
                {
                    it->second->execution_complete(std::move(data));
                }
                else
                {
                    it->second->execution_complete(std::move(conv_result));
                }
            }
            else
            {
                it->second->execution_complete(rx_result("Invalid return format"));
            }
        }
        else
        {
            it->second->execution_complete(rx_result("Empty return value"));
        }

        pending_contexts_.erase(it);
    }
    return true;
}

void upy_module::push_context (std::unique_ptr<upy_method_execution_context> ctx)
{
    locks::auto_lock_t _(&contexts_lock_);
    contexts_.push_back(std::move(ctx));
}

void upy_module::read (const string_type& path, mp_obj_t iter, mp_obj_t func)
{
    if (item_)
    {
        rx_post_function_to(item_->get_executer(), smart_this(), [this](string_type path, mp_obj_t iter, mp_obj_t func)
            {
                item_->read_value(path, read_result_callback_t(smart_this(), [this, iter, func](rx_result&& result, rx_value&& value)
                    {
                        auto callback = std::make_unique<upy_read_callback>();
                        callback->func = func;
                        callback->iter = iter;
                        callback->value = std::move(value);
                        callback->result = std::move(result);
                        upy_thread::instance().append_result(std::move(callback));

                    }));
            }, path, iter, func);
    }
}

void upy_module::write (const string_type& path, mp_obj_t val, mp_obj_t iter, mp_obj_t func)
{
    if (item_)
    {
        rx_simple_value temp;
        auto result = upy_convertor::upy_to_simple(val, temp);
        rx_post_function_to(item_->get_executer(), smart_this(), [this](string_type path, rx_simple_value value, mp_obj_t iter, mp_obj_t func)
            {
                item_->write_value(path, false, std::move(value), write_result_callback_t(smart_this(), [this, iter, func](uint32_t signal_level, rx_result&& result)
                    {
                        auto callback = std::make_unique<upy_write_callback>();
                        callback->func = func;
                        callback->iter = iter;
                        callback->result = std::move(result);
                        upy_thread::instance().append_result(std::move(callback));

                    }));
            }, path, std::move(temp), iter, func);
    }
}

void upy_module::deinit ()
{
}


// Class rx_platform::python::upy_callback_module 

upy_callback_module::upy_callback_module()
{
}


upy_callback_module::~upy_callback_module()
{
}



// Class rx_platform::python::upy_read_callback 


upy_callback_data upy_read_callback::process_module ()
{
    upy_callback_data ret;
    ret.func = func;
    ret.iter = iter;
    ret.value = mp_const_none;
    if (result && value.is_good())
    {
        auto result = upy_convertor::value_to_upy(value, &ret.value);
        if (!result)
        {
            ret.value = mp_const_none;
        }
    }
    return ret;
}


// Class rx_platform::python::upy_write_callback 


upy_callback_data upy_write_callback::process_module ()
{
    upy_callback_data ret;
    ret.func = func;
    ret.iter = iter;
    if (result)
        ret.value = mp_obj_new_bool(1);
    else
        ret.value = mp_obj_new_bool(0);
    return ret;
}


} // namespace python
} // namespace rx_platform

#endif
