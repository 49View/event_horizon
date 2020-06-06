//
// Created by dado on 06/06/2020.
//

#include "frame_invalidator.hpp"

void FrameInvalidator::invalidate() {
    bInvalidated = true;
}

bool FrameInvalidator::invalidated() const {
    return bInvalidated;
}

void FrameInvalidator::validated() {
    bInvalidated = false;
}
