#pragma once
#include <map>


#include "config.h"
#include "DynamicObjectImplemenationBase.h"

NS_BEGIN(CORE_DYNAMIC_NAMESPACE)


class DynamicObjectExpandoImplementation : public DynamicObjectImplementationBase, public std::enable_shared_from_this < DynamicObjectExpandoImplementation > {
  typedef std::map<member_name_type, DynamicObject> member_map_type;

public:
  virtual bool tryGetMemberList(member_list & lst)const override{
    for (auto member : _members){
      lst.push_back(member.first);
    }
    return true;
  };




  virtual bool tryMemberGet(const get_member_context & context, result_type & result)override {
    if (_members.find(context.name) == std::end(_members)){
      // _members.emplace(std::pair<member_name_type, result_type>(context.name, DynamicObject()));
      _members[context.name] = DynamicObject();

    }
    result.assign(_members[context.name]);
    return true;
  }
  virtual bool tryMemberSet(const set_member_context & context, DynamicObject  value)override {
    _members[context.name] = value;
    return true;
  }

  virtual bool tryInvoke(const invoke_context & context, result_type & result, const argument_list_type && arguments) {
    return false;
  }

  //  private:
  member_map_type _members;
};
NS_END(CORE_DYNAMIC_NAMESPACE)

