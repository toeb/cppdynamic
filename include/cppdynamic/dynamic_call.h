#pragma once
#include <functional>

#include "config.h"
#include "sequence.h"
#include "tuple_utilities.h"
#include "func_traits.h"
#include "any.h"
#include "Void.h"

NS_BEGIN(CORE_NAMESPACE)

  /// call any type of function passed as first argument with all passed arguments (container of any)
  /// if you call a member function the first arguments needs to be an object, object pointer, std::shared_ptr or a std::weak_ptr
  /// an overload for using initializer lists exists.
  /// e.g. 
  // any result = dynamic_call([](int i, int j){return i+j;}, {1,2});  
  // any result = dynamic_call(&staticmethod, {1,2});
  // any result = dynamic_call(&A::membermethod, {1,2});
  template < typename TCallable, typename ArgListContainer >
  any dynamic_call(TCallable callable, const ArgListContainer & args);


  // helper class for calling any type of callable
  template<typename T>
  struct dynamic_call_helper {
  public:
    typedef std::vector<any> arg_list;
  private:
    // helper method for getting a pointer to the specified type
    template<typename TType>
    static bool any_aquire_ptr(any ptr, std::function<void(TType*)> f){
      type_provider::type_id type;

      type = type_provider::typeOf<TType>();
      auto type2 = ptr.type();
      if (type_provider::areEqual(type,type2)){
        auto val = any_cast<TType>(ptr);
        f(&val);
        return true;
      }
      type = type_provider::typeOf<TType*>();
      if (type_provider::areEqual(type, type2)){
        auto cptr = any_cast<TType*>(ptr);
        f(cptr);
        return true;
      }
      type = type_provider::typeOf<std::shared_ptr<TType>>();
      if (type_provider::areEqual(type, type2)){
        auto sptr = any_cast<std::shared_ptr<TType>>(ptr);
        if (sptr){
          f(sptr.get());
          return true;
        }
      }

      type = type_provider::typeOf<std::weak_ptr<TType>>();
      if (type_provider::areEqual(type, type2)){
        auto wptr = any_cast<std::weak_ptr<TType>>(ptr);
        auto sptr = wptr.lock();
        if (sptr){
          f(sptr.get());
          return true;
        }
      }
      return false;
    }



    typedef func_traits<T> traits;
    typedef typename generate_sequence<traits::arity>::type sequence_type;
    // dispatch non void function
    template<int ...S, typename TX = any>
    static typename std::enable_if<!traits::returns_void &&traits::type == method, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>){
      typename traits::signature_tuple args;
      tuple_set(args, dargs);
      return (*func)(std::get<S>(args)...);
    }
    // dispatch void function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<traits::returns_void && traits::type == method, TX>::type
      dispatch(typename traits::function_type func, const arg_list & dargs, sequence<S...>, void * = 0){
      typename traits::signature_tuple args;
      tuple_set(args, dargs);
      (*func)(std::get<S>(args)...);
      
      return any(Void());
    }

    // dispatch non-void const member function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<!traits::returns_void &&traits::type == member_method&&traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0){
      typename traits::args_tuple args;
      std::vector<any> args_copy = dargs;
      args_copy.erase(std::begin(args_copy));
      tuple_set(args, args_copy);
      any result;
      any_aquire_ptr<typename traits::class_type>(dargs[0],
        [&](typename traits::class_type * ptr){
        result = (*ptr.*func)(std::get<S>(args)...);
      })
        ||
        any_aquire_ptr<typename std::add_const<typename traits::class_type  >::type>(dargs[0],
        [&](typename std::add_const<typename traits::class_type>::type * ptr){
        result = (*ptr.*func)(std::get<S>(args)...);
      });
      return result;
    }

    // dispatch non-void non-const member function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<!traits::returns_void &&traits::type == member_method&&!traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0){
      typename traits::args_tuple args;
      std::vector<any> args_copy = dargs;
      args_copy.erase(std::begin(args_copy));
      tuple_set(args, args_copy);
      any result;
      any_aquire_ptr<typename traits::class_type>(dargs[0],
        [&](typename traits::class_type * ptr){
        result = (*ptr.*func)(std::get<S>(args)...);
      });
      return result;
    }

    // dispatch void const member function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<traits::returns_void &&traits::type == member_method&&traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0, void * = 0){
      typename traits::args_tuple args;
      std::vector<any> args_copy = dargs;
      args_copy.erase(std::begin(args_copy));
      tuple_set(args, args_copy);
      any_aquire_ptr<typename traits::class_type>(dargs[0],
        [&](typename traits::class_type * ptr){
        (*ptr.*func)(std::get<S>(args)...);
      })
        ||
        any_aquire_ptr<typename std::add_const<typename traits::class_type  >::type>(dargs[0],
        [&](typename std::add_const<typename traits::class_type>::type * ptr){
        (*ptr.*func)(std::get<S>(args)...);
      });
      return Void();
    }
    // dispatch void non-const member function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<traits::returns_void &&traits::type == member_method&&!traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0){
      typename traits::args_tuple args;
      std::vector<any> args_copy = dargs;
      args_copy.erase(std::begin(args_copy));
      tuple_set(args, args_copy);
      any_aquire_ptr<typename traits::class_type>(dargs[0],
        [&](typename traits::class_type * ptr){
        (*ptr.*func)(std::get<S>(args)...);
      });
      return Void();
    }

    // dispatch non-void const functor function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<!traits::returns_void &&traits::type == functor&&traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void* = 0){
      typename traits::args_tuple args;
      tuple_set(args, dargs);
      return func(std::get<S>(args)...);
    }

    // dispatch non-void non-const functor function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<!traits::returns_void && traits::type == functor &&!traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0){
      typename traits::args_tuple args;
      tuple_set(args, dargs);
      return func(std::get<S>(args)...);
    }

    // dispatch void const functor function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<traits::returns_void &&traits::type == functor&&traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0){
      typename traits::args_tuple args;
      tuple_set(args, dargs);
      func(std::get<S>(args)...);
      return Void();
    }


    // dispatch void non-const functor function
    template<int ...S, typename  TX = any>
    static typename std::enable_if<traits::returns_void &&traits::type == functor&&!traits::is_const_call, TX>::type
      dispatch(typename traits::function_type & func, const arg_list & dargs, sequence<S...>, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0, void * = 0){
      typename traits::args_tuple args;
      tuple_set(args, dargs);
      func(std::get<S>(args)...);
      return Void();
    }

    
  public:
    /// call implementation calls the correct dispatcher for the specified function type
    static any call(typename traits::function_type & func, arg_list & dargs){
      return dispatch(func, dargs, sequence_type());
    };
  };


  /// call any type of function passed as first argument with all passed arguments (container of any)
  /// if you call a member function the first arguments needs to be an object, object pointer, std::shared_ptr or a std::weak_ptr
  template < typename TCallable, typename ArgListContainer >
  any dynamic_call(TCallable callable, const ArgListContainer & args){
    //static_assert(std::is_same<typename ArgListContainer::value_type, any>::value, "expected a std::container with element type any");
    auto argv = std::vector<any>(std::begin(args), std::end(args));
    return dynamic_call_helper<TCallable>::call(callable, argv);
  }


  // specialization for initializer lists 
// syntax is e.g. dynamic_call([](int i, int j){return i+j;}, {1,2});  
  template<typename TCallable>
  any dynamic_call(TCallable callable, const std::vector<any> & args){
    return dynamic_call<TCallable, std::vector<any>>(callable, args);
  }
NS_END(CORE_NAMESPACE)

