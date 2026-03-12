#include <efano/bloom.hpp>
//
#include <efano/bloom.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <batteries/int_types.hpp>

#include <random>
#include <unordered_set>

namespace {

using namespace batt::int_types;

using efano::BloomFilter;

template <typename RNG>
std::unordered_set<u64> generate_keys(RNG& rng, usize n)
{
    std::uniform_int_distribution<u64> pick_key{0, ~u64{0}};

    std::unordered_set<u64> result(n);

    for (usize i = 0; i < n; ++i) {
        result.emplace(pick_key(rng));
    }

    return result;
}

struct Result {
    double n;
    std::vector<double> bits_per_key;
    std::vector<double> false_rate;
};

TEST(BloomFilterTest, Test)
{
    std::default_random_engine rng{1};

    auto verify_keys = generate_keys(rng, 1000000);

    std::vector<Result> results;
    usize h = 0;

    for (usize n : {5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 100, 125, 150, 175, 200}) {
        auto set_keys = generate_keys(rng, n);

        Result& r = results.emplace_back();
        r.n = n;

        for (double bits_per_key : {2.0, 4.0, 8.0, 12.0, 16.0, 24.0, 32.0, 40.0, 48.0}) {
            BloomFilter<u8> filter{n, bits_per_key};

            for (const u64& key : set_keys) {
                filter.insert(&key, sizeof(key));
            }

            for (const u64& key : set_keys) {
                ASSERT_TRUE(filter.contains(&key, sizeof(key)));
            }

            double query_count = 0;
            double false_count = 0;

            for (const u64& key : verify_keys) {
                ++query_count;
                if (set_keys.count(key)) {
                    ASSERT_TRUE(filter.contains(&key, sizeof(key)));
                } else {
                    if (filter.contains(&key, sizeof(key))) {
                        ++false_count;
                    }
                }
            }

            r.bits_per_key.emplace_back(bits_per_key);
            r.false_rate.emplace_back(false_count / query_count);
        }

        h = std::max(h, r.bits_per_key.size());

        BATT_CHECK_EQ(r.bits_per_key.size(), r.false_rate.size());
    }

    std::cout << "bits";
    for (const Result& r : results) {
        std::cout << ",fpr(" << r.n << ")";
    }
    std::cout << std::endl;

    for (usize i = 0; i < h; ++i) {
        for (const Result& r : results) {
            if (&r == &(results.front())) {
                std::cout << r.bits_per_key[i] << "," << r.false_rate[i];
            } else {
                std::cout << "," << r.false_rate[i];
            }
        }
        std::cout << std::endl;
    }
}

}  // namespace
