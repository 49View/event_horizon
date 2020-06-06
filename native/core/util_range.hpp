//
// Created by dado on 06/06/2020.
//

#pragma once

template<typename T>
bool inRange( const T _key, const std::pair<T, T>& _range ) {
    return _range.first <= _key && _range.second >= _key;
}

template<typename T>
bool inRangeEx( const T _key, const std::pair<T, T>& _range ) {
    return _range.first <= _key && _range.second > _key;
}

template<typename T>
bool inRange( const T _key, const T _r1, const T _r2 ) {
    return _r1 <= _key && _r2 >= _key;
}

template<typename T>
bool inRangeEx( const T _key, const T _r1, const T _r2 ) {
    return _r1 <= _key && _r2 > _key;
}
