#pragma once 


// Overload 把多个 lambda 合并到一个对象中，并把它们的 operator() 全部引入到对象中。visit 调用这个对象的重载 operator()，因此根据类型自动选中正确的 lambda。
// visit 的函数签名:
//                visit(_Visitor&& __visitor, _Variants&&... __variants)
// 本身就是传入一个访问器,然后调用访问对应的()操作符来处理对应的类型,或者传入lambda
template <class... Ts>
struct Overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

