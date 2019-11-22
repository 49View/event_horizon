//
// Created by Dado on 2019-04-08.
//

#pragma once

#include <core/uuid.hpp>

class UUIDable {
public:
    UUIDable() {
        makeUUID();
    }

    const UUID& UUiD() const {
        return mUUID;
    }

    UUID UUiDCopy() const {
        return mUUID;
    }

    void assingNewUUID() {
        makeUUID();
    }

private:
    void makeUUID() {
        mUUID = UUIDGen::make();
    }
private:
    UUID mUUID;
};