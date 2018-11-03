// matrix.h - Square matrices for coordinate space transforms.
// Written by quadfault
// 9/14/18

#pragma once

#include <cstddef>

template <typename T, std::size_t N>
class matrix {
public:
    matrix() = default;

private:
    T values_[N];
};
