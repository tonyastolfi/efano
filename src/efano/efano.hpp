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

    /** \brief Whether the `items_` vector is currently sorted (we sort lazily).
     */
    bool sorted_ = false;

    /** \brief The number of keys inserted into the filter; this may be greater than
     * `this->items_.size()` since hash/fingerprint collisions may occur during insertion.
     */
    usize inserted_ = 0;

    /** \brief The (unpacked) item fingerprints.
     */
    std::vector<u64> items_;

    /** \brief A low-order bit mask; this represents the size of a fingerprint.
     */
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
        // The number of bits implicitly stored via key sort order.
        //
        const i64 implicit_bits = batt::log2_ceil(this->items_.size());

        // The number of bits stored per key, explicitly and implicitly.
        //
        const i64 stored_bits = batt::bit_count(this->mask_);

        // The number of low-order remainder bits explicitly stored per key.
        //
        const i64 explicit_bits = std::max<i64>(0, stored_bits - implicit_bits);

        // The total packed size (bits) per fingerprint; we need 2 bits to store a unary
        // array of how many fingerprints there are per fingerprint high-order bits value.
        //
        const i64 packed_bits_per_item = 2 + explicit_bits;

        // The total number of bits this filter requires.
        //
        const i64 packed_bits = packed_bits_per_item * this->items_.size();

        // Packed byte size.
        //
        const i64 packed_bytes = (packed_bits + 7) / 8;

        if (this->items_.size() < this->inserted_) {
            std::cerr << "insert collisions detected: " << (this->inserted_ - this->items_.size())
                      << std::endl;
        }

        // Actual bit rate.
        //
        return 8.0 * (double)packed_bytes / (double)this->inserted_;
    }

    void clear()
    {
        this->sorted_ = true;
        this->items_.clear();
        this->inserted_ = 0;
    }

    void sort_if_needed()
    {
        if (!this->sorted_) {
            std::sort(this->items_.begin(), this->items_.end());
            this->items_.erase(std::unique(this->items_.begin(), this->items_.end()), this->items_.end());
            this->sorted_ = true;
        }
    }

    u64 get_hash(const void* key_data, usize key_size) const
    {
        return XXH3_64bits(key_data, key_size) & this->mask_;
    }

    void insert(const void* key_data, usize key_size)
    {
        ++this->inserted_;
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
