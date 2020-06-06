//
// Created by dado on 06/06/2020.
//

#pragma once

class UpdateSignals {
    bool mbNeedsUpdate = false;
public:
    bool NeedsUpdate() const {
        return mbNeedsUpdate;
    }

    void NeedsUpdate( bool _mbNeedsUpdate ) {
        mbNeedsUpdate = _mbNeedsUpdate;
    }
};
