#pragma once
#include "config.h"
#include "func_traits.h"
NS_BEGIN(CORE_NAMESPACE)

  namespace detail{
    template<class T>
    struct sfinae_true : public std::true_type{
      typedef T type;
      
    };

    template<class T>
    struct sfinae_false : public std::false_type{
      typedef T type;
    };

    template<class T>
    static auto test_call_op(int)
      ->sfinae_true < decltype(&T::operator()) >;
    template<class T>
    static auto test_call_op(long)->sfinae_false < T >;

    template<class T>
    static auto test_to_any_op(int)
      ->sfinae_true < decltype(&T::to_any) >;
    template<class T>
    static auto test_to_any_op(long)->sfinae_false < T >;

    template<class T, class T2 =decltype( detail::  test_call_op<T>(0) )>
    struct has_call_op_ : public T2  {
    
    };

    template<class T, class T2 = decltype(detail::test_to_any_op<T>(0))>
    struct has_to_any_op_ : public T2{
      
    };

  } // detail::

  template<class T>
  struct has_call_op : public std::is_member_function_pointer < typename detail::has_call_op_<T>::type > {

  };
  template<class T>
  struct has_to_any_op :
    public std::integral_constant<bool, 
    std::is_member_function_pointer < typename detail::has_to_any_op_<T>::type >::value
    && detail::has_to_any_op_<T>::value
    > {

  };



  template<typename T>
  struct is_callable :public has_call_op<T>
  {
  };
  template<typename TClass, typename TRet, typename... TArgs>
  struct is_callable<TRet(TClass::*)(TArgs...)>{
    const static bool value = true;
  };

  template<typename TClass, typename TRet, typename... TArgs>
  struct is_callable<TRet(TClass::*)(TArgs...)const>{
    const static bool value = true;
  };

  template<typename TRet, typename... TArgs>
  struct is_callable<TRet(*)(TArgs...)>{
    const static bool value = true;
  };



NS_END(CORE_NAMESPACE)
