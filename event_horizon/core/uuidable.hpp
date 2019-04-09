//
// Created by Dado on 2019-04-08.
//

#pragma once

#include <core/uuid.hpp>

class UUIDable {
public:
    UUIDable() {
        mUUID = UUIDGen::make();
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