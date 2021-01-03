

/****************************************************************************
*
*  lib\rx_func_to_go.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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




template <typename anchorT, typename... Args>
struct function_to_go 
{

    typedef std::true_type has_arguments;

    using data_tuple_type = std::tuple<std::decay_t<Args>...>;

    template<typename funcT>
    function_to_go(anchorT anc, funcT&& f)
        : anchor(anc)
        , function(std::forward<funcT>(f))
    {
    }

    function_to_go() = default;
    function_to_go(const function_to_go&) = delete;
    function_to_go(function_to_go&& right) = default;
    function_to_go& operator=(function_to_go&& right) = default;

    void set_arguments(Args&&... args)
    {
        data = std::move(data_tuple_type(std::decay_t<Args>(std::forward<Args>(args))...));
    }



      void call ()
      {
          std::apply(function, loose_call<std::tuple<Args&&...> >(data));
          //std::apply(f_, make_arguments_tuple<std::tuple<Args&&...>, std::tuple<std::decay_t<Args>...> >(data_));
      }


      anchorT anchor;

      std::function<void(Args...)> function;

      data_tuple_type data;

  public:

  protected:

  private:


};


template <typename refT>
struct function_to_go<refT>
{
    typedef std::false_type has_arguments;

    template<typename funcT>
    function_to_go(refT ref, funcT&& f)
        : reference(ref)
        , function(std::forward<funcT>(f))
    {
    }

    function_to_go() = default;
    function_to_go(const function_to_go&) = delete;
    function_to_go(function_to_go&&) noexcept = default;

    void call()
    {
        function();
    }

    void set_arguments()
    {
        // nothing to do!!!
    }

    std::function<void(void)> function;

    refT reference;

};

template <typename... Args>
using rx_function_to_go = function_to_go<rx_reference_ptr, Args...>;


template <typename resultT>
using rx_result_with_callback = function_to_go<rx_reference_ptr, rx_result_with<resultT>&& >;

using rx_result_callback = function_to_go<rx_reference_ptr, rx_result&& >;


} // namespace rx



#endif
