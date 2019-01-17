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

namespace SelectionTraverseFlag {
    static const uint64_t None = 0;
    static const uint64_t Recursive = 1 << 0;
};

using SelectionTraverseFlagT = uint64_t;

class Selection {
public:
    virtual void selected( const UUID& _uuid, MatrixAnim& _trs ) = 0;
    void unselectAll();

    template <typename T>
    void selected( T _geom ) {
        selected( _geom->Hash(), _geom->TRS() );
        if ( checkBitWiseFlag(traverseFlag, SelectionTraverseFlag::Recursive) ) {
            for ( auto& c : _geom->Children() ) {
                selected( c );
            }
        }
    }

    void showGizmo(MatrixAnim& _localTransform, const Matrix4f& _view, const Matrix4f& _proj, const Rect2f& _viewport);

    bool IsSelected() const;
    void IsSelected( bool bIsSelected );
    bool IsOver() const;
    void IsOver( bool bIsOver );
    bool IsAlreadyInUse() const;
    bool isImGuiBusy() const;

protected:
    virtual void unselect( const UUID& _uuid, const Selectable& _node ) = 0;

protected:
    std::unordered_map<UUID, Selectable> selectedNodes;
    bool bIsSelected = false;
    bool bIsOver = false;
    SelectionTraverseFlagT traverseFlag = SelectionTraverseFlag::None;
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
