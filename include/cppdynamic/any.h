#pragma once

#include <type_traits>
#include <memory>

#include "config.h"
#include "is_callable.h"

NS_BEGIN(CORE_NAMESPACE)

// definitions for type system
  struct type_provider{
    //typedef const Type * type_id;
    typedef size_t type_id;
    inline static bool  areEqual(type_id a, type_id b);
    inline static bool  isAssignableFrom(type_id a, type_id b);
    template<typename T> static type_id typeOf();
    template<typename T> static type_id typeOf(const T & t);
    const static type_id unknown_type = 0;
  };
  class any;
  

  /// any class taken from boost and modified
  class any
  {
  public: // structors
    class placeholder;
    typedef type_provider::type_id type_id;
    inline any()noexcept;
    template<typename ValueType, bool = is_callable<ValueType>::value> any(const ValueType & value);
    inline any(const any & other);

    // Move constructor
    inline any(any&& other) noexcept;

    // Perfect forwarding of ValueType
    template<typename ValueType, bool = is_callable<ValueType>::value, bool = has_to_any_op<ValueType>::value>
    any(ValueType&& value
      , typename std::enable_if<!std::is_same<any&, ValueType>::value >::type* = 0 // disable if value has type `any&`
      , typename std::enable_if<!std::is_const<ValueType>::value >::type* = 0// disable if value has type `const ValueType&&`
      , typename std::enable_if<!has_to_any_op<ValueType>::value>::type* = 0// disable if type is custom convertible to any
      );
    
    template<typename ValueType>
    static any explicit_any(ValueType && value);

    template<typename T>
    static placeholder* createContent(T&& t, typename std::enable_if<has_to_any_op<T>::value>::type* = 0);

    template<typename T>
    static placeholder* createContent(T&& t, typename std::enable_if<!has_to_any_op<T>::value>::type* = 0);


    inline ~any() noexcept;
    template<typename T> operator T();

    template<typename TArgumentList> any call(TArgumentList & args);
    template<typename... TArgs> any operator()(TArgs&& ... args);

  public: // modifiers
    inline bool toStream(std::ostream & stream)const;
    inline any & swap(any & rhs) noexcept;
    inline any & operator=(const any& rhs);
    // move assignement
    inline any & operator=(any&& rhs) noexcept;
    // Perfect forwarding of ValueType
    template <class ValueType>  any & operator=(ValueType&& rhs);

  public: // queries
    template<typename T> typename std::enable_if<std::is_default_constructible<T>::value, T>::type as();
    template<typename T> typename std::enable_if<!std::is_default_constructible<T>::value, T>::type as();

    inline bool empty() const noexcept;
    inline void clear() noexcept;
    inline bool isCallable()const;
    inline type_id  type() const noexcept;
  private: // representation

    template<typename ValueType>  friend ValueType * any_cast(any *) noexcept;
    template<typename ValueType>  friend ValueType * unsafe_any_cast(any *) noexcept;
    placeholder * content;
  };

  inline std::ostream & operator<< (std::ostream & out, const any & val){
    val.toStream(out);
    return out;
  }




  /// thrown an any cast is not correct
  class  bad_any_cast : public std::bad_cast
  {
  public:
    virtual const char * what() const noexcept
    {
      return "bad_any_cast: "
      "failed conversion using boost::any_cast";
    }
  };

  /// swaps the contents of the left hand side and right hand side
  inline void swap(any & lhs, any & rhs) noexcept;
  /// casts the any type pointer to a value type
  template<typename ValueType>  ValueType * any_cast(any * operand) noexcept;
  /// 
  template<typename ValueType>  inline const ValueType * any_cast(const any * operand) noexcept;
  template<typename ValueType>  ValueType any_cast(any & operand);
  template<typename ValueType>  inline ValueType any_cast(const any & operand);
  //  template<typename ValueType>  inline ValueType&& any_cast(any&& operand);
  template<typename ValueType>  inline ValueType * unsafe_any_cast(any * operand) noexcept;
  template<typename ValueType>  inline const ValueType * unsafe_any_cast(const any * operand) noexcept;

  //speciializion for custom conversion to any
  template<typename TFrom>
  struct convert_to_any{
    static any convert(TFrom && value){ return value; }
  };
NS_END(CORE_NAMESPACE)

#include "dynamic_call.h"

NS_BEGIN(CORE_NAMESPACE)

inline std::ostream & operator<<(std::ostream & out, const Void &){ return out; }

