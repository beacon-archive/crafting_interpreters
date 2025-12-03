#pragma once

// 它让 C++ 拥有了类似 Rust 或 Haskell 中的“模式匹配”（Pattern Matching）语法体验。
// Overload 把多个 lambda 合并到一个对象中，并把它们的 operator() 全部引入到对象中。visit 调用这个对象的重载 operator()，因此根据类型自动选中正确的 lambda。
// visit 的函数签名:
//                visit(_Visitor&& __visitor, _Variants&&... __variants)
// 本身就是传入一个访问器,然后调用访问对应的()操作符来处理对应的类型,或者传入lambda


//  天,自己没有看明白这里是多重继承..., Overloaded 会继承所有传入的 lambda 的 operator()
template <class... Ts>
struct Overloaded : Ts... // <- 关键点1, 多重继承
{
  using Ts::operator()...; // <- 关键点2, 引入重载
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>; // <- 关键点3, 推导指引

// 关键点1:
// 当传入多个 lambda(Lambda 表达式在 C++ 中本质上是匿名类（Functor）。每个 Lambda 都有自己独一无二的类型。) 时, Ts...代表这些类型
// struct Overloaded 继承这些所有的 lambda, 这意味着 Overloaded 拥有所有 lambda 的实体(虽然没有数据成员, 但是结构上是包含关系).

// 关键点2:
// *在 c++ 中,如果派生类没有重写基类函数, 且基类函数之间有同名函数(这里都是 operator()),直接调用会产生歧义或者被隐藏(这是重点!!)
// using Ts::Operator().. 这是折叠表达式, 它等价于对每一个父类 lambda 执行:
//          `using LambdaType1::operator();`
//          `using LambdaType2::operator();`
//          这会将所有父(lamdba)类的 operator() 函数都添加到 Overloaded 中,形成一个 重载函数集. 这样当当调用 overloaded_obj(arg) 是,编译器就可以根据 arg 的类型, 通过 重载决议(Overload Resolution) 自动匹配最适合的那个 lambda


// 关键点3:
//  这是 c++17 的特性, 它告诉编译器, 当我想构造 Overloaded(lambda1, lambda2) 时,请自动推到出模板参数 Ts... 为 Overloaded<Type1, Type2>
//  *          如果没有这一行,就要写成: `Overloaded<decltype(lambda1), decltype(lambda2)>(...)`


// 实际的调用顺序是这样的:
//  第一步构造对象:（编译期 + 运行期初始化）
//          Overloaded{ lambda1, lambda2 }, 当看到这个时,编译器不仅要创建对象, 还需要知道这个对象的类型是什么.
//          看到 Overloaded 是一个模板, 但是没有传入类型, 于是用推到指引推断出类型
//          确定了类型之后,再根据类型生成一个具体的类:
//          struct Overloaded_Specific : Lambda1, Lambda2 {
//             using Lambda1::operator(); // 把 lambda1 的 () 拿进来
//             using Lambda2::operator(); // 把 lambda2 的 () 拿进来
//          };
//          最终程序创建了这个对象.

// 推导指引只是在没有实例化类型时,帮忙推导,实现起作用的还是模板
