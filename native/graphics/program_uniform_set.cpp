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
    values->visit( GPUUniformVisitor{handle} );
}

ProgramUniformSet::ProgramUniformSet( std::shared_ptr<HeterogeneousMap> _mat, Renderer& _rr )  {
    values = std::make_shared<GPUHeterogeneousMap>(_mat, _rr);
}

ProgramUniformSet::ProgramUniformSet() {
    values = std::make_shared<GPUHeterogeneousMap>();
}

GPUHeterogeneousMap::GPUHeterogeneousMap() {
    values = std::make_shared<HeterogeneousMap>();
}

GPUHeterogeneousMap::GPUHeterogeneousMap( std::shared_ptr<HeterogeneousMap> _values, Renderer& _rr ) :
                                          values( std::move( _values )) {

    int counter = 0;
    for( const auto& [k,n] : values->getMap<std::string>() ) {
        TextureUniformDesc u{};
        u.handle = _rr.TD( n )->getHandle();
        u.target = _rr.TD( n )->getTarget();
        u.slot = counter++;
//        LOGRS(k << " " << n << " has slot " << u.slot << " with handle: " << u.handle);
        mTextureMappings.emplace( k, u );
    };

}

std::shared_ptr<HeterogeneousMap> GPUHeterogeneousMap::Values() {
    return values;
}

void GPUHeterogeneousMap::textureAssign( const std::string& _key, const TextureUniformDesc& _value ) {
    mTextureMappings[_key] = _value;
}

void GPUHeterogeneousMap::textureUpdate( const std::string& _key, const std::string& _value, Renderer& _rr ) {
    mTextureMappings[_key].handle = _rr.TD( _value )->getHandle();
    mTextureMappings[_key].target = _rr.TD( _value )->getTarget();
}
