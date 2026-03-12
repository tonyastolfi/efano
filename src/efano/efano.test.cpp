#include <efano/efano.hpp>

// Double-include the header to test include guards.
//
#include <efano/efano.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

TEST(EfanoTest, Test)
{
    EXPECT_TRUE(efano::entry_point());
}

}  // namespace
