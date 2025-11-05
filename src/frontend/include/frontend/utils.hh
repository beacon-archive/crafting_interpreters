#pragma once

namespace beacon_lox
{
class Uncopyabble
{
public:
  Uncopyabble(const Uncopyabble &) = delete;
  Uncopyabble &
  operator=(const Uncopyabble &) = delete;

protected:
  Uncopyabble() = default;
  ~Uncopyabble() = default;
};
} // namespace beacon_lox