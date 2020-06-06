//
// Created by dado on 06/06/2020.
//

#include "incremental_hash.hpp"

HashEH HashInc() {
    static HashEH globalHash = 1;
    return ++globalHash;
}
