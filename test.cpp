#include <cppdynamic/dynamic.h>
#include <assert.h>

int main(){
    {
      // dynamic object can be a fundamental type
      dynamic::DynamicObject uut = 33;
      assert(33 == (int)uut);

    }
  {
    // dynamic object can be a complex type
    dynamic::DynamicObject uut = std::string("hello");
    std::string res = uut;
    assert("hello" == res);
  }
  {
    // dynamic object can be reassigned
    dynamic::DynamicObject uut = 33;
    assert(33 == (int)uut);
    uut = std::string("hello");
    std::string res = uut;
    assert("hello" == res);
  }
  {
    //dynamic object can be a functor
    dynamic::DynamicObject uut = [](int i, int j){return i + j; };
    int result = uut(3, 4);
    assert(result == 7);
  }

  {
    // dynamic object can be an expando object
    dynamic::DynamicObject uut;
    uut["prop1"] = 33;
    uut["prop2"] = std::string("hello");
    uut["prop3"]["prop31"] = 5;
    uut["prop4"] = [](int i, int j){return i + j; };
    int prop1 = uut["prop1"];
    std::string prop2 = uut["prop2"];
    int prop31 = uut["prop3"]["prop31"];
    int result = uut["prop4"](5, 6);

    assert(prop1 == 33);
    assert(prop2 == "hello");
    assert(prop31 == 5);
    assert(result == 11);
  }

  {
    // you can create a custom dynamic object implementation:
    class MyImp : public dynamic::DynamicObjectImplementationBase{
    protected:
      virtual bool tryMemberGet(const get_member_context & context, result_type & result)override
      {
        if (context.name == "prop1"){
          result = 44;
          return true;
        }
        if (context.name == "prop2"){
          result = std::string("asd");
          return true;
        }

        return false;
      }
    };
    // initialization is still a bit complex 
    dynamic::DynamicObject uut = 
      std::dynamic_pointer_cast<dynamic::IDynamicObjectImplementation>(std::make_shared<MyImp>());
    int a = uut["prop1"];
    std::string b = uut["prop2"];

    assert(a == 44);
    assert(b == "asd");

  }



}
