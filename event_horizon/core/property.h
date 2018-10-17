#pragma once

template <typename T>
struct Property {
    T value;

    Property() {}

    Property( const T& _value ) {
        *this = _value;
    }

    operator T() {
        return value;
    }

    operator const T&() const {
        return value;
    }

    T operator=(const T& _value) {
        return value = _value;
    }
};