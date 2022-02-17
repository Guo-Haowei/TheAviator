#pragma once
#include <cstdint>

template<typename T>
constexpr T AlignUp( T x, uint64_t a )
{
    return static_cast<T>( ( x + a - 1 ) & ( ~( a - 1 ) ) );
}

template<typename T, size_t SIZE>
constexpr size_t ArrayLength( T ( &arr )[SIZE] )
{
    return SIZE;
}
