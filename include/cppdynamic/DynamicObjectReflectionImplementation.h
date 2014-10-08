#pragma once
#include "config.h"
#include "DynamicObjectImplemenationBase.h"
NS_BEGIN(CORE_DYNAMIC_NAMESPACE)
class DynamicObjectReflectionImplementation : public DynamicObjectImplementationBase{
private:
  any _value;
public:
  DynamicObjectReflectionImplementation(any value) :_value(value){}
protected:

  virtual bool tryMemberInvoke(const invoke_member_context & context, result_type & result, const argument_list_type && arguments)override {
    auto type = _value.type();
    //        auto method = type->getMethod(context.name);
    //      if (!method)return false;
    return false;
  }



  virtual bool tryConvert(const convert_context & context, any & result)override{
    result = _value;
    return true;
  }

  virtual bool tryInvoke(const invoke_context & context, result_type & result, const argument_list_type && arguments) {
    if (!_value.isCallable())return false;
    result.assign(DynamicObject(_value.call(std::move(arguments))));
    return true;
  }


  virtual bool tryToStream(std::ostream & result) const override{
    auto res = _value.toStream(result);
    return res;
  }

  virtual bool tryCanConvert(const any::type_id & targetType, bool & result)override{
    result = type_provider::areEqual(targetType, _value.type());
    return true;
  }

  virtual bool tryGetType(any::type_id & result)override{
    result = _value.type();
    return true;
  }


};
NS_END(CORE_DYNAMIC_NAMESPACE)
