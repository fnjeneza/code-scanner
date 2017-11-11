#pragma once

namespace code::analyzer {
// Called repeatedly to incrementally create a hash value from several
// variables
// from Boost
template <typename T>
void hash_combine(std::size_t &seed, const T &v)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace code::analyzer
