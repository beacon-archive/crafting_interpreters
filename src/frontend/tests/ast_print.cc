#include "frontend/ast.hh"
#include "ast_visiter.hh"
#include <format>
#include <iomanip>
#include <iostream>

namespace
{

void
print_expr_ptr()
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
  try
  {
    //  (beacon)
    std::cout << std::format(
        "expr_liter1:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, *expr_liter1)));
    //   (17)
    std::cout << std::format(
        "expr_liter2:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, *expr_liter2)));
    //  (true)
    std::cout << std::format(
        "expr_liter3:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, *expr_liter3)));

    beacon_lox::Token t1{beacon_lox::TokenType::MINUS, "-", "-", 21};
    beacon_lox::Token t2{beacon_lox::TokenType::BANS, "!", "!", 21};
    beacon_lox::Token t3{beacon_lox::TokenType::EQUAL_EQUAL, "==", "==", 21};
    beacon_lox::Token t4{beacon_lox::TokenType::BANG_EQUAL, "!=", "!=", 21};


    beacon_lox::Expr expr_unary1{
        std::make_unique<beacon_lox::UnaryExpr>(t1, std::move(*expr_liter1))};
    // (MINUS - (beacon))
    std::cout << std::format(
        "expr_unary1:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, expr_unary1)));
    beacon_lox::Expr expr_unary2{
        std::make_unique<beacon_lox::UnaryExpr>(t2, std::move(*expr_liter2))};
    // (BANS ! (17))
    std::cout << std::format(
        "expr_unary2:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, expr_unary2)));
    beacon_lox::Expr expr_unary3{
        std::make_unique<beacon_lox::UnaryExpr>(t2, std::move(*expr_liter3))};
    // (BANS ! (true))
    std::cout << std::format(
        "expr_unary3:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, expr_unary3)));
    beacon_lox::Expr expr_binary1{
        std::make_unique<beacon_lox::BinaryExpr>(std::move(expr_unary1),
                                                 t3,
                                                 std::move(expr_unary2))};
    std::cout << std::format(
        "expr_binary1:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, expr_binary1)));
    beacon_lox::Expr expr_binary2{
        std::make_unique<beacon_lox::BinaryExpr>(std::move(expr_binary1),
                                                 t4,
                                                 std::move(expr_unary3))};
    std::cout << std::format(
        "expr_binary2:  {}\n",
        std::any_cast<std::string>(std::visit(visitor, expr_binary2)));
  }

  catch(std::exception const& e)
  {
    std::cout << "error: " << e.what() << "\n";
  }
}

int
print_expr()
{
  beacon_lox::ExprVisitor expr_visitor;
  beacon_lox::Literal li("beacon");
  beacon_lox::Expr expr;
  expr = std::make_unique<beacon_lox::LiteralExpr>(li);
  std::cout << std::format("li: {}\n", std::visit(expr_visitor, expr));


  beacon_lox::Token minus(beacon_lox::TokenType::MINUS, "-", "-", 2);
  beacon_lox::Expr unary{
      std::make_unique<beacon_lox::UnaryExpr>(minus, std::move(expr))};
  std::cout << std::format("unary: {}\n", std::visit(expr_visitor, unary));

  beacon_lox::Expr b2{std::make_unique<beacon_lox::UnaryExpr>(
      minus,
      std::make_unique<beacon_lox::LiteralExpr>(
          beacon_lox::Literal((float)17.2156487)))};
  std::cout << std::format("b2: {}\n", std::visit(expr_visitor, b2));

  beacon_lox::Expr b3{
      std::make_unique<beacon_lox::UnaryExpr>(minus, std::move(b2))};
  std::cout << std::format("b3: {}\n", std::visit(expr_visitor, b3));


  beacon_lox::Token star{beacon_lox::TokenType::STAR, "*", "*", 3};
  beacon_lox::Expr s{std::make_unique<beacon_lox::LiteralExpr>("17.364561")};
  beacon_lox::Expr b4{std::make_unique<beacon_lox::BinaryExpr>(std::move(b3),
                                                               star,
                                                               std::move(s))};
  std::cout << std::format("b4: {}\n", std::visit(expr_visitor, b4));

  float ff = 23.42132;
  std::cout << "ff:" << std::setprecision(1) << std::fixed << ff << "\n";
  return 0;
}

} // namespace

int
main()
{
  print_expr_ptr();
  print_expr();
  return 0;
}
