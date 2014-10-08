#pragma once

#include <tuple>
#include <vector>
#include <algorithm>

#include "config.h"
#include "any.h"

NS_BEGIN(CORE_NAMESPACE)

  template <int ind, typename... T>
  class tuple_helper {
  public:
    static void set_tuple(std::tuple<T...> &t, const std::vector<any>& v){
      typedef typename std::tuple_element<ind, std::tuple<T...> >::type element_type;
      std::get<ind>(t) = static_cast<element_type>(any_cast<element_type>(v[ind]));
      tuple_helper<(ind - 1), T...>::set_tuple(t, v);
    }
    static void get_tuple(const std::tuple<T...> & t, std::vector<any> & v){
      v.push_back(std::get<ind>(t));
      tuple_helper<(ind - 1), T...>::get_tuple(t, v);
    }

    static void get_type(const std::tuple<T...> & t, std::vector<type_provider::type_id> & v){
      typedef typename std::tuple_element<ind, std::tuple<T...> >::type element_type;

      v.push_back(type_provider::typeOf<element_type>());
      tuple_helper<(ind - 1), T...>::get_type(t, v);

    }
  };



  template <typename... T>
  class tuple_helper < -1, T... > {
  public:
    static void set_tuple(std::tuple<T...> & t, const std::vector<any>&v){

    }
    static void get_tuple(const std::tuple<T...> & t, std::vector<any> & v){

    }

    static void get_type(const std::tuple<T...> & t, std::vector<type_provider::type_id> & v){
    }
  };


  template <typename... T>
  class tuple_helper < 0, T... > {
  public:
    static void set_tuple(std::tuple<T...> &t, const std::vector<any>& v) {
      typedef typename std::tuple_element<0, std::tuple<T...> >::type element_type;
      std::get<0>(t) = static_cast<element_type>(any_cast<element_type>(v[0]));
    }
    static void get_tuple(const std::tuple<T...> & t, std::vector<any> & v){
      v.push_back(std::get<0>(t));
    }
    static void get_type(const std::tuple<T...> & t, std::vector<type_provider::type_id> & v){
      typedef typename std::tuple_element<0, std::tuple<T...> >::type element_type;

      v.push_back(type_provider::typeOf<element_type>());
    }
  };


  template<typename TContainer>
  auto container_to_vector(const TContainer  & elements)->std::vector < typename TContainer::value_type > {
    std::vector<typename TContainer::value_type> result;
    for (auto it : elements){
      result.push_back(it);
    }
    return result;
  }

  
  template<typename TContainer, typename ... T>
  void tuple_set(std::tuple<T...> & tuple, const TContainer & container){
    auto  elements = container_to_vector(container);
    tuple_helper<sizeof...(T)-1, T...>::set_tuple(tuple, elements);
  }


  template<typename... T>
  std::vector<any> tuple_to_vector(const std::tuple<T...> & tuple){
    std::vector<any> vec;
    tuple_helper<sizeof...(T)-1, T...>::get_tuple(tuple, vec);
    std::reverse(std::begin(vec), std::end(vec));
    return vec;
  }
  /// creates a tuple from any vector 
  template<typename... T>
  std::tuple<T...> tuple_from_vector(const std::vector<any> & vec){
    std::tuple<T...> tuple;
    tuple_helper<sizeof...(T)-1, T...>::set_tuple(tuple, vec);
    return tuple;
  }

  /// returns the reflected types for each tuple element as a vector
  template<typename... T>
  std::vector < type_provider::type_id > get_tuple_types(const std::tuple<T...> & t){
    std::vector<type_provider::type_id> vec;
    tuple_helper<sizeof...(T)-1, T...>::get_type(t, vec);
    std::reverse(std::begin(vec), std::end(vec));
    return vec;
  }

NS_END(CORE_NAMESPACE)

