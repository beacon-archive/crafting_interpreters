#include "frontend/ast.hh"
#include <format>
#include <iomanip>
#include <iostream>
#include <cmath>



std::string
to_string(const beacon_lox::Expr &expr);


std::string
parenthesize(std::string_view name, const beacon_lox::Expr &expr)
{
  return std::format("({}{})", name, to_string(expr));
}
std::string
parenthesize(std::string_view name,
             const beacon_lox::Expr &expr1,
             const beacon_lox::Expr &expr2)
{
  return std::format("({} {} {})", name, to_string(expr1), to_string(expr2));
}



std::string
print_unary(const beacon_lox::UnaryExprPtr &expr)
{
  return parenthesize(expr->token.get_lexeme(), expr->expr);
}

std::string
print_literal(const beacon_lox::LiteralExprPtr &expr)
{
  // return parenthesize(expr->literal);
  return std::format("({})", expr->literal);
}

std::string
print_binary(const beacon_lox::BinaryExprPtr &expr)
{
  return std::format("({} {} {})",
                     expr->token.get_lexeme(),
                     to_string(expr->left),
                     to_string(expr->right));
}

std::string
to_string(const beacon_lox::Expr &expr)
{
  switch(expr.index())
  {
    case 0:
      return print_literal(std::get<0>(expr));
    case 1:
      return print_unary(std::get<1>(expr));
    case 2:
      return print_binary(std::get<2>(expr));
    case 3:
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
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               *expr_liter1)));
  //   (17)
  std::cout << std::format("expr_liter2:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               *expr_liter2)));
  //  (true)
  std::cout << std::format("expr_liter3:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               *expr_liter3)));

  beacon_lox::Token t1{beacon_lox::TokenType::MINUS, "-", "-", 21};
  beacon_lox::Token t2{beacon_lox::TokenType::BANS, "!", "!", 21};
  beacon_lox::Token t3{beacon_lox::TokenType::EQUAL_EQUAL, "==", "==", 21};
  beacon_lox::Token t4{beacon_lox::TokenType::BANG_EQUAL, "!=", "!=", 21};


  beacon_lox::Expr expr_unary1{
      std::make_unique<beacon_lox::UnaryExpr>(std::move(*expr_liter1),
                                              t1,
                                              beacon_lox::UnaryOp::MINUS)};
  // (MINUS - (beacon))
  std::cout << std::format("expr_unary1:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               expr_unary1)));
  beacon_lox::Expr expr_unary2{
      std::make_unique<beacon_lox::UnaryExpr>(std::move(*expr_liter2),
                                              t2,
                                              beacon_lox::UnaryOp::BANS)};
  // (BANS ! (17))
  std::cout << std::format("expr_unary2:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               expr_unary2)));
  beacon_lox::Expr expr_unary3{
      std::make_unique<beacon_lox::UnaryExpr>(std::move(*expr_liter3),
                                              t2,
                                              beacon_lox::UnaryOp::BANS)};
  // (BANS ! (true))
  std::cout << std::format("expr_unary3:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               expr_unary3)));
  beacon_lox::Expr expr_binary1{std::make_unique<beacon_lox::BinaryExpr>(
      std::move(expr_unary1),
      t3,
      beacon_lox::BinaryOp::EQUAL_EQUAL,
      std::move(expr_unary2))};
  std::cout << std::format("expr_binary1:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
                               expr_binary1)));
  beacon_lox::Expr expr_binary2{
      std::make_unique<beacon_lox::BinaryExpr>(std::move(expr_binary1),
                                               t4,
                                               beacon_lox::BinaryOp::BANG_EQUAL,
                                               std::move(expr_unary3))};
  std::cout << std::format("expr_binary2:  {}\n",
                           std::any_cast<std::string>(std::visit(
                               [&visitor](const auto &value) -> std::any
                               { return value->accept(&visitor); },
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
      std::make_unique<beacon_lox::UnaryExpr>(std::move(expr),
                                              minus,
                                              beacon_lox::UnaryOp::MINUS)};
  std::cout << std::format("{}\n", to_string(unary));

  beacon_lox::Expr b2{std::make_unique<beacon_lox::UnaryExpr>(
      std::make_unique<beacon_lox::LiteralExpr>(
          beacon_lox::Literal((float)17.2156487)),
      minus,
      beacon_lox::UnaryOp::MINUS)};
  std::cout << std::format("{}\n", to_string(b2));

  beacon_lox::Expr b3{
      std::make_unique<beacon_lox::UnaryExpr>(std::move(b2),
                                              minus,
                                              beacon_lox::UnaryOp::MINUS)};
  std::cout << std::format("{}\n", to_string(b3));


  beacon_lox::Token star{beacon_lox::TokenType::STAR, "*", "*", 3};
  beacon_lox::Expr s{std::make_unique<beacon_lox::LiteralExpr>("17.364561")};
  beacon_lox::Expr b4{
      std::make_unique<beacon_lox::BinaryExpr>(std::move(b3),
                                               star,
                                               beacon_lox::BinaryOp::STAR,
                                               std::move(s))};
  std::cout << std::format("{}\n", to_string(b4));

  float ff = 23.42132;
  std::cout << "ff:" << std::setprecision(1) << std::fixed << ff << "\n";
  return 0;
}