//
// Created by Dado on 2019-03-12.
//

#pragma once

#include <cstdint>
#include <cstddef>

template <size_t K=0>
class Versionable {
public:
    inline static constexpr uint64_t Version() {
        return K;
    }
};
