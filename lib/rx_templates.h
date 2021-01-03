

/****************************************************************************
*
*  lib\rx_templates.h
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


#ifndef rx_templates_h
#define rx_templates_h 1






namespace rx {

template<class T>
std::decay_t<T> decay_copy(T&& v)
{
    return std::forward<T>(v);
}


template<typename T>
struct arg_type
{
    private:
        typedef typename std::remove_reference<T>::type U;
    public:
         typedef typename std::remove_cv<U>::type type;
};

template<typename T>
typename arg_type<T>::type forward_as_argument(T&& t)
{
    using ret_type = typename arg_type<T>::type;
    if constexpr (std::is_lvalue_reference<T>::value)
    {
        if constexpr (std::is_copy_constructible<ret_type>::value)
        {
            ret_type ret(t);
            return ret;
        }
        else
        {
            using ret_type = typename arg_type<T>::type;
            ret_type ret(std::move(t));
            return ret;
        }
    }
    else if constexpr (std::is_rvalue_reference<T>::value)
    {
        return std::move(t);
    }
    else
    {
        if constexpr (!std::is_copy_constructible<ret_type>::value)
        {
            return std::forward<T>(t);
        }
        else
        {
            return std::move(t);
        }
    }
}


//
//template<typename T>
//typename arg_type<T>::type forward_as_argument(T t)
//{
//    return std::move(t);
//}

//template<typename T>
//typename arg_type<T>::type forward_as_argument(T t)
//{
//    using ret_type = typename arg_type<T>::type;
//
//    if constexpr (std::is_copy_constructible<ret_type>::value)
//    {
//        ret_type ret(t);
//        return ret;
//    }
//    else
//    {
//        using ret_type = typename arg_type<T>::type;
//        ret_type ret(std::move(t));
//        return ret;
//    }
//}


template<typename argT, typename storedT>
argT get_loose_call_value(storedT& v)
{
    if constexpr (std::is_lvalue_reference<argT>::value)
    {
        return v;
    }
    else if constexpr (std::is_rvalue_reference<argT>::value)
    {
        return std::move(v);
    }
    else
    {
        return v;
    }
}

template<class tupleArgs, class tupleSrc, size_t... Is>
constexpr auto loose_call(tupleSrc& t, std::index_sequence<Is...>)
{
    tupleArgs ret(get_loose_call_value<typename std::tuple_element<Is, tupleArgs>::type, typename std::tuple_element<Is, tupleSrc>::type>
        (std::get<Is>(t))...);
    return ret;
}
template<class tupleArgs, class tupleSrc>
constexpr auto loose_call(tupleSrc& t)
{
    return loose_call<tupleArgs, tupleSrc>(t, std::make_index_sequence<std::tuple_size<tupleSrc>::value >{});
}


template<class tupleT, size_t idx>
void rx_move_tuple_element(tupleT& dest, tupleT& src)
{
    std::get<idx>(dest) = std::move(std::get<idx>(src));
}
/*
template<class tupleT, size_t... Is>
void rx_move_tuple(tupleT& dest, tupleT& src, std::index_sequence<Is...>)
{
    rx_move_tuple_element<tupleT, Is>(dest, src);...
}
template<class tupleT>
void rx_move_tuple(tupleT& dest, tupleT& src)
{
    rx_move_tuple<tupleT>(dest, src, std::make_index_sequence<std::tuple_size<tupleT>::value >{});
}

*/

// This code below is from a gay's post on stack overflow
// the gay:
// <https://stackoverflow.com/users/596781/kerrek-sb>
// the code ::
// <https://stackoverflow.com/questions/10766112/c11-i-can-go-from-multiple-args-to-tuple-but-can-i-go-from-tuple-to-multiple>
// tuple to args...
namespace tuple_args_detail
{
template <typename F, typename Tuple, bool Done, int Total, int... N>
struct call_impl
{
    static void call(F f, Tuple&& t)
    {
        call_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
        //call_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
    }
};

template <typename F, typename Tuple, int Total, int... N>
struct call_impl<F, Tuple, true, Total, N...>
{
    static void call(F f, Tuple&& t)
    {
        f(std::forward<std::tuple_element<N, Tuple>::type>(std::get<N>(t))...);
        //f(std::get<N>(std::forward<Tuple>(t))...);
    }
};
}

// user invokes this
template <typename F, typename Tuple>
void call_with(F f, Tuple&& t)
{
    typedef typename std::decay<Tuple>::type ttype;
    tuple_args_detail::call_impl<F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(f, std::forward<Tuple>(t));
}






template <typename T, typename conceptT>
class type_less_box 
{

  public:
      type_less_box (const T& whose);


  protected:

  private:


};






class content_concept 
{

  public:

  protected:

  private:


};






struct type_less_concept 
{

      virtual ~type_less_concept() = 0;

  public:

  protected:

  private:


};


// Parameterized Class rx::type_less_box 

template <typename T, typename conceptT>
type_less_box<T,conceptT>::type_less_box (const T& whose)
{
}



// Class rx::content_concept 


} // namespace rx



#endif
