#pragma once

namespace beacon_lox
{
class Uncopyabble
{
public:
  Uncopyabble(Uncopyabble const&) = delete;
  Uncopyabble&
  operator=(Uncopyabble const&) = delete;

protected:
  Uncopyabble() = default;
  ~Uncopyabble() = default;
};
} // namespace beacon_lox