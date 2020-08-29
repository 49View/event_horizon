#include "matrix4f.h"
#include "quaternion.h"
#include "matrix_anim.h"

void Matrix4f::setOrthogonalProjection() {
    mRows[0] = Vector4f( 2.0f, 0.0f, 0.0f, 0.0f );
    mRows[1] = Vector4f( 0.0f, 2.0f, 0.0f, 0.0f );
    mRows[2] = Vector4f( 0.0f, 0.0f, 0.0f, 0.0f );
    mRows[3] = Vector4f( -1.0f, -1.0f, 0.0f, 1.0f );
}

void
Matrix4f::setOrthogonalProjection( float leftv, float rightv, float topv, float bottomv, float nearv, float farv ) {
    //	Result[0][0] = valType(2) / (right - left);
    //    Result[1][1] = valType(2) / (top - bottom);
    //    Result[2][2] = - valType(2) / (zFar - zNear);
    //    Result[3][0] = - (right + left) / (right - left);
    //    Result[3][1] = - (top + bottom) / (top - bottom);
    //    Result[3][2] = - (zFar + zNear) / (zFar - zNear);

    mRows[0] = Vector4f( 2.0f / ( rightv - leftv ), 0.0f, 0.0f, 0.0f );
    mRows[1] = Vector4f( 0.0f, 2.0f / ( topv - bottomv ), 0.0f, 0.0f );
    mRows[2] = Vector4f( 0.0f, 0.0f, -( 2.0f / ( farv - nearv )), 0.0f );
    mRows[3] = Vector4f( -(( rightv + leftv ) / ( rightv - leftv )), -(( topv + bottomv ) / ( topv - bottomv )),
                         -(( farv + nearv ) / ( farv - nearv )), 1.0f );
}

void Matrix4f::setAspectRatioMatrix( float aspectRatio ) {
    mRows[0] = Vector4f( 1.0f / aspectRatio, 0.0f, 0.0f, 0.0f );
    mRows[1] = Vector4f( 0.0f, 1.0f, 0.0f, 0.0f );
    mRows[2] = Vector4f( 0.0f, 0.0f, 1.0f, 0.0f );
    mRows[3] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
}

void Matrix4f::setAspectRatioMatrixScreenSpace( float aspectRatio ) {
    mRows[0] = Vector4f( 2.0f / aspectRatio, 0.0f, 0.0f, 0.0f );
    mRows[1] = Vector4f( 0.0f, 2.0f, 0.0f, 0.0f );
    mRows[2] = Vector4f( 0.0f, 0.0f, -1.0f, 0.0f );
    mRows[3] = Vector4f( -1.0f, -1.0f, 0.0f, 1.0f );
}

const Matrix4f Matrix4f::IDENTITY = Matrix4f( Vector4f( 1.0f, 0.0f, 0.0f, 0.0f ),
                                              Vector4f( 0.0f, 1.0f, 0.0f, 0.0f ),
                                              Vector4f( 0.0f, 0.0f, 1.0f, 0.0f ),
                                              Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ));

const Matrix4f Matrix4f::IDENTITY_YINV = Matrix4f( Vector4f( 1.0f, 0.0f, 0.0f, 0.0f ),
                                                   Vector4f( 0.0f, -1.0f, 0.0f, 0.0f ),
                                                   Vector4f( 0.0f, 0.0f, 1.0f, 0.0f ),
                                                   Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ));

const Matrix4f Matrix4f::BEEF = Matrix4f( Vector4f( 32432.0f, 0.423432432f, 43243242.0f, 972378298.0f ),
                                          Vector4f( 5.0f, 1.0f, 432432432.0f, 4324230.0f ),
                                          Vector4f( 894343.0f, 7897432.0f, 343221.0f, 23.0f ),
                                          Vector4f( 0.4324324230f, 243343220.0f, 23154.0f, 5676751.0f ));

const Matrix4f Matrix4f::ZERO = Matrix4f( V4f::ZERO, V4f::ZERO, V4f::ZERO, V4f::ZERO );
const Matrix4f Matrix4f::ONE = Matrix4f( V4f::ONE, V4f::ONE, V4f::ONE, V4f::ONE );

void Matrix4f::make3x3NormalizedRotationMatrix() {
    setRow( 0, { normalize( getRow( 0 ).xyz()), 0.0f } );
    setRow( 1, { normalize( getRow( 1 ).xyz()), 0.0f } );
    setRow( 2, { normalize( getRow( 2 ).xyz()), 0.0f } );
    setRow( 3, Vector4f::W_AXIS );
}

void Matrix4f::setFromRotationAnglePos( const Vector3f &normal, const Vector3f &originAxis, const Vector3f &pos ) {
    if ( !isVerySimilar( normal, originAxis )) {
        if ( isVerySimilar( normal, originAxis * -1.0f )) {
            setRotation( M_PI, Vector3f::X_AXIS );
        } else {
            float cosTetha = dot( normal, originAxis );
            Vector3f nn = normalize( cross( normal, originAxis ));
            setRotationFromAngle( cosTetha, nn );
        }
    }
    setTranslation( pos );
}

