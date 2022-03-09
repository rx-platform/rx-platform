

/****************************************************************************
*
*  lib\rx_func_to_go.h
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


#ifndef rx_func_to_go_h
#define rx_func_to_go_h 1


#include "rx_templates.h"
#include "rx_ptr.h"




namespace rx {


template<typename... Args>
struct tuple_wrapper
{
    tuple_wrapper(Args... args)
        : data(decay_copy(std::forward<Args>(args))...)
    {
    }
    tuple_wrapper() = default;
    template<typename rightT>
    tuple_wrapper& operator=(rightT&& right)
    {
        data= std::forward<rightT>(right);
        return *this;
    }
    std::tuple<std::decay_t<Args>...> data;
};




template <typename funcT, typename... Args>
struct function_to_go 
{

    typedef std::true_type has_arguments;

    using data_tuple_type = std::tuple<std::decay_t<Args>...>;

    function_to_go(rx_reference_ptr anc, funcT f)
        : anchor(anc)
        , function(std::move(f))
    {
    }

    function_to_go() = default;
    function_to_go(const function_to_go&) = delete;
    function_to_go(function_to_go&& right) noexcept = default;
    function_to_go& operator=(function_to_go&& right) noexcept = default;

    void set_arguments(Args&&... args)
    {
        data = std::move(data_tuple_type(std::decay_t<Args>(std::forward<Args>(args))...));
    }



      void call ()
      {
          std::apply(function, loose_call<std::tuple<Args&&...> >(data));
          //std::apply(f_, make_arguments_tuple<std::tuple<Args&&...>, std::tuple<std::decay_t<Args>...> >(data_));
      }


      rx_reference_ptr anchor;

      funcT function;

      data_tuple_type data;

  public:

  protected:

  private:


};


template <typename funcT>
struct function_to_go<funcT>
{
    typedef std::false_type has_arguments;

    function_to_go(rx_reference_ptr anc, funcT f)
        : anchor(anc)
        , function(std::move(f))
    {
    }

    function_to_go() = default;
    function_to_go(const function_to_go&) = delete;
    function_to_go(function_to_go&&) noexcept = default;
    function_to_go& operator=(function_to_go && right) noexcept = default;

    void call()
    {
        function();
    }

    void set_arguments()
    {
        // nothing to do!!!
    }

    rx_reference_ptr anchor;

    funcT function;
};








template <typename funcT, typename resultT, typename... Args>
class result_function_to_go 
{
public:

    typedef std::true_type has_arguments;

    using data_tuple_type = std::tuple<std::decay_t<Args>...>;

    result_function_to_go(rx_reference_ptr anc, funcT f)
        : anchor(anc)
        , function(std::move(f))
    {
    }

    result_function_to_go() = default;
    result_function_to_go(const result_function_to_go&) = delete;
    result_function_to_go(result_function_to_go && right) noexcept = default;
    result_function_to_go& operator=(result_function_to_go && right) noexcept = default;

    void set_arguments(Args&&... args)
    {
        data = std::move(data_tuple_type(std::decay_t<Args>(std::forward<Args>(args))...));
    }

  public:

      resultT call ()
      {
          return std::apply(function, loose_call<std::tuple<Args&&...> >(data));
      }


      rx_reference_ptr anchor;

      funcT function;

      data_tuple_type data;


  protected:

  private:


};


template <typename funcT, typename resultT>
struct result_function_to_go<funcT, resultT>
{
public:
    typedef std::false_type has_arguments;

    result_function_to_go(rx_reference_ptr anc, funcT f)
        : anchor(anc)
        , function(std::move(f))
    {
    }

    result_function_to_go() = default;
    result_function_to_go(const result_function_to_go&) = delete;
    result_function_to_go(result_function_to_go&&) noexcept = default;
    result_function_to_go& operator=(result_function_to_go&& right) noexcept = default;

    resultT call()
    {
        return function();
    }

    void set_arguments()
    {
        // nothing to do!!!
    }

    funcT function;

    rx_reference_ptr anchor;

};


} // namespace rx



#endif
