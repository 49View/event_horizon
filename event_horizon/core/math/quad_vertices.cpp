#include "quad_vertices.h"



QuadVertices3::QuadVertices3( const Rect2f& r, const AABB& projection ) {
	int lda = projection.leastDominantAxis();

	switch ( lda ) {
	case 0:
	mVertices[0] = { 0.0f, r.bottomRight() };
	mVertices[1] = { projection.calcWidth(), r.topRight() };
	mVertices[2] = { 0.0f, r.bottomLeft() };
	mVertices[3] = { projection.calcWidth(), r.topLeft() };
	break;
	case 1:
	mVertices[0] = { r.bottomRight().x(), 0.0f, r.bottomRight().y() };
	mVertices[1] = { r.topRight().x(),    projection.calcHeight(), r.topRight().y() };
	mVertices[2] = { r.bottomLeft().x(),  0.0f, r.bottomLeft().y() };
	mVertices[3] = { r.topLeft().x(),     projection.calcHeight(), r.topLeft().y() };
	break;
	case 2:
	mVertices[0] = { r.bottomRight(), 0.0f };
	mVertices[1] = { r.topRight()   ,projection.calcDepth() };
	mVertices[2] = { r.bottomLeft() , 0.0f };
	mVertices[3] = { r.topLeft()    ,projection.calcDepth() };
	break;
	default:
	break;
	}

	for ( auto t = 0; t < 4; t++ ) {
		mVertices[t] += projection.minPoint();
	}
}

const QuadVertices4 QuadVertices4::QUAD_VERTICES( Vector4f( -1.0f, 1.0f, 0.0f, 1.0f ), Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ), Vector4f( -1.0f, -1.0f, 0.0f, 1.0f ),
												  Vector4f( 1.0f, -1.0f, 0.0f, 1.0f ) );

const QuadVertices3 QuadVertices3::QUAD_VERTICES_X( Vector3f( 0.0f, -1.0f, 1.0f ), Vector3f( 0.0f, 1.0f, 1.0f ), Vector3f( 0.0f, -1.0f, -1.0f ), Vector3f( 0.0f, 1.0f, -1.0f ) );
const QuadVertices3 QuadVertices3::QUAD_VERTICES_Y( Vector3f( -1.0f, 0.0f, 1.0f ), Vector3f( 1.0f, 0.0f, 1.0f ), Vector3f( -1.0f, 0.0f, -1.0f ), Vector3f( 1.0f, 0.0f, -1.0f ) );
const QuadVertices3 QuadVertices3::QUAD_VERTICES_Z( Vector3f( -1.0f, 1.0f, 0.0f ), Vector3f( 1.0f, 1.0f, 0.0f ), Vector3f( -1.0f, -1.0f, 0.0f ), Vector3f( 1.0f, -1.0f, 0.0f ) );

// Texture coorrds
const QuadVertices2 QuadVertices2::QUAD_TEX_COORDS_CENTERED( Vector2f( -.5f, -.5f ), Vector2f( .5f, -.5f ), Vector2f( -.5f, .5f ), Vector2f( .5f, .5f ) );

const QuadVertices2 QuadVertices2::QUAD_TEX_COORDS( Vector2f( 0.0f, 0.0f ), Vector2f( 1.0f, 0.0f ), Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, 1.0f ) );
const QuadVertices2 QuadVertices2::QUAD_INV_TEX_COORDS( Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, 1.0f ), Vector2f( 0.0f, 0.0f ), Vector2f( 1.0f, 0.0f ) );
const QuadVertices2 QuadVertices2::QUAD_TEX_COORDS_MIRROR( Vector2f( 1.0f, 0.0f ), Vector2f( 0.0f, 0.0f ), Vector2f( 1.0f, 1.0f ), Vector2f( 0.0f, 1.0f ) );
const QuadVertices2 QuadVertices2::QUAD_INV_TEX_COORDS_MIRROR( Vector2f( 1.0f, 1.0f ), Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, 0.0f ), Vector2f( 0.0f, 0.0f ) );

const QuadVertices2 QuadVertices2::QUAD_TEX_STRIP_COORDS( Vector2f( 1.0f, 0.0f ), Vector2f( 1.0f, 1.0f ), Vector2f( 0.0f, 0.0f ), Vector2f( 0.0f, 1.0f ) );
const QuadVertices2 QuadVertices2::QUAD_TEX_STRIP_INV_Y_COORDS( Vector2f( 1.0f, 1.0f ), Vector2f( 1.0f, 0.0f ), Vector2f( 0.0f, 1.0f ), Vector2f( 0.0f, 0.0f ) );
const QuadVertices2 QuadVertices2::QUAD_TEX_STRIP_INV_COORDS( Vector2f( 0.0f, 0.0f ), Vector2f( 1.0f, 0.0f ), Vector2f( 0.0f, 1.0f ), Vector2f( 1.0f, 1.0f ) );