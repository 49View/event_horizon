//
// Created by Dado on 06/01/2018.
//

#pragma once

#include <core/formatting_utils.h>
#include <core/observable.h>
#include <core/builders.hpp>
#include <core/math/rect2f.h>
#include <core/math/aabb.h>
#include <core/math/vector4f.h>
#include <core/soa_utils.h>

#include <poly/poly.hpp>
#include <poly/scene_graph.h>
#include <poly/scene_graph_geom_builder_base.hpp>

namespace Utility::TTFCore { class Font; }

class UIElement : public Publisher<UIElement> {
public:
    UIElement() = default;
    UIElement( const std::string& name, UIShapeType shapeType, std::shared_ptr<Material> _mat, int renderBucketIndex ) :
               shapeType( shapeType ), material( _mat ), renderBucketIndex( renderBucketIndex ) {
        Name( name );
    }

    virtual ~UIElement() = default;

    const std::shared_ptr<Material>& getMaterial() const {
        return material;
    }

    void setMaterial( const std::shared_ptr<Material>& _material ) {
        UIElement::material = _material;
    }

    UIShapeType ShapeType() const {
        return shapeType;
    }

    void ShapeType( UIShapeType _shapeType ) {
        UIElement::shapeType = _shapeType;
    }

    const std::shared_ptr<PosTex3dStrip>& VertexList() const {
        return vs;
    }

    std::shared_ptr<PosTex3dStrip>& VertexList() {
        return vs;
    }

    void VertexList( const std::shared_ptr<PosTex3dStrip>& vs ) {
        UIElement::vs = vs;
        bbox3d = vs->BBox3d();
    }

    int RenderBucketIndex() const {
        return renderBucketIndex;
    }

    void RenderBucketIndex( int renderBucketIndex ) {
        UIElement::renderBucketIndex = renderBucketIndex;
    }

    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( "Name,BBbox", this->Name(),bbox3d ); }

    void serializeDependenciesImpl( std::shared_ptr<SerializeBin> writer ) const override {}
    void serializeInternal( std::shared_ptr<SerializeBin> writer ) const override {}
    void deserializeInternal( std::shared_ptr<DeserializeBin> reader ) override {}

    std::string generateThumbnail() const override { return std::string{}; }

protected:
    std::string calcHashImpl() override {
//        -###- Fixme implement hash function
        return std::string();
    }

private:
    UIShapeType shapeType = UIShapeType::Rect2d;
    std::shared_ptr<Material> material;
    int renderBucketIndex = 0;
    std::shared_ptr<PosTex3dStrip> vs;

public:
    static uint64_t Version() { return 1000; }
    inline const static std::string EntityGroup() { return EntityGroup::UI; }

};

class UIShapeBuilder : public SceneGraphGeomBaseBuilder, public Observable<UIShapeBuilder> {
public:
    virtual ~UIShapeBuilder() = default;

    UIShapeBuilder( SceneGraph& _sg, UIShapeType shapeType );
    UIShapeBuilder( SceneGraph& _sg, UIShapeType _shapeType, const std::string& _ti, float _fh = 0.0f );

    void assemble() override;

    void init() {
        material->setShaderName( getShaderType( shapeType ) );
        defaultFontIfNecessary();
    }

    void defaultFontIfNecessary() {
        if ( shapeType == UIShapeType::Text2d || shapeType == UIShapeType::Text3d ) {
            fontName = defaultFontName;
        }
    }

    std::string getShaderType( UIShapeType _st ) const;

    bool Centred() const {
        return isCentred;
    }

    UIShapeBuilder& v( const std::initializer_list<Vector3f>& vlist ) {
        for ( const auto& v : vlist ) {
            outlineVerts.push_back(v);
        }
        return *this;
    }

    UIShapeBuilder& v( const std::vector<Vector3f>& vlist ) {
        outlineVerts = vlist;
        return *this;
    }

    UIShapeBuilder& p( const std::vector<Vector3f>& vlist ) {
        triPolyMesh.insert(triPolyMesh.end(), vlist.begin(), vlist.end());
        return *this;
    }

    UIShapeBuilder& lw( const float _lineWidth ) {
        lineWidth = _lineWidth;
        return *this;
    }

    UIShapeBuilder& blw( const bool _lineWrap ) {
        wrapLine = _lineWrap;
        return *this;
    }

    UIShapeBuilder& t( const std::string& _tname ) {
        tname = _tname;
        return *this;
    }

    UIShapeBuilder& r( const Rect2f& _rect ) {
        rect = _rect;
        return *this;
    }

    UIShapeBuilder& origin( const Vector2f& _orig ) {
        orig = _orig;
        return *this;
    }

    UIShapeBuilder& s( float _size ) {
        size = Vector2f{ _size, _size };
        return *this;
    }

    UIShapeBuilder& s( const Vector2f& _size ) {
        size = _size;
        return *this;
    }

    UIShapeBuilder& cb( const Vector4f& _color ) {
        backgroundColor = _color;
        return *this;
    }

    UIShapeBuilder& ZLevel( const int _zLevel ) {
        zLevel = UIZ.level( _zLevel );
        return *this;
    }

    UIShapeBuilder& Z( const float _z ) {
        zLevel = _z;
        return *this;
    }

    UIShapeBuilder& bv( const float _bv ) {
        bevelRadius = _bv;
        return *this;
    }

    UIShapeBuilder& fm( RectFillMode _fm ) {
        fillMode = _fm;
        return *this;
    }

    UIShapeBuilder& centred() {
        isCentred = true;
        return *this;
    }

