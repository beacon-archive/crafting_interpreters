#include "ast_visiter.hh"

namespace beacon_lox
{

auto
ExprVisitor::operator()(LiteralExprPtr const& literal) -> std::string
{
  return std::format("({})", literal->literal);
}

auto
ExprVisitor::operator()(UnaryExprPtr const& unary) -> std::string
{
  return std::format("({} {} {})",
                     unary->token.get_type(),
                     unary->token.get_lexeme(),
                     std::visit(*this, unary->expr));
}

auto
ExprVisitor::operator()(BinaryExprPtr const& binary) -> std::string
{
  return std::format("({} {} {} {})",
                     binary->token.get_type(),
                     binary->token.get_lexeme(),
                     std::visit(*this, binary->left),
                     std::visit(*this, binary->right));
}

auto
ExprVisitor::operator()(GroupingExprPtr const& grouping) -> std::string
{
  return std::format("(grouping {})", std::visit(*this, grouping->expr));
}

auto
ExprVisitor::operator()(VarExprPtr const& /*unused*/) -> std::string
{
  return "need environment!";
}

auto
ExprVisitor::operator()(AssignablePtr const& /*unused*/) -> std::string
{
  return "need environment!";
}

auto ExprVisitor::
operator()(auto /*unused*/) -> std::string
{
  throw std::runtime_error("unkown Expr type");
}
} // namespace beacon_lox