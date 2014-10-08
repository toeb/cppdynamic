#pragma once
#include "config.h"
#include "IDynamicObjectImplementation.h"

NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
/// defautl implementation for IDynamicObjectImplementation interface
/// minimal usable implementations shoudl override tryMemberGet and tryMemberSet
class DynamicObjectImplementationBase : public IDynamicObjectImplementation{
protected:
  /// implement this if you want the dynamic object's properties to be iterable
  virtual bool tryGetMemberList(member_list & list)const override{ return false; };
  /// implement this if you want your dynamic object to be invokable DynamicObject obj(your_imp); obj(...);
  virtual bool tryInvoke(const invoke_context & context, result_type & result, const argument_list_type && arguments)override{ return false; }
  /// implement this if you want your dynamic object's member to be invokable
  virtual bool tryMemberInvoke(const invoke_member_context & context, result_type & result, const argument_list_type && arguments)override{
    result_type member;
    if (!tryMemberGet(context, member))return false;
    result = member.call(arguments);
    return true;
  };
  /// implement this if you want to allow dynamic deletion of members
  virtual bool tryMemberDelete(const delete_member_context & context)override{ return false; };
  /// implement this if you want to allow read access to your dynamic object;s members
  virtual bool tryMemberGet(const get_member_context & context, result_type & result)override{
    return false; 
  };
  /// implement this if you want to allow write access to your dynamic object;s members
  virtual bool tryMemberSet(const set_member_context & context, DynamicObject   value)override{ return false; };
  /// implement this if you want your dynamic object to be convertible to any other value
  virtual bool tryConvert(const convert_context & context, any & result)override{ return false; };
  virtual bool tryCanConvert(const any::type_id & targetType, bool & result)override{
    any::type_id type;
    if (!tryGetType(type))return false;
    if (type_provider::areEqual(type, targetType))result = true;
    else result = false;
    return true;
  }
  /// implement this if you want to allow binar operations
  virtual bool tryBinaryOperation(const binary_operation_context & context, result_type & result, any && lhs, any && rhs)override{ return false; };
  /// implement this if your want to allow unary operations
  virtual bool tryUnaryOperation(const unary_operation_context & context, result_type & result, any && rhs)override{ return false; };
  /// implement this if you want to show what kind of type this dynamic object wraps (in combination with convert)
  virtual bool tryGetType(any::type_id & result)override{
    any converted;
    if (!tryConvert(convert_context(), converted))return false;
    result = converted.type();
    return true;
  }
  virtual bool tryToStream(std::ostream & result) const override{ return false; }

};
NS_END(CORE_DYNAMIC_NAMESPACE)


/// implementations
NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
/*  bool DynamicObjectImplementationBase::tryGetMemberList(member_list & list)const { return false; };
  bool DynamicObjectImplementationBase::tryInvoke(const invoke_context & context, result_type & result, const argument_list_type && arguments) { return false; };
  bool DynamicObjectImplementationBase::tryMemberInvoke(const invoke_member_context & context, result_type & result, const argument_list_type && arguments) {
  result_type member;
  if (!tryMemberGet(context, member))return false;
  result = member.call(arguments);
  return true;
  };
  bool DynamicObjectImplementationBase::tryMemberDelete(const delete_member_context & context) { return false; };

  bool DynamicObjectImplementationBase::tryMemberGet(const get_member_context & context, result_type & result) { return false; }
  bool DynamicObjectImplementationBase::tryMemberSet(const set_member_context & context, any && value) { return false; }
  bool DynamicObjectImplementationBase::tryConvert(const convert_context & context, any & result){ return false; }
  bool DynamicObjectImplementationBase::tryBinaryOperation(const binary_operation_context & context, result_type & result, any && lhs, any && rhs){ return false; };
  bool DynamicObjectImplementationBase::tryUnaryOperation(const unary_operation_context & context, result_type & result, any && rhs) { return false; };*/

  NS_END(CORE_DYNAMIC_NAMESPACE)

