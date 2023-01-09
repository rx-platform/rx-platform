

/****************************************************************************
*
*  lib\member_check.h
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


#ifndef member_check_h
#define member_check_h 1




//////////////////////////////////////////////////////////////////////////////
// This code here is important for the code
// I'we found it on web here, don't know who the guy is
// <https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature>
//////////////////////////////////////////////////////////////////////////////

namespace member_check
{

/*
    - Multiple inheritance forces ambiguity of member names.
    - SFINAE is used to make aliases to member names.
    - Expression SFINAE is used in just one generic has_member that can accept
      any alias we pass it.
*/

//Variadic to force ambiguity of class members.  C++11 and up.
template <typename... Args> struct ambiguate : public Args... {};

//Non-variadic version of the line above.
//template <typename A, typename B> struct ambiguate : public A, public B {};

template<typename A, typename = void>
struct got_type : std::false_type {};

template<typename A>
struct got_type<A> : std::true_type {
    typedef A type;
};

template<typename T, T>
struct sig_check : std::true_type {};

template<typename Alias, typename AmbiguitySeed>
struct has_member {
    template<typename C> static char(&f(decltype(&C::value)))[1];
    template<typename C> static char(&f(...))[2];

    //Make sure the member name is consistently spelled the same.
    static_assert(
        (sizeof(f<AmbiguitySeed>(0)) == 1)
        , "Member name specified in AmbiguitySeed is different from member name specified in Alias, or wrong Alias/AmbiguitySeed has been specified."
        );

    static bool const value = sizeof(f<Alias>(0)) == 2;
};

//Check for any member with given name, whether var, func, class, union, enum.
#define CREATE_MEMBER_CHECK(member)                                         \
                                                                            \
template<typename T, typename = std::true_type>                             \
struct Alias_##member;                                                      \
                                                                            \
template<typename T>                                                        \
struct Alias_##member <                                                     \
    T, std::integral_constant<bool, got_type<decltype(&T::member)>::value>  \
> { static const decltype(&T::member) value; };                             \
                                                                            \
struct AmbiguitySeed_##member { char member; };                             \
                                                                            \
template<typename T>                                                        \
struct has_member_##member {                                                \
    static const bool value                                                 \
        = has_member<                                                       \
            Alias_##member<ambiguate<T, AmbiguitySeed_##member>>            \
            , Alias_##member<AmbiguitySeed_##member>                        \
        >::value                                                            \
    ;                                                                       \
}


//Check for member variable with given name.
#define CREATE_MEMBER_VAR_CHECK(var_name)                                   \
                                                                            \
template<typename T, typename = std::true_type>                             \
struct has_member_var_##var_name : std::false_type {};                      \
                                                                            \
template<typename T>                                                        \
struct has_member_var_##var_name<                                           \
    T                                                                       \
    , std::integral_constant<                                               \
        bool                                                                \
        , !std::is_member_function_pointer<decltype(&T::var_name)>::value   \
    >                                                                       \
> : std::true_type {}

//Check for member function with given name AND signature.
#define CREATE_MEMBER_FUNC_SIG_CHECK(func_name, func_sig, templ_postfix)    \
                                                                            \
template<typename T, typename = std::true_type>                             \
struct has_member_func_##templ_postfix : std::false_type {};                \
                                                                            \
template<typename T>                                                        \
struct has_member_func_##templ_postfix<                                     \
    T, std::integral_constant<                                              \
        bool                                                                \
        , sig_check<func_sig, &T::func_name>::value                         \
    >                                                                       \
> : std::true_type {}


//Check for member class with given name.
#define CREATE_MEMBER_CLASS_CHECK(class_name)               \
                                                            \
template<typename T, typename = std::true_type>             \
struct has_member_class_##class_name : std::false_type {};  \
                                                            \
template<typename T>                                        \
struct has_member_class_##class_name<                       \
    T                                                       \
    , std::integral_constant<                               \
        bool                                                \
        , std::is_class<                                    \
            typename got_type<typename T::class_name>::type \
        >::value                                            \
    >                                                       \
> : std::true_type {}

//Check for member union with given name.
#define CREATE_MEMBER_UNION_CHECK(union_name)               \
                                                            \
template<typename T, typename = std::true_type>             \
struct has_member_union_##union_name : std::false_type {};  \
                                                            \
template<typename T>                                        \
struct has_member_union_##union_name<                       \
    T                                                       \
    , std::integral_constant<                               \
        bool                                                \
        , std::is_union<                                    \
            typename got_type<typename T::union_name>::type \
        >::value                                            \
    >                                                       \
> : std::true_type {}

//Check for member enum with given name.
#define CREATE_MEMBER_ENUM_CHECK(enum_name)                 \
                                                            \
template<typename T, typename = std::true_type>             \
struct has_member_enum_##enum_name : std::false_type {};    \
                                                            \
template<typename T>                                        \
struct has_member_enum_##enum_name<                         \
    T                                                       \
    , std::integral_constant<                               \
        bool                                                \
        , std::is_enum<                                     \
            typename got_type<typename T::enum_name>::type  \
        >::value                                            \
    >                                                       \
> : std::true_type {}

//Check for function with given name, any signature.
#define CREATE_MEMBER_FUNC_CHECK(func)          \
template<typename T>                            \
struct has_member_func_##func {                 \
    static const bool value                     \
        = has_member_##func<T>::value           \
        && !has_member_var_##func<T>::value     \
        && !has_member_class_##func<T>::value   \
        && !has_member_union_##func<T>::value   \
        && !has_member_enum_##func<T>::value    \
    ;                                           \
}

//Create all the checks for one member.  Does NOT include func sig checks.
#define CREATE_MEMBER_CHECKS(member)    \
CREATE_MEMBER_CHECK(member);            \
CREATE_MEMBER_VAR_CHECK(member);        \
CREATE_MEMBER_CLASS_CHECK(member);      \
CREATE_MEMBER_UNION_CHECK(member);      \
CREATE_MEMBER_ENUM_CHECK(member);       \
CREATE_MEMBER_FUNC_CHECK(member)



}// namespace member_check


#endif
