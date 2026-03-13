#include <efano/bloom.hpp>
//
#include <efano/bloom.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <efano/filter.test.hpp>

#include <batteries/int_types.hpp>

#include <random>

namespace {

using namespace batt::int_types;

TEST(BloomFilterTest, Test)
{
    std::default_random_engine rng{1};
    efano::run_filter_test<efano::BloomFilter<u8>>(rng, std::cout);
}

}  // namespace