    UIShapeBuilder& expandable() {
        isExpandable = true;
        return *this;
    }

    UIShapeBuilder& f( const std::string& _fn ) {
        fontName = getFileNameNoExt(_fn);
        return *this;
    }

    UIShapeBuilder& fh( const float _fh ) {
        fontHeight = _fh;
        return *this;
    }

    UIShapeBuilder& an( const RectCreateAnchor _anchor ) {
        anchor = _anchor;
        return *this;
    }

    UIShapeBuilder& rot(float _val) {
        mRot = _val;
        return *this;
    }

    UIShapeBuilder& arAngleLength(float _val) {
        arrowAngleLenght = _val;
        return *this;
    }

    UIShapeBuilder& arAngle(float _val) {
        arrowAngle = _val;
        return *this;
    }
    UIShapeBuilder& arLength(float _val) {
        arrowLength = _val;
        return *this;
    }
    UIShapeBuilder& arThickness(float _val) {
        arrowThickness = _val;
        return *this;
    }

    UIShapeBuilder& fx( const UIRenderFlags _rf ) {
        effects = _rf;
        return *this;
    }

    UIShapeBuilder& al( const UiAlignElement _al ) {
        alignElement = _al;
        return *this;
    }

    UIShapeBuilder& ti( const std::string& _title ) {
        title = _title;
        return *this;
    }

    UIShapeBuilder& inj( GeomAssetSP _cloned );

    UIShapeBuilder& ta( const UiControlFlag& _ta ) {
        if ( checkBitWiseFlag( _ta, UiControlFlag::TextAlignRight ) ) textAlignment = UiControlFlag::TextAlignRight;
        if ( checkBitWiseFlag( _ta, UiControlFlag::FloatRight ) ) textAlignment = UiControlFlag::TextAlignRight;
        if ( checkBitWiseFlag( _ta, UiControlFlag::TextAlignCenter ) ) textAlignment = UiControlFlag::TextAlignCenter;
        if ( checkBitWiseFlag( _ta, UiControlFlag::FloatCenter ) ) textAlignment = UiControlFlag::TextAlignCenter;
        return *this;
    }

    UIShapeBuilder& vn( const std::string& _vname ) {
        vname = _vname;
        return *this;
    }

    UIShapeBuilder& rbi( const int _rbi ) {
        renderBucketIndex = _rbi;
        return *this;
    }

    UIShapeBuilder& matchNameWithTName() {
        Name( tname );
        return *this;
    }

// MaterialBuildable policies
    UIShapeBuilder& m( const std::string& _shader, const std::string& _matName = "" ) {
        materialSet(_shader, _matName);
        return *this;
    }

    template <typename T>
    UIShapeBuilder& mc( const std::string& _name, T _value ) {
        materialConstant( _name, _value);
        return *this;
    }

    UIShapeBuilder& c( const Color4f & _color ) {
        materialColor( _color );
        return *this;
    }

    UIShapeBuilder& c( const std::string& _hexcolor ) {
        materialColor( Vector4f::XTORGBA( _hexcolor ) );
        return *this;
    }


    UIAssetSP buildr() {
        build();
        return elem;
    }

protected:
    void elemCreate() override;
    bool validate() const override;

protected:
    void createDependencyList() override;

private:
    std::shared_ptr<PosTex3dStrip> makeRoundedRect( const QuadVertices2& uvm );
    std::shared_ptr<PosTex3dStrip> makeRect( const QuadVertices2& uvm );
    std::shared_ptr<PosTex3dStrip> makeText( const Utility::TTFCore::Font& f );
    std::shared_ptr<PosTex3dStrip> makePolygon();
    std::shared_ptr<PosTex3dStrip> makeLine( const std::vector<Vector3f>& _vlist );
    std::shared_ptr<PosTex3dStrip> makeLines( const std::vector<std::vector<Vector3f>>& _vlists );
    std::shared_ptr<PosTex3dStrip> makeArrow();
    std::shared_ptr<PosTex3dStrip> makeSeparator();

private:
    UIShapeType shapeType = UIShapeType::Rect2d;
    std::shared_ptr<Matrix4f> mTransform = std::make_shared<Matrix4f>(Matrix4f::MIDENTITY());
    RectCreateAnchor anchor = RectCreateAnchor::None;
    RectFillMode fillMode = RectFillMode::AspectFill;
    JMATH::Rect2f rect = Rect2f::IDENTITY;
    float bevelRadius = 0.02f;
    Vector2f orig = Vector2f::HUGE_VALUE_NEG;
    Vector2f size = Vector2f::HUGE_VALUE_NEG;
    float mRot = 0.0f;
    Color4f  backgroundColor = Vector4f::ZERO;
    float zLevel = 0.0f;
    int renderBucketIndex = 0;

    // Line
    float lineWidth = 0.01f;

    // Arrows
    float   arrowAngleLenght = 0.22f;
    float   arrowAngle = M_PI_4 * 0.2f;
    float   arrowLength = 0.13f;
    float   arrowThickness = 0.02f;

    // Text
    UiControlFlag textAlignment = UiControlFlag::TextAlignLeft;

    bool isCentred = false;
    bool isExpandable = false;
    bool wrapLine = false;
    UIRenderFlags effects = UIRenderFlags::NoEffects;
    UiAlignElement alignElement;
    std::vector<Vector3f> outlineVerts;
    std::vector<Vector3f> triPolyMesh;
    float fontHeight = .1f;
    std::string title;
    std::string tname = "white";
    std::string vname;
    std::string fontName;

    UIAssetSP elem;

    static uint64_t sid;
};

using UISB = UIShapeBuilder;