void Matrix4f::setFromRTS( const Vector3f &pos, const Vector3f &axis, const Vector3f &zoom,
                           MatrixRotationOrder mro /*= MatrixRotationOrder::xyz*/ ) {
    Matrix4f rotMX = Matrix4f::IDENTITY;
    Matrix4f rotMY = Matrix4f::IDENTITY;
    Matrix4f rotMZ = Matrix4f::IDENTITY;
    Matrix4f scaleM = Matrix4f::IDENTITY;
    Matrix4f posM = Matrix4f::IDENTITY;
    rotMX.setRotation( axis.x(), Vector3f::X_AXIS );
    rotMY.setRotation( axis.y(), Vector3f::Y_AXIS );
    rotMZ.setRotation( axis.z(), Vector3f::Z_AXIS );
    scaleM.scale( zoom );
    posM.translate( pos );
    Matrix4f rotM;
    switch ( mro ) {
        case MatrixRotationOrder::xyz:
            rotM = rotMX * rotMY * rotMZ;
            break;
        case MatrixRotationOrder::xzy:
            rotM = rotMX * rotMZ * rotMY;
            break;
        case MatrixRotationOrder::yxz:
            rotM = rotMY * rotMX * rotMZ;
            break;
        case MatrixRotationOrder::yzx:
            rotM = rotMY * rotMZ * rotMX;
            break;
        case MatrixRotationOrder::zxy:
            rotM = rotMZ * rotMX * rotMY;
            break;
        case MatrixRotationOrder::zyx:
            rotM = rotMZ * rotMY * rotMX;
            break;
    }
    Matrix4f conc = rotM * scaleM * posM;
    for ( int t = 0; t < 4; t++ ) {
        setRow( t, conc.getRow( t ));
    }
}

void Matrix4f::setPerspective( float fovyInDegrees, float aspectRatio, float znear, float zfar ) {
    float ymax, xmax;
    float temp, temp2, temp3, temp4;
    ymax = znear * tanf( fovyInDegrees * M_PI / 360.0f );
    xmax = ymax * aspectRatio;
    float left = -xmax;
    float right = xmax;
    float bottom = -ymax;
    float top = ymax;

    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;

    mRows[0] = Vector4f( temp / temp2, 0.0f, 0.0f, 0.0f );
    mRows[1] = Vector4f( 0.0f, temp / temp3, 0.0f, 0.0f );
    mRows[2] = Vector4f(( right + left ) / temp2, ( top + bottom ) / temp3, -( zfar + znear ) / temp4, -1.0f );
    mRows[3] = Vector4f( 0.0f, 0.0f, ( -temp * zfar ) / temp4, 0.0f );
}

void Matrix4f::setOrthogonalProjection( float left, float right, float bottom, float top ) {
    mRows[0] = Vector4f( 2.0f / ( right - left ), 0.0f, 0.0f, 0.0f );
    mRows[1] = Vector4f( 0.0f, 2.0f / ( top - bottom ), 0.0f, 0.0f );
    mRows[2] = Vector4f( 0.0f, 0.0f, 0.0f, 0.0f );
    mRows[3] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
}

Matrix4f::Matrix4f( const Quaternion &rot ) {
    *this = rot.rotationMatrixNotNormalised();
}

Matrix4f::Matrix4f( const MatrixAnim &rts ) {
    Matrix4f mpos = Matrix4f{ rts.Pos() };
    Matrix4f mscale = Matrix4f::IDENTITY;
    Matrix4f mrot{ rts.Rot() };
    mscale.scale( rts.Scale());

    *this = mscale * mrot * mpos;
}

Matrix4f::Matrix4f( const Vector3f &pos, const Quaternion &axis, const Vector3f &zoom ) {
    Matrix4f mpos = Matrix4f{ pos };
    Matrix4f mscale = Matrix4f::IDENTITY;
    Matrix4f mrot{ axis };
    mscale.scale( zoom );

    *this = mscale * mrot * mpos;
}

void Matrix4f::lookAt2( const Vector3f &eye, const Vector3f &at, const Vector3f &up ) {
    Vector3f z = normalize( eye - at );  // Forward
    Vector3f x = normalize( cross( up, z )); // Right
    Vector3f y = cross( z, x );

    setRow( 0, Vector4f( x.x(), y.x(), z.x(), 0.0f ));
    setRow( 1, Vector4f( x.y(), y.y(), z.y(), 0.0f ));
    setRow( 2, Vector4f( x.z(), y.z(), z.z(), 0.0f ));
    setRow( 3, Vector4f( -( dot( x, eye )), -( dot( y, eye )), -( dot( z, eye )), 1.0f ));
}

std::ostream &operator<<( std::ostream &os, const Matrix4f &f ) {
    os << std::endl <<
       "Row0: " << f.mRows[0].toStringCommaSeparated() << std::endl <<
       "Row1: " << f.mRows[1].toStringCommaSeparated() << std::endl <<
       "Row2: " << f.mRows[2].toStringCommaSeparated() << std::endl <<
       "Row3: " << f.mRows[3].toStringCommaSeparated() << std::endl;
    return os;
}

