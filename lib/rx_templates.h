

/****************************************************************************
*
*  lib\rx_templates.h
*
*  Copyright (c) 2020 ENSACO Solutions doo
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
