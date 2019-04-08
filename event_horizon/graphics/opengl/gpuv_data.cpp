#include "gpuv_data.h"
#include <graphics/render_material.hpp>
#include <graphics/vertex_processing.h>

unsigned int sLastHandle = 0;

void GPUVData::programStart( std::shared_ptr<RenderMaterial> _material ) const {
//    sNumDrawCalls++;
    if ( sLastHandle != _material->BoundProgram()->handle()) {
        GLCALL( glUseProgram( _material->BoundProgram()->handle()));
        sLastHandle = _material->BoundProgram()->handle();
    }
    _material->GlobalUniforms()->setOn( _material->BoundProgram()->handle());
//    if ( _material->Hash() != sMaterialHash ) {
//        sNumStateChanges++;
    //		sMatHash.insert( _material->Hash() );
    _material->Uniforms()->setOn( _material->BoundProgram()->handle());
//        sMaterialHash = _material->Hash();
//    }
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

GPUVData::GPUVData( std::shared_ptr<cpuVBIB> vbib ) {
    bool bCreate = vao == 0;
    if ( bCreate ) GLCALL(glGenVertexArrays( 1, &vao ));
    GLCALL(glBindVertexArray( vao ));

    if ( bCreate ) glGenBuffers( 1, &vbo );
    GLCALL(glBindBuffer( GL_ARRAY_BUFFER, vbo ));
    GLCALL(glBufferData( GL_ARRAY_BUFFER, vbib->numVerts * vbib->elenentSize, vbib->bufferVerts.get(), GL_STATIC_DRAW ));

    if ( bCreate ) {
        int stride = vbib->elenentSize;
        GLint attIndex = 0;
        for ( int32_t t = 0; t < vbib->vElementAttribSize; t++ ) {
            GLCALL(glEnableVertexAttribArray( attIndex ));
            int size = vbib->vElementAttrib[t].size;
            uintptr_t offset = vbib->vElementAttrib[t].offset;
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
    if ( vbib->numIndices > 0 ) {
        if ( bCreate ) GLCALL(glGenBuffers( 1, &ibo ));
        GLCALL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ));
        GLCALL(glBufferData( GL_ELEMENT_ARRAY_BUFFER, vbib->numIndices * sizeof( int32_t ), vbib->bufferIndices.get(),
                      GL_STATIC_DRAW ));
    }

    numIndices = static_cast<GLuint>(vbib->numIndices == 0 ? vbib->numVerts : vbib->numIndices);
    primitveType = primitiveToGl( vbib->primiteType );
}

void GPUVData::deleteBuffers() {
    GLCALL(glDeleteBuffers( 1, &vbo ));
    GLCALL(glDeleteBuffers( 1, &ibo ));
    GLCALL(glDeleteVertexArrays( 1, &vao ));
}

bool GPUVData::isEmpty() const {
    return numIndices == 0 || vao == 0;
}
