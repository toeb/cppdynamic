#pragma once
#include "config.h"

NS_BEGIN(CORE_NAMESPACE)
  template<int ...>
  struct sequence { };

  template<int N, int ...S>
  struct generate_sequence : generate_sequence < N - 1, N - 1, S... > { };
  
  template<int ...S>
  struct generate_sequence < 0, S... > {
    typedef sequence<S...> type;
  };

NS_END(CORE_NAMESPACE)
