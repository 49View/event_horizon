//
// Created by Dado on 2019-03-13.
//

#pragma once

#include <core/htypes_shared.hpp>

template <typename K>
class QueryPolicy {
public:
    inline static constexpr bool usesNotExactQuery() {
        return !std::is_same<K, BuilderQueryType::Exact>::value;
    }
};
