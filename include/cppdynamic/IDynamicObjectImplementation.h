#pragma once
#include <string>
#include <vector>

#include "config.h"
#include "any.h"
#include "dynamic.types.h"


NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
class DynamicProperty;
class DynamicObject;
class IDynamicObjectImplementation{
public:
  
  //protected:
  struct DynamicContext{ DynamicContext(){} DynamicContext(member_name_type name) :name(name){} member_name_type name; };
  typedef DynamicContext get_member_context;
  typedef DynamicContext set_member_context;
  typedef DynamicContext delete_member_context;
  typedef DynamicContext invoke_context;
  typedef DynamicContext invoke_member_context;
  typedef DynamicContext convert_context;
  typedef DynamicContext can_convert_context;
  typedef DynamicContext binary_operation_context;
  typedef DynamicContext unary_operation_context;
  typedef DynamicContext get_type_context;
  typedef DynamicContext assign_context;
  typedef DynamicObject result_type;
  // protected:
  virtual bool tryGetMemberList(member_list & members) const = 0;
  virtual bool tryInvoke(const invoke_context & context, result_type & result, const argument_list_type && arguments) = 0;
  virtual bool tryMemberInvoke(const invoke_member_context & context, result_type & result, const argument_list_type && arguments) = 0;
  virtual bool tryMemberGet(const get_member_context & context, result_type & result) = 0;
  virtual bool tryMemberSet(const set_member_context & context, DynamicObject  value) = 0;
  virtual bool tryMemberDelete(const delete_member_context & context) = 0;
  virtual bool tryConvert(const convert_context & context, any & result) = 0;
  virtual bool tryCanConvert(const any::type_id & targetType, bool & result) = 0;
  virtual bool tryGetType(any::type_id & result) = 0;
  virtual bool tryToStream(std::ostream & result)const = 0;
  //virtual bool tryAssign(const assign_context & context, any && value) = 0;
  virtual bool tryBinaryOperation(const binary_operation_context & context, result_type & result, any && lhs, any && rhs) = 0;
  virtual bool tryUnaryOperation(const unary_operation_context & context, result_type & result, any && rhs) = 0;
  //friends

  friend class DynamicObject;
  friend class DynamicProperty;
};
NS_END(CORE_DYNAMIC_NAMESPACE)
