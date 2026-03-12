#pragma once
#define EFANO_ELIAS_FANO_FILTER_HPP

namespace efano {

struct EliasFanoFilter {
    template <typename IterT>
    explicit EliasFanoFilter(double bits_per_key, IterT first, IterT last) noexcept
    {
    }
};

}  // namespace efano
