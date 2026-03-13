// NOTE: not used currently; this was built for compact packing of e-f filters;
//  (we might also look into https://github.com/tlk00/BitMagic, or even
//   https://www.boost.org/doc/libs/latest/libs/dynamic_bitset/doc/html/dynamic_bitset/reference/boost/dynamic_bitset.html)
//
#pragma once
#define EFANO_BIT_PACKED_HPP

#include <efano/import/int_types.hpp>
#include <efano/import/slice.hpp>

namespace efano {

template <typename T>
static constexpr T mask(i32 first, i32 last)
{
    return ((T{1} << (last - first)) - 1) << first;
}

template <typename Src, typename Dst>
inline i64 copy_bits(Slice<const Src> src, i64 first_src_bit_i, i64 last_src_bit_i,  //
                     Slice<Dst> dst, i64 first_dst_bit_i)
{
    //----- --- -- -  -  -   -
    const auto skip_front = [](i64& first_bit, i64& last_bit, i64 bit_size, auto& slice) {
        const i64 skip = first_bit / bit_size;
        const i64 skip_bits = skip * bit_size;

        slice.drop_front(skip);
        first_bit -= skip_bits;
        last_bit -= skip_bits;
    };
    const auto advance_position = [](i64 n_to_copy, i64& first_bit, i64& bits_avail, i64 bit_size,
                                     auto& slice) {
        first_bit += n_to_copy;
        bits_avail -= n_to_copy;
        if (bits_avail == 0) {
            slice.drop_front();
            if (slice.empty()) {
                return false;
            }
            bits_avail = bit_size;
            first_bit = 0;
        }
        return true;
    };
    //----- --- -- -  -  -   -
    static constexpr i64 kSrcBitSize = sizeof(Src) * 8;
    static constexpr i64 kDstBitSize = sizeof(Dst) * 8;
    //----- --- -- -  -  -   -

    i64 n_copied = 0;
    i64 n_remaining = last_src_bit_i - first_src_bit_i;
    i64 last_dst_bit_i = first_dst_bit_i + n_remaining;

    skip_front(first_src_bit_i, last_src_bit_i, kSrcBitSize, src);
    skip_front(first_dst_bit_i, last_dst_bit_i, kDstBitSize, dst);

    i64 src_bits_avail = kSrcBitSize - first_src_bit_i;
    i64 dst_bits_avail = kDstBitSize - first_dst_bit_i;

    while (n_remaining > 0) {
        const i64 n_to_copy = std::min(n_remaining, std::min(src_bits_avail, dst_bits_avail));

        const Src src_mask = mask<Src>(first_src_bit_i, first_src_bit_i + n_to_copy);
        const Dst dst_mask = mask<Dst>(first_dst_bit_i, first_dst_bit_i + n_to_copy);

        dst.front() = (dst.front() & ~dst_mask) |  //
                      (Dst{(src.front() & src_mask) >> first_src_bit_i} << first_dst_bit_i);

        n_remaining -= n_to_copy;
        n_copied += n_to_copy;

        if (!advance_position(n_to_copy, first_src_bit_i, src_bits_avail, kSrcBitSize, src)) {
            break;
        }
        if (!advance_position(n_to_copy, first_dst_bit_i, dst_bits_avail, kDstBitSize, dst)) {
            break;
        }
    }

    return n_copied;
}

static_assert(mask<u8>(0, 5) == 0b11111);
static_assert(mask<u8>(2, 5) == 0b11100);
static_assert(mask<u32>(17, 41) == 0xfffe0000);

}  // namespace efano
