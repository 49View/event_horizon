#ifdef UE4
#include "SixthViewer.h"
#endif
#include "matrix3f.h"



const Matrix3f Matrix3f::IDENTITY = Matrix3f( Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ), Vector3f( 0.0f, 0.0f, 1.0f ) );
const Matrix3f Matrix3f::ZERO = Matrix3f( { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );
const Matrix3f Matrix3f::ONE = Matrix3f( { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } );

