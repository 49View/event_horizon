//
// Created by dado on 06/06/2020.
//

#pragma once

template<typename intType, typename utype>
utype getCircularArrayIndex( intType pi, utype size ) {
    if ( std::llabs(pi) == size ) return 0;
    return pi >= 0 ? ( pi % size ) : size - static_cast<intType>( std::llabs(pi % size) );
}

template<typename intType, typename utype>
intType getCircularArrayIndexUnsigned( intType pi, utype size ) {
    if ( pi == size ) return 0;
    return pi >= 0 ? ( pi % size ) : size - ( pi % size );
}

// This the type traits unified version of getCircularArrayIndex*
// Plese use it all the time as it's much cleaner and less verbose
template<typename intType, typename utype>
utype cai( intType pi, utype size ) {
    if constexpr ( std::is_unsigned_v<intType> ) {
        if ( pi == static_cast<intType>(size) ) return 0;
        return pi >= 0 ? ( pi % size ) : size - ( pi % size );
    } else {
        if ( std::llabs(pi) == size ) return 0;
        return pi >= 0 ? ( pi % size ) : size - static_cast<intType>( std::llabs(pi) % size  );
    }
}

template<typename T>
T getLeftVectorFromList( const std::vector<T>& va, int64_t m, bool wrapIt ) {
    T ret;

    if ( m > 0 ) return va[m - 1];

    if ( wrapIt ) {
        int64_t index = m == 0 ? va.size() - 1 : m - 1;
        return va[index];
    }
    // do not wrap it so it needs to extend the vector as it there was another on one the same line opposite the first point (index 1)
    T ext = va[0] - va[1];
    return va[0] + ext;
}

template<typename T>
T getRightVectorFromList( const std::vector<T>& va, int64_t m, bool wrapIt ) {
    T ret;

    if ( m < static_cast<int64_t>( va.size() - 1 ) ) return va[m + 1];

    if ( wrapIt ) {
        int64_t index = m == static_cast<int64_t>( va.size() ) - 1 ? 0 : m + 1;
        return va[index];
    }
    // do not wrap it so it needs to extend the vector as it there was another on one the same line continuing from the last point
    T ext = va[va.size() - 1] - va[va.size() - 2];
    return va[va.size() - 1] + ext;
}
