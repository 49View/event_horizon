#include "vertex_processing_opengl.h"

unsigned int sLastHandle = 0;
int64_t VertexProcessing::sMaterialHash;
int VertexProcessing::sNumDrawCalls;
int VertexProcessing::sNumStateChanges;
std::set<int64_t> VertexProcessing::sMatHash;

void VertexProcessing::programStart( std::shared_ptr<RenderMaterial> _material ) const {
    sNumDrawCalls++;
    if ( sLastHandle != _material->BoundProgram()->handle()) {
        GLCALL( glUseProgram( _material->BoundProgram()->handle()));
        sLastHandle = _material->BoundProgram()->handle();
    }
    _material->GlobalUniforms()->setOn( _material->BoundProgram()->handle());
    if ( _material->Hash() != sMaterialHash ) {
        sNumStateChanges++;
        //		sMatHash.insert( _material->Hash() );
        _material->Uniforms()->setOn( _material->BoundProgram()->handle());
        sMaterialHash = _material->Hash();
    }
}

void VertexProcessing::create( std::shared_ptr<cpuVBIB> vbib ) {
    bool bCreate = vao == 0;
    if ( bCreate ) glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    if ( bCreate ) glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, vbib->numVerts * vbib->elenentSize, vbib->bufferVerts.get(), GL_STATIC_DRAW );

    if ( bCreate ) {
        int stride = vbib->elenentSize;
        GLint attIndex = 0;
        for ( int32_t t = 0; t < vbib->vElementAttribSize; t++ ) {
            glEnableVertexAttribArray( attIndex );
            int size = vbib->vElementAttrib[t].size;
            uintptr_t offset = vbib->vElementAttrib[t].offset;
            if ( size <= 4 ) {
                glVertexAttribPointer( attIndex, size, GL_FLOAT, GL_FALSE, stride,
                                       reinterpret_cast<GLvoid *>( offset ));
                ++attIndex;
            } else {
                if ( size == 9 ) {
                    glEnableVertexAttribArray( attIndex + 1 );
                    glEnableVertexAttribArray( attIndex + 2 );
                    glVertexAttribPointer( attIndex, size / 3, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 0 * sizeof( float ) * 3 )));
                    glVertexAttribPointer( attIndex + 1, size / 3, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 1 * sizeof( float ) * 3 )));
                    glVertexAttribPointer( attIndex + 2, size / 3, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 2 * sizeof( float ) * 3 )));
                    attIndex += 3;
                } else if ( size == 16 ) {
                    glEnableVertexAttribArray( attIndex + 1 );
                    glEnableVertexAttribArray( attIndex + 2 );
                    glEnableVertexAttribArray( attIndex + 3 );
                    glVertexAttribPointer( attIndex, size / 4, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 0 * sizeof( float ) * 4 )));
                    glVertexAttribPointer( attIndex + 1, size / 4, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 1 * sizeof( float ) * 4 )));
                    glVertexAttribPointer( attIndex + 2, size / 4, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 2 * sizeof( float ) * 4 )));
                    glVertexAttribPointer( attIndex + 3, size / 4, GL_FLOAT, GL_FALSE, stride,
                                           reinterpret_cast<GLvoid *>( offset + ( 3 * sizeof( float ) * 4 )));
                    attIndex += 4;
                }
            }
        }
    }
    if ( vbib->numIndices > 0 ) {
        if ( bCreate ) glGenBuffers( 1, &ibo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, vbib->numIndices * sizeof( int32_t ), vbib->bufferIndices.get(),
                      GL_STATIC_DRAW );
    }

    numIndices = vbib->numIndices == 0 ? vbib->numVerts : vbib->numIndices;
    primitveType = vbib->primiteType;
}