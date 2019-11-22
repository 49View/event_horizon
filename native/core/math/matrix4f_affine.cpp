#ifdef UE4
#include "SixthViewer.h"
#endif
#include "matrix4f_affine.h"



const Matrix4fAffine Matrix4fAffine::IDENTITY = Matrix4fAffine( Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ), Vector3f( 0.0f, 0.0f, 1.0f ), Vector3f( 0.0f, 0.0f, 0.0f ) );

