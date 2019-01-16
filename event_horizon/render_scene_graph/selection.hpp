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
    Selectable( const Color4f& oldColor, MatrixAnim& localTransform ) : oldColor( oldColor ),
                                                                        trs( localTransform ) {}

    Color4f oldColor;
    MatrixAnim& trs;
};

class Selection {
public:
    virtual void selected( const UUID& _uuid, MatrixAnim& _trs ) = 0;
    virtual Matrix4f getViewMatrix() = 0;
    virtual Matrix4f getProjMatrix() = 0;

    template <typename T>
    void selected( T _geom ) {
        selected( _geom->Hash(), _geom->TRS() );
        for ( auto& c : _geom->Children() ) {
            selected( c );
        }
    }

    void showGizmo( MatrixAnim& _localTransform ) {
        showTransform( _localTransform, getViewMatrix(), getProjMatrix() );
    }

protected:
    void showTransform( MatrixAnim& _mat, const Matrix4f& _view, const Matrix4f& _proj );

protected:
    std::unordered_map<UUID, Selectable> selectedNodes;
};

struct SelectionRecursiveLamba {
    explicit SelectionRecursiveLamba( Selection& sel ) : mSel( sel ) {}

    void operator()(GeomAssetSP arg) const {
        mSel.selected( arg );
    }
    void operator()(UIAssetSP arg) const {
        mSel.selected( arg );
    }

private:
    Selection& mSel;
};
