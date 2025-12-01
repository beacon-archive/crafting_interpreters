#pragma once

#include "frontend/ast.hh"

namespace beacon_lox
{
class ExprVisitor
{
public:
  auto
  operator()(LiteralExprPtr const& literal) -> std::string;
  auto
  operator()(UnaryExprPtr const& unary) -> std::string;
  auto
  operator()(BinaryExprPtr const& binary) -> std::string;
  auto
  operator()(GroupingExprPtr const& grouping) -> std::string;
  auto
  operator()(VarExprPtr const& /*unused*/) -> std::string;
  auto
  operator()(AssignablePtr const& /*unused*/) -> std::string;
  auto
  operator()(auto /*unused*/) -> std::string;
};


} // namespace beacon_lox