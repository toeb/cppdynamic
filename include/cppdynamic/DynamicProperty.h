#pragma once
#include "config.h"
#include "dynamic.types.h"
NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
class DynamicObject;
/// Class for accessing DynamicObject's properties by indexer
class DynamicProperty{
  friend class DynamicObject;
protected:
  // constructor
  inline DynamicProperty(DynamicObject obj, const member_name_type & key);
public:
  // no copy constructor todo....
  //DynamicProperty(const DynamicProperty &) = delete;

  // custom conversion to any
  inline any to_any()const;
  template<typename T, typename TNext = typename std::enable_if<!std::is_same<typename std::decay<T>::type, DynamicProperty>::value, T>::type>
  DynamicProperty & operator=(T && val);
  inline DynamicProperty & operator=(const DynamicProperty & other);
  // set property;
  inline void set(DynamicObject  val);
  inline DynamicObject get()const;

  inline any value()const;// todo remove


  inline auto operator[](const member_name_type & key)->DynamicProperty;


  // cast operations
  template<typename T> operator T();
  template<typename T> T as();
  inline operator DynamicObject();

  // vararg call operator  DynamicProperty& prop; prop(1,2, any(3), DynamicObject(4));....
  template<typename ... TArgs> DynamicObject operator()(TArgs && ... args);

  // dynamic call operations for any type of std container or vector of any (initializer lsit constructor usable ie
  //  call({2,3,4}); // in that case usage of operator() would be easier
  template<typename TArgumentList> DynamicObject call(const TArgumentList & arguments);
  inline DynamicObject call(const argument_list_type & arguments);
private:
  DynamicObject  _object;
  member_name_type _name;
};
inline static std::ostream & operator<< (std::ostream & out, const DynamicProperty & prop){
  out << static_cast<DynamicObject>(prop);
  return out;
}
/// implementation
any DynamicProperty::to_any()const{
  auto prop = _object.get(_name);
  return prop.to_any();
}
DynamicProperty::DynamicProperty(DynamicObject obj, const member_name_type & key) :_object(obj), _name(key){

}
template<typename T> DynamicProperty::operator T(){
  return as<T>();
}
inline DynamicProperty::operator DynamicObject() {
  return get();
}
template<typename T, typename TNext> DynamicProperty & DynamicProperty::operator=(T && val){
  set(val);
  return *this;
}
DynamicProperty & DynamicProperty::operator = (const DynamicProperty & other){
  set(other.get());
  return *this;
}


inline DynamicObject DynamicProperty::get()const{
  return _object.get(_name);
}

void DynamicProperty::set(DynamicObject  val){
  _object.set(_name, val);
}

template<typename T> T DynamicProperty::as(){
  return _object.get(_name).as<T>();
}

template<typename TArgumentList>
DynamicObject DynamicProperty::call(const TArgumentList & arguments){
  return call(container_to_vector(arguments));
}
DynamicObject DynamicProperty::call(const argument_list_type & arguments){
  return _object.callMember(_name, arguments);
}
template<typename ... TArgs>
DynamicObject DynamicProperty::operator()(TArgs && ... args){

  return call(std::vector < any > { static_cast<any>(args)... });
}


auto DynamicProperty::operator[](const member_name_type & key)->DynamicProperty{
  auto obj = _object.get(_name);
  return obj[key];

}

NS_END(CORE_DYNAMIC_NAMESPACE)