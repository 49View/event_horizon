#include "gpuv_data.h"
#include <graphics/render_material.hpp>
#include <graphics/vertex_processing.h>

unsigned int sLastHandle = 0;

void GPUVData::programStart( RenderMaterial* _material, Program* _program ) const {
//    sNumDrawCalls++;
    auto handle = _program ? _program->handle() : _material->BoundProgram()->handle();
    if ( sLastHandle != handle) {
        GLCALL( glUseProgram( handle ));
        sLastHandle = handle;
    }
    _material->GlobalUniforms()->setOn( handle );
    static std::string sMaterialHash;

//    if ( _material->Hash() != sMaterialHash ) {
//        sNumStateChanges++;
    //		sMatHash.insert( _material->Hash() );
        _material->Uniforms()->setOn( handle );
        sMaterialHash = _material->Hash();
//    }
}

void GPUVData::programStart(  Program* _program ) const {
//    sNumDrawCalls++;
    if ( sLastHandle != _program->handle()) {
        GLCALL( glUseProgram( _program->handle() ));
        sLastHandle = _program->handle();
    }
}

void GPUVData::draw() const {
    GLCALL( glBindVertexArray( vao ) );
    if ( ibo == 0 ) {
        GLCALL( glDrawArrays( primitveType, 0, numIndices ) );
    } else {
        GLCALL( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ) );
        GLCALL( glDrawElements( primitveType, numIndices, GL_UNSIGNED_INT, (void *) 0 ) );
    }
}

void GPUVData::setupVIO() {
    bool bCreate = vao == 0;
    if ( bCreate ) GLCALL(glGenVertexArrays( 1, &vao ));
    GLCALL(glBindVertexArray( vao ));

    if ( bCreate ) glGenBuffers( 1, &vbo );
    GLCALL(glBindBuffer( GL_ARRAY_BUFFER, vbo ));
    GLCALL(glBufferData( GL_ARRAY_BUFFER, vbib.numVerts * vbib.elenentSize, vbib.bufferVerts.get(), GL_STATIC_DRAW ));

    if ( bCreate ) {
        int stride = vbib.elenentSize;
        GLint attIndex = 0;
        for ( int32_t t = 0; t < vbib.vElementAttribSize; t++ ) {
            GLCALL(glEnableVertexAttribArray( attIndex ));
            int size = vbib.vElementAttrib[t].size;
            uintptr_t offset = vbib.vElementAttrib[t].offset;
            if ( size <= 4 ) {
                GLCALL(glVertexAttribPointer( attIndex, size, GL_FLOAT, GL_FALSE, stride,
                                              reinterpret_cast<GLvoid *>( offset )));
                ++attIndex;
            } else {
                if ( size == 9 ) {
                    GLCALL(glEnableVertexAttribArray( attIndex + 1 ));
                    GLCALL(glEnableVertexAttribArray( attIndex + 2 ));
                    GLCALL(glVertexAttribPointer( attIndex, size / 3, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 0 * sizeof( float ) * 3 ))));
                    GLCALL(glVertexAttribPointer( attIndex + 1, size / 3, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 1 * sizeof( float ) * 3 ))));
                    GLCALL(glVertexAttribPointer( attIndex + 2, size / 3, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 2 * sizeof( float ) * 3 ))));
                    attIndex += 3;
                } else if ( size == 16 ) {
                    GLCALL(glEnableVertexAttribArray( attIndex + 1 ));
                    GLCALL(glEnableVertexAttribArray( attIndex + 2 ));
                    GLCALL(glEnableVertexAttribArray( attIndex + 3 ));
                    GLCALL(glVertexAttribPointer( attIndex, size / 4, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 0 * sizeof( float ) * 4 ))));
                    GLCALL(glVertexAttribPointer( attIndex + 1, size / 4, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 1 * sizeof( float ) * 4 ))));
                    GLCALL(glVertexAttribPointer( attIndex + 2, size / 4, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 2 * sizeof( float ) * 4 ))));
                    GLCALL(glVertexAttribPointer( attIndex + 3, size / 4, GL_FLOAT, GL_FALSE, stride,
                                                  reinterpret_cast<GLvoid *>( offset + ( 3 * sizeof( float ) * 4 ))));
                    attIndex += 4;
                }
            }
        }
    }
    if ( vbib.numIndices > 0 ) {
        if ( bCreate ) GLCALL(glGenBuffers( 1, &ibo ));
        GLCALL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ));
        GLCALL(glBufferData( GL_ELEMENT_ARRAY_BUFFER, vbib.numIndices * sizeof( uint32_t ), vbib.bufferIndices.get(),
                             GL_STATIC_DRAW ));
    }

    numIndices = static_cast<GLuint>(vbib.numIndices == 0 ? vbib.numVerts : vbib.numIndices);
    primitveType = primitiveToGl( vbib.primiteType );
}