inline bool type_provider::areEqual(type_id a, type_id b){
  return a == b;
  // return a->getRttiHashCode() == b->getRttiHashCode();
}
template<typename T>auto  type_provider::typeOf()->type_id{
  //return  type_of<T>();
  auto name = typeid(T).name();
  return typeid(T).hash_code();
}


inline bool type_provider::isAssignableFrom(type_id a, type_id b){
  return areEqual(a, b);
}
template<typename T>auto type_provider::typeOf(const T & t)->type_id{ return typeOf<T>(); }

/*
bool type_provider::areEqual(type_id a, type_id b){
return a->getRttiHashCode() == b->getRttiHashCode();
}

bool type_provider::isAssignableFrom(type_id a, type_id b){
return areEqual(a, b);
}
template<typename T>auto  type_provider::typeOf()->type_id{
return typeof(T);
}
template<typename T>auto type_provider::typeOf(const T & t)->type_id{ return typeof<T>(); }

*/
class any::placeholder{
public: // structors

  virtual ~placeholder()
  {
  }

public: // queries

  virtual type_provider::type_id type() const noexcept = 0;
  virtual bool isCallable()const = 0;
  virtual placeholder * clone() const = 0;
  virtual any call(const std::vector<any> & argument_list){ return any(); };
  virtual bool toStream(std::ostream & out)const{ return false; }
};
namespace has_insertion_operator_impl {
  typedef char no;
  typedef char yes[2];

  struct any_t {
    template<typename T> any_t(T const&);
  };

  no operator<<(std::ostream const&, any_t const&);

  yes& test(std::ostream&);
  no test(no);

  template<typename T>
  struct has_insertion_operator {
    static std::ostream  s;
    static T const &t;
    static bool const value = sizeof(test(s << t)) == sizeof(yes);
  };
}

template<typename T>
struct has_insertion_operator :
  has_insertion_operator_impl::has_insertion_operator < T > {
};

template<typename ValueType, bool is_callable_value = is_callable<ValueType>::value>
class holder : public any::placeholder
{
public: // structors

  holder(const ValueType & value)
    : held(value)
  {

  }
  holder(ValueType&& value)
    : held(static_cast<ValueType&&>(value))
  {


  }
public: // queries

  virtual type_provider::type_id type() const override noexcept
  {
    return  type_provider::typeOf<ValueType>();
  }

    virtual placeholder * clone() const override noexcept
  {
    return new holder(held);
  }

    //      template<typename T>
    //    typename std::enable_if<!has_insertion_operator<T>::value, bool>::type to_stream(std::ostream & out)const{
    //      return false;
    //    }

    template<typename TRet, typename TClass, typename ...TArgs>
  struct OverloadHelper{
    typedef TRet(TClass::* func_type)(TArgs...);
    static func_type get_overload(func_type func){ return func; }
  };



  //    template<typename T>
  //    typename std::enable_if<has_insertion_operator<T>::value, bool>::type to_stream(std::ostream & out)const{
  //      //out << held;
  //      return true;
  //    }

  virtual bool toStream(std::ostream & out)const{
    //      return this->to_stream<ValueType>(out);
    //out << "tostream";
    //  out << held;
    return false;
  }

private:

  /// callable held types return the result of the call operation
  template<typename TX = any>
  static typename std::enable_if<is_callable_value, TX>::type doCall(ValueType & value, const std::vector<any> & argument_list){
    return dynamic_call(value, argument_list);
  }
  // non callable held types reutrn the value
  template<typename TX = any>
  static typename std::enable_if<!is_callable_value, TX>::type doCall(ValueType & value, const std::vector<any> & argument_list){
    return value;
  }



public:
  virtual bool isCallable()const{
    return is_callable_value;
  }

  virtual any call(const std::vector<any> & argument_list){
    return doCall(held, argument_list);
    //return any();
  };



public: // representation

  ValueType held;

private: // intentionally left unimplemented


  holder & operator=(const holder &);
};


template<typename TArgumentList> any any::call(TArgumentList & args){
  std::vector<any> arglst;
  for (auto arg : args){
    arglst.push_back(arg);
  }
  return content->call(arglst);
  //return any();

}
template<typename... TArgs> any any::operator()(TArgs && ... args){
  std::vector<any> dArgs{ args... };
  return call(dArgs);
}


template<typename ValueType, bool>
any::any(const ValueType & value)
  : content(new holder<typename std::decay<const ValueType>::type>(value))
{
}

any::any() noexcept : content(0){}
any::any(const any & other) : content(other.content ? other.content->clone() : 0)
{
}
any::any(any&& other) noexcept
  : content(other.content)
{
  other.content = 0;
}


