#pragma once
#define EFANO_FILTER_TEST_HPP

#include <efano/import/int_types.hpp>

#include <batteries/seq.hpp>

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
    double target_bit_rate;
    double actual_bit_rate;
    double error_rate;
};

template <typename FilterT, typename RNG>
void run_filter_test(RNG& rng, std::ostream& out)
{
    std::vector<u64> keys = generate_keys(rng, 1000000);

    std::vector<Result> results;

    for (usize n : {5, 10, 15, 20, 25, 30, 40, 50, 60, 70, 80, 100, 125, 150, 175, 200}) {
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

            results.emplace_back(Result{
                .n = (double)n,
                .target_bit_rate = bits_per_key,
                .actual_bit_rate = filter.actual_bits_per_key(),
                .error_rate = false_count / query_count,
            });
        }
    }

    std::sort(results.begin(), results.end(), [](const Result& l, const Result& r) {
        return l.target_bit_rate < r.target_bit_rate || (l.target_bit_rate == r.target_bit_rate && l.n < r.n);
    });

    double prev_n = -1;
    for (Result& r : results) {
        if (r.n < prev_n) {
            break;
        }
        if (r.n != prev_n) {
            out << "bits,fpr(N=" << r.n << "),";
            prev_n = r.n;
        }
    }
    out << std::endl;

    std::set<i64> actual_bit_rates;
    prev_n = -1;
    for (const Result& r : results) {
        if (r.n < prev_n) {
            out << std::endl;
        }
        prev_n = r.n;
        out << r.actual_bit_rate << "," << r.error_rate << ",";
        actual_bit_rates.emplace(r.actual_bit_rate * 1024.0);
    }
    out << std::endl;

    out << std::endl << "bits,expected(Bloom)" << std::endl;
    const double base = std::pow(2.0, (-1.0 / std::log2(std::exp(1))));
    for (i64 rate_1024 : actual_bit_rates) {
        double rate = (double)rate_1024 / 1024.0;
        out << rate << "," << std::pow(base, rate) << std::endl;
    }
}

}  // namespace efano
