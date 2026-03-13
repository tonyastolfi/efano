#include <efano/elias_fano_filter.hpp>
//
#include <efano/elias_fano_filter.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <efano/filter.test.hpp>

#include <random>

namespace {

TEST(EliasFanoFilterTest, Test)
{
    std::default_random_engine rng{1};
    efano::run_filter_test<efano::EliasFanoFilter>(rng, std::cout);
}

}  // namespace
