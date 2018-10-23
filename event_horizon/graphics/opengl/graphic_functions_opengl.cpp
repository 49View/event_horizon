#include "../graphic_functions.hpp"
#include "gl_util.h"

void setDepthTest( bool enabled, DepthFunction df ) {
	if ( enabled ) {
		GLCALL( glEnable( GL_DEPTH_TEST ) );
		setDepthFunction( df );
	} else {
		GLCALL( glDisable( GL_DEPTH_TEST ) );
	}
}

void enableDepthTest( bool enabled ) {
	if ( enabled ) {
		GLCALL( glEnable( GL_DEPTH_TEST ) );
	} else {
		GLCALL( glDisable( GL_DEPTH_TEST ) );
	}
}

void setDepthWrite( bool enabled ) {
	glDepthMask( enabled ? GL_TRUE : GL_FALSE );
}

void setAlphaBlending( bool enabled ) {
	if ( enabled ) {
		GLCALL( glEnable( GL_BLEND ) );
		GLCALL( glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
	} else {
		GLCALL( glDisable( GL_BLEND ) );
	}
}

void setCullMode( CullMode cm ) {
	if ( cm == CULL_NONE ) {
		GLCALL( glDisable( GL_CULL_FACE ) );
	} else {
		GLCALL( glEnable( GL_CULL_FACE ) );
		GLCALL( glCullFace( cullModeToGl( cm ) ) );
	}
}

void setDepthFunction( DepthFunction dp ) {
	GLCALL( glDepthFunc( depthFunctionToGl( dp ) ) );
}

std::string cubemapFaceToString( const CubemapFaces cmf ) {
    switch ( cmf ) {
		case CubemapFaces::Top:
			return "top";
		case CubemapFaces::Bottom:
			return "bottom";
		case CubemapFaces::Front:
			return "front";
		case CubemapFaces::Back:
			return "back";
		case CubemapFaces::Left:
			return "left";
		case CubemapFaces::Right:
			return "right";
	}
	return "";
}

void grabScreen( int x, int y, int w, int h, void* buffer ) {
	for ( auto q = h+y-1, c=0; q >= y; q--, c++ ) {
		GLCALL( glReadPixels( x, q, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)((char*)buffer + (c*w*4))) );
	}
}
