#pragma once
#include <memory>
#include <ostream>
#include <sstream>

#include "config.h"
#include "any.h"
#include "default.h"
#include "IDynamicObjectImplementation.h"

NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
//forward declartion for used classes
class DynamicProperty;

/// A object which can be accessed using the index operation
class DynamicObject {
public:
  typedef  std::shared_ptr<IDynamicObjectImplementation> implementation_ptr;
protected:
  implementation_ptr _implementation;
public:
  // interface operations

  // constructors
  inline DynamicObject(implementation_ptr imp);
  inline DynamicObject();
  template<typename T, typename TDecayed = typename std::decay<T>::type, bool concept =
    !std::is_same<DynamicObject, TDecayed>::value
    &&
    !std::is_same<DynamicProperty, TDecayed>::value
    &&
    !std::is_same<any, TDecayed>::value 
    && 
    !std::is_convertible<T, implementation_ptr>::value
  >
    DynamicObject(T && value,
    typename std::enable_if<concept>::type* = 0
    );
  inline DynamicObject(const DynamicProperty & property);
  inline DynamicObject(const any & value);
  inline DynamicObject(any && value);

  // indexer access to properties (set and get)
  inline auto operator[](const member_name_type & key)->DynamicProperty;
  inline auto operator[](const member_name_type & key)const->const DynamicProperty;
  // call operator
  template<typename... TArgs>
  DynamicObject operator()(TArgs && ... args);
  // list of member names
  inline auto members()const->member_list;

  // set member value
  inline void set(const member_name_type & name, DynamicObject other);
  // get member value
  inline DynamicObject get(const member_name_type & name)const;


  // assignment operators  and copy constructor
  inline DynamicObject(const DynamicObject & other);

  inline DynamicObject & operator=(const DynamicObject & other);
  inline DynamicObject & operator=(const DynamicObject && other);
  inline DynamicObject & operator =(const any & any);
  template<typename T> DynamicObject & operator=(T && t);


  // cast operator
  template<typename T> T as();
  inline  operator any();
  template<typename T> operator T();

  // type id operators
  template<typename T> bool isConvertibleTo();
  inline bool isConvertibleTo(any::type_id typeId);
  inline any::type_id getType()const;

  /// less used operations

  // custom conversion to any 
  inline any to_any()const;

  friend class DynamicProperty;

  // to string conversion
  inline std::string toString()const;
  inline void toStream(std::ostream & out)const;

  // invoke dynamic object 
  template<typename TArgumentList>   DynamicObject call(const TArgumentList & container);
  inline DynamicObject call(const argument_list_type && arguments);

  // invoke member of dynamic object
  template<typename TArgumentList> DynamicObject callMember(const member_name_type & member, const TArgumentList & container);
  inline DynamicObject callMember(const member_name_type & member, const argument_list_type && arguments);



  // assign another dynamic object
  inline void assign(const DynamicObject  & other);




};

static inline std::ostream& operator<<(std::ostream & out, const DynamicObject  & obj){ obj.toStream(out); return out; }
NS_END(CORE_DYNAMIC_NAMESPACE)


#include "DynamicProperty.h"
#include "DynamicObjectExpandoImplementation.h"
#include "DynamicObjectReflectionImplementation.h"



NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
std::string DynamicObject::toString()const{
  std::stringstream ss;
  toStream(ss);
  return ss.str();
}
void DynamicObject::toStream(std::ostream & out)const{
  auto res = _implementation->tryToStream(out);
}


bool DynamicObject::isConvertibleTo(any::type_id typeId){
  bool result;
  if (!_implementation->tryCanConvert(typeId, result))return false;
  return result;
}
any::type_id DynamicObject::getType()const{
  any::type_id type;
  if (!_implementation->tryGetType(type))return type_provider::unknown_type;
  return type;
}
template<typename T> bool DynamicObject::isConvertibleTo(){
  return isConvertibleTo(type_provider::typeOf<T>());
}

