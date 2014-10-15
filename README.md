# cppdynamic

Dynamic object are useful, especially in rapid prototyping, non-performance critical situations, and situations in which data/functions -bags are needed (non schema specific data).
Also when serializing and deserializing dynamic objects can be a very valuable asset.

Dynamic programming languages inherently support this. e.g.

    var obj = {};
    obj.a = 'asd';
    obj.b = {};
    obj.b.c = 3;
    obj.d = function(i,j){ return i+j;}
    

C# Also supports dynamic objects utilizing the `dynamic` keyword:

    dynamic obj = new ExpandoObject();
    obj.a = "asd";
    obj.b = new ExpandObject();
    obj.b.c = 3;
    obj.d = (int i, int j)=>i+j;
    

I tried as to stay close to c#'s dynamic objects when trying to implement them for c++. Of course I could not alter the language so I have to use the indexer operator `operator[](const std::string &)`  


My syntax is as follows:

    DynamicObject obj;
    obj["a"] = "asd";
    obj["b"] = DynamicObject();
    obj["b"]["c"] = 3;
    obj["d"] = [](int i, int j){return i+j;};
    

Here is a working example of what you can do:

    #include <core.dynamic.h>
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
        assert("hello" == (std::string)uut);
      }
      {
        // dynamic object can be reassigned
        dynamic::DynamicObject uut = 33;
        assert(33 == (int)uut);
        uut = std::string("hello");
        assert("hello" == (std::string)uut);
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
        std::function<int(int, int)> prop4 = uut["prop4"];
        auto result2 = prop4(3, 4);
    
    
        assert(prop1 == 33);
        assert(prop2 == "hello");
        assert(prop31 == 5);
        assert(result == 11);
        assert(result2 == 7);
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

Caveats:

*   Functors with multiple overloads of `operator()` cannot be used as members or values (the is_callable<> and func_traits<> type traits would need to be specialized for the corresonding type.) This is especially sad, because no compiler independent version for std::bind return functors are usable.
*   Method modifiers except nonconst and const are not implemented.
*   

Since c++ is statically typed working around it can be hard at times, however it is possible thanks to operator overloading. Especially `operator()`, `operator[]`, `operator T()` are usefull to allow dynamic objects.