#pragma once
#include <core/core.h>
#include <core/Nil.h>
NS_BEGIN(CORE_NAMESPACE)
template<size_t index, typename T0=NS(CORE_NAMESPACE)::Nil, typename T1=NS(CORE_NAMESPACE)::Nil, typename T2=NS(CORE_NAMESPACE)::Nil, typename T3=NS(CORE_NAMESPACE)::Nil, typename T4=NS(CORE_NAMESPACE)::Nil, typename T5=NS(CORE_NAMESPACE)::Nil, typename T6=NS(CORE_NAMESPACE)::Nil, typename T7=NS(CORE_NAMESPACE)::Nil, typename T8=NS(CORE_NAMESPACE)::Nil, typename T9=NS(CORE_NAMESPACE)::Nil>
struct template_argument{
typedef NS(CORE_NAMESPACE)::Nil type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<9, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T9 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<8, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T8 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<7, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T7 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<6, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T6 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<5, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T5 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<4, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T4 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<3, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T3 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<2, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T2 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<1, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T1 type;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
struct template_argument<0, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>{
typedef T0 type;
};


NS_END(CORE_NAMESPACE)
