#pragma once

#include "core/util.h"

#include <memory>
#include <vector>
#include <stack>
#include <unordered_map>
#include "core/group_container.h"
#include "core/math/vector2f.h"
#include "core/math/vector4f.h"
#include "core/math/rect2f.h"
#include "ui_control.h"
#include "ui_render.h"
#include "../renderer.h"
#include "core/kvfmapping.h"
#include "ui_bsdata.hpp"
//#include "ray_tracing/vertex_buffer.h"

struct Layout;
struct LayoutElement;
struct LayoutArrangement;
struct StyleSheetElement;

class UiControlManager;

struct ControlGroupAlignData {
    Vector2f origin;
    Vector2f size;
};

using UiModelViewPair = std::pair<std::shared_ptr<UiControl>, std::unique_ptr<UiControlRenderer>>;

struct UiControlFactoryMakerI {
    virtual ~UiControlFactoryMakerI();
    virtual UiModelViewPair get( const UiControlBuilder& _data, Renderer& rr ) = 0;
};

template<typename V, typename R>
struct UiControlFactoryMaker : public UiControlFactoryMakerI {
    virtual UiModelViewPair get( const UiControlBuilder& _data, Renderer& rr ) override {
        auto view = std::make_shared<V>( rr.FM() );
        view->init( _data );
//        auto render = std::make_unique<R>( rr, view, generateVPList( rr, view.get()));
        auto render = std::make_unique<R>( rr, view );
        render->make();
        return { view, std::move( render ) };
    }
};

class UiControlFactory {
public:
    UiControlFactory();
    UiModelViewPair make( const UiControlBuilder& _data, Renderer& rr );

private:
    std::map<std::string, std::unique_ptr<UiControlFactoryMakerI>> controlNameMap;
};

class UiControlGroup : public GroupContainerMSV<UiModelViewPair> {
private:
    UiControlGroup( UiControlGroup const& ) = delete;
    void operator=( UiControlGroup const& ) = delete;
public:
    UiControlGroup( UiControlManager& _uicm,  const std::string& _uiViewName, int _flags, const KVFMapping& _mapping );

    void update( const float timeStep );
    Vector2f size() const;

    int Flags() const { return mFlags; }

    void addFlags( const int _flags );
    void removeFlags( const int _flags );

    ControlGroupAlignData
    pushAlignment( const UiControlPosition npos, const UiControlSizeType sizeType, Vector2f& size,
                   const int flags,
                   const UiAlignElement _margins, const UiAlignElement _paddings );

    void setVisible( bool visible );
    void setEnabled( bool enabled );
    void Selected( const bool val );

    bool isVisible() const {
        return mVisible;
    }

    void Padding( const Vector4f& val ) { mPadding = val; }

    void Margins( const Vector4f& val ) { mMargins = val; }

    Vector4f Padding() const { return mPadding.raw(); }

    Vector4f Margins() const { return mMargins.raw(); }

    Vector2f fullSize() const;
    Vector2f fullSizeWithMargins( const UiAlignElement& _margins, const UiAlignElement& _padding ) const;
    Vector2f fullSizeQuadWithMargins( const UiAlignElement& _margins, const UiAlignElement& _padding ) const;
    Vector2f fullWidthWithMargins( const float _heightPerc, const UiAlignElement& _margins,
                                   const UiAlignElement& _padding ) const;
    Vector2f remainingWidthWithMargins( const float _heightPerc, const UiAlignElement& _margins,
                                        const UiAlignElement& _padding ) const;

    bool IsScreenSpace() const { return mIsScreenSpace; }

    MatrixAnim AnimMatrix() const { return mAnimMatrix; }

    MatrixAnim& AnimMatrix() { return mAnimMatrix; }

    void AnimMatrix( const MatrixAnim& val );

    Vector3f UINormalAxis() const { return mUINormalAxis; }

    void UINormalAxis( const Vector3f& val );
    void OrigRot( const Matrix4f& val );
    //std::vector<std::shared_ptr<VertexBuffer>> createVertexBuffers();

    //std::vector<std::unique_ptr<UiControl>>& getLayoutControls( const std::string& _name, int _index = 0 );

    void changeTextOn( const std::string& _cname, const std::string& _newText );
    void changeTitleOn( const std::string& _cname, const std::string& _newText );

    int getNumbersOfLayoutControlsFor( const std::string& _name );

    bool isOnVR() const { return mbIsOnVR; }

    void isOnVR( const bool val );

    void StartZLevel( int val ) { mUIZ = val; }

    void pushAnchor( const Vector2f& _size, const UiAlignElement& _margins );
    void popAnchor( const std::string& _anchorPos, const UiAlignElement& _margins );

    Matrix4f PreRotation() const { return mPreRotation; }

    void PreRotation( Matrix4f val ) { mPreRotation = val; }

    int64_t Hash() const { return mHash; }

    void Hash( int64_t val ) { mHash = val; }

    std::string SchemaName() const { return mSchemaName; }

    void SchemaName( const std::string& val ) { mSchemaName = val; }

private:
    Vector2f getStartingOffsetFromRootPosition( const std::string& rootPos,
                                                const Vector2f& _customTRootPos );
    void overrideStyleIfNeeded( UiControlInitData& cid, const StyleSheetElement& ss ) const;
    int getNumberOfArrayLayoutsFromInjection( const Layout& l, const LayoutArrangement& _layoutArrangement );

    void IsScreenSpace( bool const val ) { mIsScreenSpace = val; }

    void advanceZ( const LayoutElement& _element, int ai );
    void pushOffset();
    void popOffset( int _index, int _numElements );

    void push_layout( const LayoutArrangement& _layoutArrangement );
private:

    int64_t mHash;
    std::string mSchemaName;
    int mFlags;
    Rect2f mRect;
    UiAlignElement mPadding;
    UiAlignElement mMargins;
    MatrixAnim mAnimMatrix;
    Vector3f mUINormalAxis;
    Matrix4f mOrigRot;
    Matrix4f mPreRotation;
    int mUIZ;

    bool mVisible;
    bool mIsScreenSpace;
    bool mbIsOnVR;

    Vector2f mStartPosition;
    Vector2f mOffsets;
    Vector2f mOffsetArrayInjectionStack;
    int mZArrayInjectionStack;
    std::stack<Vector4f> mAnchors;

    KVFMapping mMapping;

    UiControlFactory mFactory;
    UiControlManager& uicm;
};

//struct RBUILDER( UiViewBuilder, UIView, "", JSON, BuilderQueryType::Exact )
//
//};

class UiViewBuilder : public ResourceBuilder {
    using ResourceBuilder::ResourceBuilder;
public:
    virtual ~UiViewBuilder();

    const std::string prefix() const override { return ""; }
    bool build( DependencyMaker& _md ) override;
    static bool usesNotExactQuery() { return false; };
};
