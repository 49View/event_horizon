//
// Created by Dado on 2019-04-08.
//

#pragma once

#include <core/uuid.hpp>

class UUIDCopiable {
public:
    UUIDCopiable( const UUID& _m ) {
        mUUID = _m;
    }

    const UUID& UUiD() const {
        return mUUID;
    }

    UUID UUiDCopy() const {
        return mUUID;
    }

private:
    UUID mUUID;
};