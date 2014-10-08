#pragma once
#include <memory>

// 
#define NS(...) dynamic
#define NS_BEGIN(...) namespace dynamic{
#define NS_END(...) }
#define NS_USE(...) using namespace dynamic;
#define CORE_NAMESPACE
#define CORE_DYNAMIC_NAMESPACE



// allow use of strncpy without warnings
#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#ifdef WIN32
#pragma warning(disable : 4250)
#define noexcept
#endif

#ifdef __GNUC__
// shims for gcc less than 4.7
#if __GNUC__ < 5 && __GNUC_MINOR__ < 7

#define override
#define final


namespace std{

  template<typename T, typename ...TArgs> std::unique_ptr<T> make_unique(TArgs&&... args){
    return std::unique_ptr<T>(new T(args...));
  }

  template<typename T> struct is_default_constructible{ static const bool value = false; };
  template<typename T, typename TTo> struct is_assignable{ static const bool value = false; };
}

#endif
#endif