any DynamicObject::to_any()const{
  any result;
  if (_implementation->tryConvert(default_value(), result))return result;
  return any::explicit_any((DynamicObject)*this);
}
DynamicObject::DynamicObject(implementation_ptr imp) :_implementation(imp){}



auto DynamicObject::members()const->member_list{
  member_list lst;
  _implementation->tryGetMemberList(lst);
  return lst;
}
template<typename TArgumentList>
DynamicObject DynamicObject::call(const TArgumentList & container){
  return call(std::move(container_to_vector(container)));
}
DynamicObject DynamicObject::call(const argument_list_type && arguments){
  DynamicObject result;
  auto success = _implementation->tryInvoke(IDynamicObjectImplementation::invoke_context(), result, std::move(arguments));
  return result;
}
template<typename TArgumentList>
DynamicObject DynamicObject::callMember(const member_name_type & member, const TArgumentList & container){
  return callMember(member, container_to_vector(container));
}
DynamicObject DynamicObject::callMember(const member_name_type & member, const argument_list_type && arguments){
  DynamicObject result;
  auto success = _implementation->tryMemberInvoke(member, result, std::move(arguments));
  return result;
}
DynamicObject & DynamicObject::operator =(const any & any){
  DynamicObject res(any);
  assign(res);
  return *this;
}
template<typename T> DynamicObject & DynamicObject::operator=(T && t){
  DynamicObject res(t);
  assign(res);
  return *this;
}
void DynamicObject::assign(const DynamicObject  & other){
  _implementation = other._implementation;
}
DynamicObject & DynamicObject::operator=(const DynamicObject & other){
  _implementation = other._implementation;
  return *this;
}

DynamicObject & DynamicObject::operator=(const DynamicObject && other){
  _implementation = other._implementation;
  return *this;
}

template<typename T> T DynamicObject::as(){
  any result;
  _implementation->tryConvert(default_value(), result);
  return result.as<T>();

}
template<typename T> DynamicObject::operator T(){
  return as<T>();
}

DynamicObject::operator any(){
  any result;
  auto success = _implementation->tryConvert(default_value(), result);
  if (!success) {
    any(*this);
  }

  return result;
}

void DynamicObject::set(const member_name_type & name, DynamicObject  value){
  auto success = _implementation->tryMemberSet(name, value);
}
DynamicObject DynamicObject::get(const member_name_type & name)const{
  DynamicObject val;
  auto success = _implementation->tryMemberGet(name, val);
  return val;
  //any result;
  //_implementation->tryMemberGet(name, result)
}



template<typename... TArgs>
DynamicObject DynamicObject::operator()(TArgs && ... args){
  return call(std::vector<dynamic::any>{ args... });
};

auto DynamicObject::operator[](const member_name_type & key)->DynamicProperty{
  DynamicProperty prop(*this, key);
  return prop;
}

auto DynamicObject::operator[](const member_name_type & key)const->const DynamicProperty{
  DynamicProperty prop(*this, key);
  return prop;
}


DynamicObject::DynamicObject() : _implementation(std::make_shared<DynamicObjectExpandoImplementation>()){}
template<typename T, typename TDecayed, bool concept>
DynamicObject::DynamicObject(T && value, typename std::enable_if<concept>::type*
  ) : _implementation(std::make_shared<DynamicObjectReflectionImplementation>(value)){
  auto c = concept;
  auto tn = typeid(T).name();
  auto isprop = std::is_same<typename std::decay<T>::type, DynamicProperty>::value;
}

DynamicObject::DynamicObject(any && value) : _implementation(std::make_shared<DynamicObjectReflectionImplementation>(value)){}
DynamicObject::DynamicObject(const any & value) : _implementation(std::make_shared<DynamicObjectReflectionImplementation>(value)){}
DynamicObject::DynamicObject(const DynamicProperty & prop) : _implementation(prop.get()._implementation){

}
DynamicObject::DynamicObject(const DynamicObject & other) : _implementation(other._implementation){}
NS_END(CORE_DYNAMIC_NAMESPACE)
