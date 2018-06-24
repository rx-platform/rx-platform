#pragma once

/*
type lists definitions, similar like loki stuff
*/



#define TYPELIST_1(T1) tl::type_list<T1, tl::null_type>
#define TYPELIST_2(T1, T2) tl::type_list<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) tl::type_list<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) \
tl::type_list<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) \
tl::type_list<T1, TYPELIST_4(T2, T3, T4, T5) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
tl::type_list<T1, TYPELIST_5(T2, T3, T4, T5, T6) >
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
tl::type_list<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
tl::type_list<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >
#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
tl::type_list<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
tl::type_list<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >
#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
tl::type_list<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >
#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
tl::type_list<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) >



namespace tl
{

template <typename T>
struct type2type
{
	typedef T original_type;
};

class null_type {};


struct empty_type {};

template <class T, class U>
struct type_list
{
	typedef T head;
	typedef U tail;
};

template <class tlist> struct length;
template <> struct length<null_type>
{
enum { value = 0 };
};
template <class T, class U>
struct length< type_list<T, U> >
{
enum { value = 1 + length<U>::value };
};

template <class TList, class T> struct append;
template <> struct append<null_type, null_type>
{
	typedef null_type result;
};
template <class T> struct append<null_type, T>
{
	typedef TYPELIST_1(T) result;
};
template <class Head, class Tail>
struct append<null_type, type_list<Head, Tail> >
{
	typedef type_list<Head, Tail> result;
};
template <class Head, class Tail, class T>
struct append<type_list<Head, Tail>, T>
{
	typedef type_list<Head,
		typename append<Tail, T>::Result>
		result;
};

template <class tlist, template <class> class unit>
class gen_scatter_hierarchy;
// gen_scatter_hierarchy specialization: Typelist to Unit
template <class T1, class T2, template <class> class unit>
class gen_scatter_hierarchy<type_list<T1, T2>, unit>
	: public gen_scatter_hierarchy<T1, unit>
	, public gen_scatter_hierarchy<T2, unit>
{
public:
	typedef type_list<T1, T2> tlist;
	typedef gen_scatter_hierarchy<T1, unit> left_base;
	typedef gen_scatter_hierarchy<T2, unit> right_base;
};
// Pass an atomic type (non-typelist) to Unit
template <class atomic_type, template <class> class unit>
class gen_scatter_hierarchy : public unit<atomic_type>
{
	typedef unit<atomic_type> left_base;
};
// Do nothing for NullType
template <template <class> class unit>
class gen_scatter_hierarchy<null_type, unit>
{
};

template<class TList,template <class AtomicType, class Base> class unit,class root = empty_type>
class gen_linear_hierarchy;
template<class T1,class T2,template <class, class> class unit,class root>
class gen_linear_hierarchy<type_list<T1, T2>, unit, root>
	: public unit< T1, gen_linear_hierarchy<T2, unit, root> >
{
};
template<class T,template <class, class> class unit,class root>
class gen_linear_hierarchy<TYPELIST_1(T), unit, root>
	: public unit<T, root>
{
};

template<typename T>
class type_traits
{
private:
	template <class U> struct pointer_traits
	{
		enum { result = false };
		typedef null_type pointee_type;
	};
	template <class U> struct pointer_traits<U*>
	{
		enum { result = true };
		typedef U pointee_type;
	};
	template <class U> struct un_const
	{
		typedef U Result;
	};
	template <class U> struct un_const<const U>
	{
		typedef U Result;
	};
public:
	enum { is_pointer = pointer_traits<T>::result };
	typedef typename pointer_traits<T>::pointee_type pointee_type;
	typedef typename un_const<T>::Result non_const_type;
};


template <typename T>
struct boxing_t
{
	T value;
};


}