template<typename ValueType, bool, bool >
any::any(ValueType&& value
  , typename std::enable_if<!std::is_same<any&, ValueType>::value >::type*  // disable if value has type `any&`
  , typename std::enable_if<!std::is_const<ValueType>::value >::type* // disable if value has type `const ValueType&&`
  , typename std::enable_if<!has_to_any_op<ValueType>::value>::type* // disable if type is custom convertible to any
  )
  : content(createContent(std::move(value)))
{
}


template<typename ValueType>
any any::explicit_any(ValueType && value){
  any any;
  any.content = new holder<typename std::decay<ValueType>::type>(static_cast<ValueType&&>(value));
  return any;
}



template<typename T>
auto any::createContent(T&&value, typename std::enable_if<has_to_any_op<T>::value>::type*)->placeholder*{
  auto any_val = value.to_any();

  return any_val.content->clone();
}


template<typename T>
auto any::createContent(T&&value, typename std::enable_if<!has_to_any_op<T>::value>::type*)->placeholder*{
  return new holder< typename std::decay<T>::type >(static_cast<T&&>(value));
}


any::~any() noexcept
{
  delete content;
}

inline bool any::isCallable()const
{
  return content->isCallable();
}


template<typename T> typename std::enable_if<std::is_default_constructible<T>::value, T>::type any::as(){
  auto res = any_cast<T>(this);
  if (!res)return T();
  return *res;
}
template<typename T> typename std::enable_if<!std::is_default_constructible<T>::value, T>::type any::as(){
  auto res = any_cast<T>(this);
  if (!res)throw new std::exception();//("cannot cast.");
  return *res;
}
bool any::toStream(std::ostream & out)const{
  return this->content->toStream(out);
}
any & any::swap(any & rhs) noexcept
{
  std::swap(content, rhs.content);
  return *this;
}

any & any::operator=(const any& rhs)
{
  any(rhs).swap(*this);
  return *this;
}

// move assignement
any & any::operator=(any&& rhs) noexcept
{
  rhs.swap(*this);
  any().swap(rhs);
  return *this;
}

// Perfect forwarding of ValueType
template <class ValueType>
any & any::operator=(ValueType&& rhs)
{
  any(static_cast<ValueType&&>(rhs)).swap(*this);
  return *this;
}


bool any::empty() const noexcept
{
  return !content;
}

void any::clear() noexcept
{
  any().swap(*this);
}

type_provider::type_id any::type() const noexcept
{

  return content ? content->type() : type_provider::typeOf<void>();
}

inline void swap(any & lhs, any & rhs) noexcept
{
  lhs.swap(rhs);
}


template<typename ValueType>
ValueType * any_cast(any * operand) noexcept
{
  if (!operand)return 0;
  auto type = operand->type();
  auto type2 = type_provider::typeOf<ValueType>();

  if (!type_provider::areEqual(type, type2))return 0;
  return &static_cast<holder<ValueType> *>(operand->content)->held;
}


template<typename ValueType>
inline const ValueType * any_cast(const any * operand) noexcept
{
  return any_cast<ValueType>(const_cast<any *>(operand));
}


template<typename ValueType>
ValueType any_cast(any & operand)
{
  typedef typename  std::remove_reference<ValueType>::type nonref;


  nonref * result = any_cast<nonref>(&operand);
  if (!result) throw new bad_any_cast();

  return *result;
}
template<typename ValueType>
inline ValueType any_cast(const any & operand)
{
  typedef typename std::remove_reference<ValueType>::type nonref;
  return any_cast<const nonref &>(const_cast<any &>(operand));
}
/* template<typename ValueType>
inline ValueType&& any_cast(any&& operand)
{

static_assert(
std::is_rvalue_reference<ValueType&&>::value
|| std::is_const< typename std::remove_reference<ValueType>::type >::value,
"any_cast shall not be used for getting nonconst references to temporary objects"
);
return any_cast<ValueType&&>(operand);
}*/
// Note: The "unsafe" versions of any_cast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template<typename ValueType>
inline ValueType * unsafe_any_cast(any * operand) noexcept
{
  typedef NS(CORE_NAMESPACE)::holder<ValueType> return_type;
  return &static_cast<return_type*>(operand->content)->held;
}

template<typename ValueType>
inline const ValueType * unsafe_any_cast(const any * operand) noexcept
{
  return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
}



template<typename T> any::operator T(){
  return any_cast<T>(*this);
}

NS_END(CORE_NAMESPACE)