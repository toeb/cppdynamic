#pragma once
#include <tuple>

#include "config.h"
NS_BEGIN(CORE_NAMESPACE)
  // the different types of callables that exist
  enum callable_type{
    member_method = 1,
    method = 2,
    functor = 3,
    not_callable = 4
  };
  struct Global{};

  template<typename T>
  struct _func_traits
    //: public func_traits<decltype(&T::operator())> does not work in MSVC2013
  {
    static const callable_type type = functor;
    static const bool returns_void = true;
    static const bool is_const_call = true;
    static const bool is_volatile_call = false;
    typedef void function_type;
    typedef void class_type;
    typedef void defining_class_type;
    typedef void return_type;
    typedef decltype(std::make_tuple<>()) args_tuple;
    typedef decltype(std::make_tuple<>()) signature_tuple;
    static const int arity = 0;
  };


  template < typename TClass, typename TRet, typename ... TArgs>
  struct _func_traits < TRet(TClass::*)(TArgs...) >
  {
    typedef TRet(TClass::*function_type)(TArgs...);
    static const callable_type type = member_method;
    static const bool is_const_call = false;
    static const bool is_volatile_call = false;
    typedef TClass defining_class_type;
    typedef TClass class_type;
    typedef TRet return_type;
    typedef std::tuple<TArgs...> args_tuple;
    typedef std::tuple<TClass*, TArgs...> signature_tuple;
    static const bool returns_void = std::is_void< return_type>::value;
    static const int arity = sizeof...(TArgs);


  };
  template < typename TClass, typename TRet, typename ... TArgs>
  struct _func_traits < TRet(TClass::*)(TArgs...)const >
  {
    typedef TRet(TClass::*function_type)(TArgs...)const;
    static const callable_type type = member_method;
    static const bool is_const_call = true;
    static const bool is_volatile_call = false;
    typedef TClass defining_class_type;
    typedef TClass class_type;
    typedef TRet return_type;
    typedef std::tuple<TArgs...> args_tuple;
    typedef std::tuple<TClass*, TArgs...> signature_tuple;
    static const int arity = sizeof...(TArgs);
    static const bool returns_void = std::is_void< return_type>::value;

  };

  template <  typename TRet, typename ... TArgs>
  struct _func_traits < TRet(*)(TArgs...) >
  {
    typedef  TRet(*function_type)(TArgs...);
    static const callable_type type = method;
    static const bool is_const_call = false;
    static const bool is_volatile_call = false;
    typedef Global class_type;
    typedef TRet return_type;
    typedef std::tuple<TArgs...> args_tuple;
    typedef std::tuple<TArgs...> signature_tuple;
    static const int arity = sizeof...(TArgs);
    static const bool returns_void = std::is_void< return_type>::value;

  };

  /// todo member methods defined in parent types should be correctly have a difference between class_type and defining_class_type
  template<typename T>
  struct func_traits : public _func_traits < decltype(&T::operator()) > {
    static const callable_type type = functor;
    typedef T function_type;
    typedef T class_type;
  };
  template < typename TClass, typename TRet, typename ... TArgs>
  struct func_traits < TRet(TClass::*)(TArgs...)const > : public _func_traits < TRet(TClass::*)(TArgs...)const > {  };
  template < typename TClass, typename TRet, typename ... TArgs>
  struct func_traits < TRet(TClass::*)(TArgs...) > : public _func_traits < TRet(TClass::*)(TArgs...) > {  };
  template<typename TRet, typename... TArgs>
  struct func_traits<TRet(*)(TArgs...)> : public _func_traits < TRet(*)(TArgs...) > {  };
NS_END(CORE_NAMESPACE)
