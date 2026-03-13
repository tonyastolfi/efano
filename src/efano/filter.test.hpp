#pragma once
#define EFANO_FILTER_TEST_HPP

#include <efano/import/int_types.hpp>

#include <random>
#include <unordered_set>
#include <vector>

namespace efano {

template <typename RNG>
std::vector<u64> generate_keys(RNG& rng, usize n)
{
    std::uniform_int_distribution<u64> pick_key{0, ~u64{0}};

    std::vector<u64> result(n);

    for (usize i = 0; i < n; ++i) {
        result.emplace_back(pick_key(rng));
    }

    return result;
}

struct Result {
    double n;
    std::vector<double> bits_per_key;
    std::vector<double> false_rate;
};

template <typename FilterT, typename RNG>
void run_filter_test(RNG& rng, std::ostream& out)
{
    std::vector<u64> keys = generate_keys(rng, 1000000);

    std::vector<Result> results;
    usize h = 0;

    for (usize n : {5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 100, 125, 150, 175, 200}) {
        Result& r = results.emplace_back();
        r.n = n;

        std::unordered_set<u64> filter_keys;
        for (const u64& key : keys) {
            filter_keys.emplace(key);
            if (filter_keys.size() >= n) {
                break;
            }
        }
        BATT_CHECK_EQ(n, filter_keys.size());

        for (double bits_per_key : {2.0, 4.0, 8.0, 12.0, 16.0, 24.0, 32.0, 40.0, 48.0}) {
            FilterT filter{n, bits_per_key};

            for (const u64& key : filter_keys) {
                filter.insert(&key, sizeof(key));
            }

            for (const u64& key : filter_keys) {
                ASSERT_TRUE(filter.contains(&key, sizeof(key))) << BATT_INSPECT(key) << filter.dump();
            }

            double query_count = 0;
            double false_count = 0;

            for (const u64& key : keys) {
                ++query_count;
                if (filter_keys.count(key)) {
                    ASSERT_TRUE(filter.contains(&key, sizeof(key)));
                } else {
                    if (filter.contains(&key, sizeof(key))) {
                        ++false_count;
                    }
                }
            }

            r.bits_per_key.emplace_back(filter.actual_bits_per_key());
            r.false_rate.emplace_back(false_count / query_count);
        }

        h = std::max(h, r.bits_per_key.size());

        BATT_CHECK_EQ(r.bits_per_key.size(), r.false_rate.size());
    }

    out << "bits";
    for (const Result& r : results) {
        out << ",fpr(N=" << r.n << ")";
    }
    out << std::endl;

    for (usize i = 0; i < h; ++i) {
        for (const Result& r : results) {
            if (&r == &(results.front())) {
                out << r.bits_per_key[i];
            }
            out << "," << r.false_rate[i];
        }
        out << std::endl;
    }
}

}  // namespace efano
