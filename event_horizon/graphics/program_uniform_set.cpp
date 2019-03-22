#include "program_uniform_set.h"
#include "texture_manager.h"
#include "renderer.h"
#include "core/serializebin.hpp"

std::map<std::string, int> ProgramUniformSet::mUBONames;
std::map<std::string, int> ProgramUniformSet::mUBOOffsetMap;
std::map<std::string, unsigned int> ProgramUniformSet::mUBOHandles;

int ProgramUniformSet::getUBOPoint( const std::string& ubo_name ) {
    auto n = mUBONames.find( ubo_name );
    if ( n == mUBONames.end()) {
        auto s = static_cast<int>( mUBONames.size());
        mUBONames[ubo_name] = s;
        return s;
    }
    return n->second;
}

void ProgramUniformSet::setUBOStructure( const std::string& uniformName, int off ) {
    mUBOOffsetMap[uniformName] = mUBOSize;
    mUBOSize+=off;
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, int value ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &value, 4 );
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float value ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &value, 4 );
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y ) {
    Vector2f v{ x, y };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y, float z ) {
    Vector3f v{ x, y, z };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector3f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y, float z, float w ) {
    Vector4f v{ x, y, z, w };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector4f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector2f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector3f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector3f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector4f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector4f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix2f& matrix ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &matrix, sizeof( Matrix2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix3f& matrix ) {
    int accOff = 0;
    for ( int t = 0; t < 3; t++ ) {
        std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName] + accOff, &matrix.getRow( t ), sizeof( Vector3f ));
        accOff += 16;
    }
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix4f& matrix ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &matrix, sizeof( Matrix4f ));
}

void ProgramUniformSet::setOn( unsigned int handle ) {
    visit( GPUUniformVisitor{handle} );
}

ProgramUniformSet::ProgramUniformSet( std::shared_ptr<Material> _mat, Renderer& _rr ) : Material(*_mat.get()) {

    visitTextures( [&]( TextureUniformDesc& u, unsigned int counter ){
        u.handle = _rr.TD( u.name )->getHandle();
        u.target = _rr.TD( u.name )->getTarget();
        u.slot = counter;
    });

}
