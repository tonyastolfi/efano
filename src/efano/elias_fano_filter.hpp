#pragma once
#define EFANO_ELIAS_FANO_FILTER_HPP

#include <efano/import/int_types.hpp>

#include <batteries/assert.hpp>
#include <batteries/bit_ops/bit_count.hpp>
#include <batteries/math.hpp>
#include <batteries/stream_util.hpp>

#include <xxh3.h>
#include <xxhash.h>

#include <algorithm>
#include <bitset>
#include <cmath>
#include <vector>

namespace efano {

struct EliasFanoFilter {
    using Self = EliasFanoFilter;

    //+++++++++++-+-+--+----- --- -- -  -  -   -

    bool sorted_ = false;
    std::vector<u64> items_;
    u64 mask_;

    //+++++++++++-+-+--+----- --- -- -  -  -   -

    explicit EliasFanoFilter(usize n, double bits_per_key) noexcept
    {
        const i32 implicit_bits = batt::log2_ceil(n);
        const i32 explicit_bits = std::ceil(bits_per_key) - 2;
        const i32 stored_bits = implicit_bits + explicit_bits;

        this->mask_ = (u64{1} << stored_bits) - 1;
    }

    auto dump()
    {
        return [this](std::ostream& out) {
            this->sort_if_needed();
            out << batt::dump_range(this->items_) << BATT_INSPECT(std::bitset<64>{this->mask_})
                << BATT_INSPECT(this->sorted_);
        };
    }

    double actual_bits_per_key() const
    {
        const i32 implicit_bits = batt::log2_ceil(this->items_.size());
        const i32 stored_bits = batt::bit_count(this->mask_);
        const i32 explicit_bits = std::max(0, stored_bits - implicit_bits);

        return 2.0 + (double)explicit_bits;
    }

    void sort_if_needed()
    {
        if (!this->sorted_) {
            std::sort(this->items_.begin(), this->items_.end());
            this->items_.erase(std::unique(this->items_.begin(), this->items_.end()), this->items_.end());
            this->sorted_ = true;
            std::cerr << "sorted! " << BATT_INSPECT(this->dump()) << std::endl;
        }
    }

    u64 get_hash(const void* key_data, usize key_size) const
    {
        return XXH3_64bits(key_data, key_size) & this->mask_;
    }

    void insert(const void* key_data, usize key_size)
    {
        const u64 hash_val = this->get_hash(key_data, key_size);
        this->sorted_ = this->items_.empty() || (this->sorted_ && (hash_val > this->items_.back()));
        this->items_.emplace_back(hash_val);
    }

    bool contains(const void* key_data, usize key_size) const
    {
        BATT_CHECK(this->sorted_);
        const u64 hash_val = this->get_hash(key_data, key_size);
        auto iter = std::lower_bound(this->items_.begin(), this->items_.end(), hash_val);
        return iter != this->items_.end() && *iter == hash_val;
    }

    bool contains(const void* key_data, usize key_size)
    {
        this->sort_if_needed();
        return const_cast<const Self*>(this)->contains(key_data, key_size);
    }
};

}  // namespace efano
