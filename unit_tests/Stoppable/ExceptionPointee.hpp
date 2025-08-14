#ifndef __STOPPABLE_UNIT_TEST_EXCEPTION_POINTEE_MATRCHER_HPP
#define __STOPPABLE_UNIT_TEST_EXCEPTION_POINTEE_MATRCHER_HPP

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdexcept>

namespace Stoppable::tests {
// NOLINTNEXTLINE(readability-identifier-naming)
MATCHER_P(ExceptionPointee, exception_type, "") {
  try {
    std::rethrow_exception(arg);
    return false;
  } catch (const std::exception& ex) {
    return typeid(ex) == typeid(exception_type);
  }
}
} // namespace Stoppable::tests
#endif //__STOPPABLE_UNIT_TEST_EXCEPTION_POINTEE_MATRCHER_HPP