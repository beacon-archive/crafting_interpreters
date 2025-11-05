#include "frontend/error.hh"

namespace beacon_lox
{
// Global error instance that needs to be mutable to track runtime errors
// throughout the interpreter's execution. While globals are generally
// discouraged, this is an intentional design choice for error handling.
Error g_error;
} // namespace beacon_lox