void GPUVData::updateVBO( cpuVBIB&& _vbib ) {
    vbib = std::move(_vbib);
    setupVIO();
}

GPUVData::GPUVData( cpuVBIB&& _vbib ) : vbib(std::move(_vbib)) {
    setupVIO();
}

void GPUVData::deleteBuffers() {
    GLCALL(glDeleteBuffers( 1, &vbo ));
    GLCALL(glDeleteBuffers( 1, &ibo ));
    GLCALL(glDeleteVertexArrays( 1, &vao ));
    vao = 0;
    ibo = 0;
    vbo = 0;
}

bool GPUVData::isEmpty() const {
    return numIndices == 0 || vao == 0;
}

void GPUVData::updateUVs( const uint32_t *xrefs, const std::vector<V3f>& _pos, const std::vector<V2f>& _uvs, uint64_t _index, uint64_t _xrefStart ) {

    size_t uvStride = vbib.vElementAttrib[1+_index].offset;
    size_t uvSize = vbib.vElementAttrib[1+_index].size * sizeof(float);

    std::vector<V3f> remappedPos{};
    for ( size_t t = 0; t < vbib.numVerts; t++ ) {
        V3f p{};
        memcpy( (char*)(&p), vbib.bufferVerts.get() + vbib.elenentSize*t, sizeof(float)*3 );
        remappedPos.emplace_back( p );
    }

    for ( size_t t = 0; t < vbib.numVerts; t++ ) {
        memcpy( vbib.bufferVerts.get() + vbib.elenentSize*t + uvStride, (const char*)(&_uvs[t]), uvSize );
        memcpy( vbib.bufferVerts.get() + vbib.elenentSize*t, (const char*)(&_pos[t]), sizeof(float)*3 );
//        memcpy( vbib.bufferVerts.get() + vbib.elenentSize*t, (const char*)(&remappedPos[xrefs[t+_xrefStart]]), sizeof(float)*3 );
    }

    GLCALL(glBindVertexArray( vao ));
    GLCALL(glBindBuffer( GL_ARRAY_BUFFER, vbo ));
    GLCALL(glBufferData( GL_ARRAY_BUFFER, vbib.numVerts * vbib.elenentSize, vbib.bufferVerts.get(), GL_STATIC_DRAW ));

//    LOGRS( "Indices dump");
//    for ( auto t = 0; t < vbib.numIndices; t++ ) {
//        LOGRS( indices[t] );
//    }
//    GLCALL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ));
//    GLCALL(glBufferData( GL_ELEMENT_ARRAY_BUFFER, vbib.numIndices * sizeof( uint32_t ), indices,
//                         GL_STATIC_DRAW ));

}

void GPUVData::updateP3V3( const std::vector<V3f>& _values ) {

    size_t nStride = vbib.vElementAttrib[2].offset;
    size_t nSize = vbib.vElementAttrib[2].size * sizeof(float);

    for ( size_t t = 0; t < vbib.numVerts; t++ ) {
        memcpy( vbib.bufferVerts.get() + vbib.elenentSize*t + nStride, (const char*)(&_values[t]), nSize );
    }

    GLCALL(glBindVertexArray( vao ));
    GLCALL(glBindBuffer( GL_ARRAY_BUFFER, vbo ));
    GLCALL(glBufferData( GL_ARRAY_BUFFER, vbib.numVerts * vbib.elenentSize, vbib.bufferVerts.get(), GL_DYNAMIC_DRAW ));
}
