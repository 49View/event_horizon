//
// Created by Dado on 2019-01-13.
//

#pragma once

#include <unordered_map>
#include <core/uuid.hpp>
#include <core/node.hpp>
#include <core/math/vector4f.h>
#include <poly/poly.hpp>

struct Selectable {
    Color4f oldColor;
};

class Selection {
public:
    virtual void selected( const UUID& _uuid ) = 0;

    template <typename T>
    void selected( T _geom ) {
        selected( _geom->Hash() );
        for ( auto& c : _geom->Children() ) {
            selected( c );
        }
    }

protected:
    std::unordered_map<UUID, Selectable> selectedNodes;
};

struct SelectionRecursiveLamba {
    SelectionRecursiveLamba( Selection& sel ) : mSel( sel ) {}

    void operator()(GeomAssetSP arg) const {
        mSel.selected( arg );
    }
    void operator()(UIAssetSP arg) const {
        mSel.selected( arg );
    }

private:
    Selection& mSel;
};
