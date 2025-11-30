#include "frontend/ast.hh"
#include <format>
#include <iomanip>
#include <iostream>



std::string
to_string(beacon_lox::Expr const& expr);


std::string
parenthesize(std::string_view name, beacon_lox::Expr const& expr)
{
  return std::format("({}{})", name, to_string(expr));
}
std::string
parenthesize(std::string_view name,
             beacon_lox::Expr const& expr1,
             beacon_lox::Expr const& expr2)
{
  return std::format("({} {} {})", name, to_string(expr1), to_string(expr2));
}



std::string
print_unary(beacon_lox::UnaryExprPtr const& expr)
{
  return parenthesize(expr->token.get_lexeme(), expr->expr);
}

std::string
print_literal(beacon_lox::LiteralExprPtr const& expr)
{
  // return parenthesize(expr->literal);
  return std::format("({})", expr->literal);
}

std::string
print_binary(beacon_lox::BinaryExprPtr const& expr)
{
  return std::format("({} {} {})",
                     expr->token.get_lexeme(),
                     to_string(expr->left),
                     to_string(expr->right));
}

std::string
to_string(beacon_lox::Expr const& expr)
{
  switch(expr.index())
  {
    case 1:
      return print_literal(std::get<1>(expr));
    case 2:
      return print_unary(std::get<2>(expr));
    case 3:
      return print_binary(std::get<3>(expr));
    case 4:
      return {"22"};
    default:
      //error
      return "";
  }
}

int
main()
{
  auto expr_liter1{std::make_unique<beacon_lox::Expr>(
      std::make_unique<beacon_lox::LiteralExpr>("beacon"))};
  auto expr_liter2{std::make_unique<beacon_lox::Expr>(
      std::make_unique<beacon_lox::LiteralExpr>(17.))};
  auto expr_liter3{std::make_unique<beacon_lox::Expr>(
      std::make_unique<beacon_lox::LiteralExpr>(true))};
  auto expr_liter4{std::make_unique<beacon_lox::Expr>(
      std::make_unique<beacon_lox::LiteralExpr>("uan"))};


  beacon_lox::ExprVisitor visitor;
  //  (beacon)
  std::cout << std::format("expr_liter1:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               *expr_liter1)));
  //   (17)
  std::cout << std::format("expr_liter2:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               *expr_liter2)));
  //  (true)
  std::cout << std::format("expr_liter3:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               *expr_liter3)));

  beacon_lox::Token t1{beacon_lox::TokenType::MINUS, "-", "-", 21};
  beacon_lox::Token t2{beacon_lox::TokenType::BANS, "!", "!", 21};
  beacon_lox::Token t3{beacon_lox::TokenType::EQUAL_EQUAL, "==", "==", 21};
  beacon_lox::Token t4{beacon_lox::TokenType::BANG_EQUAL, "!=", "!=", 21};


  beacon_lox::Expr expr_unary1{
      std::make_unique<beacon_lox::UnaryExpr>(t1, std::move(*expr_liter1))};
  // (MINUS - (beacon))
  std::cout << std::format("expr_unary1:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               expr_unary1)));
  beacon_lox::Expr expr_unary2{
      std::make_unique<beacon_lox::UnaryExpr>(t2, std::move(*expr_liter2))};
  // (BANS ! (17))
  std::cout << std::format("expr_unary2:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               expr_unary2)));
  beacon_lox::Expr expr_unary3{
      std::make_unique<beacon_lox::UnaryExpr>(t2, std::move(*expr_liter3))};
  // (BANS ! (true))
  std::cout << std::format("expr_unary3:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               expr_unary3)));
  beacon_lox::Expr expr_binary1{
      std::make_unique<beacon_lox::BinaryExpr>(std::move(expr_unary1),
                                               t3,
                                               std::move(expr_unary2))};
  std::cout << std::format("expr_binary1:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               expr_binary1)));
  beacon_lox::Expr expr_binary2{
      std::make_unique<beacon_lox::BinaryExpr>(std::move(expr_binary1),
                                               t4,
                                               std::move(expr_unary3))};
  std::cout << std::format("expr_binary2:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](auto const& value) -> std::any
                               {
                                 using T = std::decay_t<decltype(value)>;
                                 if constexpr(std::is_same_v<T, std::monostate>)
                                 {
                                   return {"nil"}; // 或者处理空指针逻辑
                                 }
                                 else
                                 {
                                   return value->accept(&visitor);
                                 }
                               },
                               expr_binary2)));
  return 0;
}


int
main2()
{
  beacon_lox::Literal li("beacon");
  beacon_lox::Expr expr;
  expr = std::make_unique<beacon_lox::LiteralExpr>(li);
  std::cout << std::format("{}\n", to_string(expr));


  beacon_lox::Token minus(beacon_lox::TokenType::MINUS, "-", "-", 2);
  beacon_lox::Expr unary{
      std::make_unique<beacon_lox::UnaryExpr>(minus, std::move(expr))};
  std::cout << std::format("{}\n", to_string(unary));

  beacon_lox::Expr b2{std::make_unique<beacon_lox::UnaryExpr>(
      minus,
      std::make_unique<beacon_lox::LiteralExpr>(
          beacon_lox::Literal((float)17.2156487)))};
  std::cout << std::format("{}\n", to_string(b2));

  beacon_lox::Expr b3{
      std::make_unique<beacon_lox::UnaryExpr>(minus, std::move(b2))};
  std::cout << std::format("{}\n", to_string(b3));


  beacon_lox::Token star{beacon_lox::TokenType::STAR, "*", "*", 3};
  beacon_lox::Expr s{std::make_unique<beacon_lox::LiteralExpr>("17.364561")};
  beacon_lox::Expr b4{std::make_unique<beacon_lox::BinaryExpr>(std::move(b3),
                                                               star,
                                                               std::move(s))};
  std::cout << std::format("{}\n", to_string(b4));

  float ff = 23.42132;
  std::cout << "ff:" << std::setprecision(1) << std::fixed << ff << "\n";
  return 0;
}