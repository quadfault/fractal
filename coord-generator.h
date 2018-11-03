// coord-generator.h - Concurrent pixel coordinate generator.
// Written by quadfault
// 9/15/18

#pragma once

#include <atomic>
#include <cstddef>
#include <optional>
#include <utility>

class coord_generator {
public:
    coord_generator(std::size_t width, std::size_t height):
        width_(width), max_index_(width * height - 1), index_(0)
    {}

    std::optional<std::pair<std::size_t, std::size_t>> next() {
        auto next_index = index_.fetch_add(1, std::memory_order_seq_cst);
        if (next_index > max_index_) {
            return std::nullopt;
        }

        auto x = next_index % width_;
        auto y = next_index / width_;

        return {{ x, y }};
    }

private:
    const std::size_t width_;
    const std::size_t max_index_;
    std::atomic<std::size_t> index_;
};
