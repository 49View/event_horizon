//
// Created by dado on 06/06/2020.
//

#pragma once

class FrameInvalidator {
public:
    void invalidate();
    bool invalidated() const;
    void validated();

private:
    bool bInvalidated = false;
};

