#pragma once
#define EFANO_BLOOM_HPP

#include <efano/import/bit_ops.hpp>
#include <efano/import/int_types.hpp>

#include <batteries/assert.hpp>

#include <xxh3.h>
#include <xxhash.h>

#include <cmath>
#include <random>
#include <vector>

namespace efano {

template <typename WordT>
struct BloomFilter {
    static constexpr usize kBitsPerWord = sizeof(WordT) * 8;

    //+++++++++++-+-+--+----- --- -- -  -  -   -

    usize key_count = 0;
    std::vector<u64> hash_seeds;
    std::vector<WordT> words;

    //+++++++++++-+-+--+----- --- -- -  -  -   -

    template <typename T, typename VisitorFn = bool(WordT& word, WordT mask)>
    static bool visit(T* filter, const void* key_data, usize key_size, VisitorFn&& visitor_fn)
    {
        for (const u64 hash_seed : filter->hash_seeds) {
            const u64 hash_val = XXH3_64bits_withSeed(key_data, key_size, hash_seed);
            const usize word_i = (hash_val / kBitsPerWord) % filter->words.size();
            const usize bit_i = hash_val % kBitsPerWord;

            if (!visitor_fn(filter->words[word_i], (WordT{1} << bit_i))) {
                return false;
            }
        }
        return true;
    }

    //+++++++++++-+-+--+----- --- -- -  -  -   -

    explicit BloomFilter(usize n, double bits_per_key) noexcept
    {
        const usize n_hashes = std::ceil(bits_per_key * std::log(2.0));

        this->hash_seeds.resize(n_hashes);

        std::random_device dev_rand;
        std::uniform_int_distribution<u64> pick_seed{u64{0}, ~u64{0}};
        for (u64& seed : this->hash_seeds) {
            seed = pick_seed(dev_rand);
        }

        this->words.resize(std::ceil((double)n * bits_per_key / (double)kBitsPerWord));
    }

    const char* dump() const
    {
        return "TODO [tastolfi 2026-03-13] ";
    }

    double actual_bits_per_key() const
    {
        return (double)kBitsPerWord * (double)this->words.size() / (double)this->key_count;
    }

    void clear()
    {
        this->key_count = 0;
        for (WordT& w : this->words) {
            w = 0;
        }
    }

    void insert(const void* key_data, usize key_size)
    {
        ++this->key_count;
        BloomFilter::visit(this, key_data, key_size, [](WordT& word, WordT mask) {
            word |= mask;
            return true;
        });
    }

    bool contains(const void* key_data, usize key_size) const
    {
        return BloomFilter::visit(this, key_data, key_size, [](const WordT& word, WordT mask) {
            return (word & mask) == mask;
        });
    }
};

}  // namespace efano
