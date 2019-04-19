//
// Created by Dado on 2019-01-13.
//

#pragma once

#include <unordered_map>
#include <core/uuid.hpp>
#include <core/node.hpp>
#include <core/math/vector4f.h>
#include <core/resources/resource_utils.hpp>
#include <poly/poly.hpp>
#include <poly/scene_graph.h>

class Camera;
class SceneOrchestrator;

namespace SelectableFlag {
    static const uint64_t None = 0;
    static const uint64_t Selected = 1 << 0;
    static const uint64_t Highlighted = 1 << 1;
};

using SelectableFlagT = uint64_t;

struct Selectable {
    Selectable( const Color4f& oldColor, MatrixAnim& localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) :
        oldColor( oldColor ),
        trs( localTransform ),
        node( _node ),
        flags( _flags ) {}

    Color4f oldColor;
    MatrixAnim& trs;
    NodeVariantsSP node;
    SelectableFlagT flags = SelectableFlag::None;
};

namespace SelectionTraverseFlag {
    static const uint64_t None = 0;
    static const uint64_t Recursive = 1 << 0;
};

using SelectionTraverseFlagT = uint64_t;

template <typename Tint, typename T>
void xandBitWiseFlag( Tint& source, T flag ) {
    int dest = source ^ flag;
    source = static_cast<Tint>(dest);
}

class Selection {
public:
    virtual void selected( const UUID& _uuid, MatrixAnim& _trs, NodeVariantsSP _node, SelectableFlagT _flags ) = 0;
    void unselect( const UUID& _uuid, Selectable& _node );
    void unselectAll();

    template <typename T>
    void selected( T _geom, SelectableFlagT _flags = SelectableFlag::Selected|SelectableFlag::Highlighted ) {
//        ### REF selected needs to be worked out with new assets graphs (UUID)
//        selected( _geom->UUiD(), _geom->TRS(), _geom, _flags );
//        for ( auto& c : _geom->Children() ) {
//            SelectableFlagT recFlags = _flags;
//            if ( !checkBitWiseFlag(traverseFlag, SelectionTraverseFlag::Recursive) ) {
//                xandBitWiseFlag(recFlags, SelectableFlag::Selected);
//            }
//            selected( c, recFlags );
//        }
    }

    void showGizmo(Selectable& _node, std::shared_ptr<Camera> _cam );

    bool IsSelected() const;
    void IsSelected( bool bIsSelected );
    bool IsOver() const;
    void IsOver( bool bIsOver );
    bool IsAlreadyInUse() const;
    bool isImGuiBusy() const;

    bool inputIsBlockedOnSelection() const;

protected:
    virtual void unselectImpl( const UUID& _uuid, Selectable& _node ) = 0;

protected:
    std::unordered_map<UUID, Selectable> selectedNodes;
    bool bIsSelected = false;
    bool bIsOver = false;
    SelectionTraverseFlagT traverseFlag = SelectionTraverseFlag::None;
};

struct SelectionRecursiveLamba {
    explicit SelectionRecursiveLamba( Selection& sel ) : mSel( sel ) {}

    void operator()(GeomSP arg) const {
        mSel.selected( arg );
    }

private:
    Selection& mSel;
};

struct SelectionAddToKeyFrame {
    SelectionAddToKeyFrame( const std::string& timelineName, float time ) : timelineName( timelineName ), time( time ) {}

    void operator()(GeomSP arg) const;
//    void operator()(UIAssetSP arg) const {
//        arg->addKeyFrame( timelineName, time );
//    }

private:
    std::string timelineName;
    float time;
};
