//
// Created by Dado on 08/02/2018.
//

#include "../../render_scene_graph.h"
#include "core/image_builder.h"
#include "poly/hier_geom.hpp"
#include "poly/geom_builder.h"
#include "poly/geom_builder.h"
#define LIGHTMAPPER_IMPLEMENTATION
#define LM_DEBUG_INTERPOLATION

#include "graphics/opengl/lightmapper_opengl.h"
#include "poly/lightmap_exchange_format.h"
#include <thekla/thekla_atlas.h>

scene_t _outputScene;

void chartCount( const std::shared_ptr<HierGeom> _g, Thekla::Atlas_Input_Mesh& inputMesh ) {
    if ( _g->Geom() ) {
        inputMesh.vertex_count += _g->Geom()->vData().numVerts();
        inputMesh.face_count += _g->Geom()->vData().numIndices()/3;
    }

    for ( const auto& c : _g->Children() ) {
        chartCount( c, inputMesh );
    }
}

void chart( const std::shared_ptr<HierGeom> _g, Thekla::Atlas_Input_Mesh& inputMesh, int& _vi, int& _fi ) {
    if ( _g->Geom() ) {
        auto vData = _g->Geom()->vData();
        Matrix4f lMatfull = *(_g->getLocalHierTransform().get());
        Matrix4f lRot = lMatfull;
        lRot.make3x3NormalizedRotationMatrix();

        for ( size_t q = 0; q < vData.numVerts(); q++ ) {
            Vector3f posTrasformed = lMatfull * vData.getVcoords3d()[q];
            Vector3f norTrasformed = lRot * vData.getVnormals3d()[q];
            norTrasformed = normalize( norTrasformed );

            inputMesh.vertex_array[_vi+q].position[0] = posTrasformed[0];
            inputMesh.vertex_array[_vi+q].position[1] = posTrasformed[1];
            inputMesh.vertex_array[_vi+q].position[2] = posTrasformed[2];
            inputMesh.vertex_array[_vi+q].normal[0] = norTrasformed[0];
            inputMesh.vertex_array[_vi+q].normal[1] = norTrasformed[1];
            inputMesh.vertex_array[_vi+q].normal[2] = norTrasformed[2];
            inputMesh.vertex_array[_vi+q].uv[0] = vData.getVUVs()[q][0];
            inputMesh.vertex_array[_vi+q].uv[1] = vData.getVUVs()[q][1];
            inputMesh.vertex_array[_vi+q].first_colocal = _vi+q;

            // Link colocals. You probably want to do this more efficiently! Sort by one axis or use a hash or grid.
//            for (int vv = 0; vv < q; vv++) {
//                if (inputMesh.vertex_array[_vi+q].position[0] == inputMesh.vertex_array[_vi+vv].position[0] &&
//                    inputMesh.vertex_array[_vi+q].position[1] == inputMesh.vertex_array[_vi+vv].position[1] &&
//                    inputMesh.vertex_array[_vi+q].position[2] == inputMesh.vertex_array[_vi+vv].position[2]) {
//                    inputMesh.vertex_array[_vi+q].first_colocal = _vi+vv;
//                }
//            }

        }

        size_t numFaceCount = vData.numIndices() / 3;
        for (size_t f = 0; f < numFaceCount; f++) {
            inputMesh.face_array[_fi+f].material_index = 0;
            inputMesh.face_array[_fi+f].vertex_index[0]= _vi + vData.getVIndices()[f*3+0];
            inputMesh.face_array[_fi+f].vertex_index[1]= _vi + vData.getVIndices()[f*3+1];
            inputMesh.face_array[_fi+f].vertex_index[2]= _vi + vData.getVIndices()[f*3+2];
        }

        _vi += vData.numVerts();
        _fi += numFaceCount;
    }

    for ( const auto& c : _g->Children() ) {
        chart( c, inputMesh, _vi, _fi );
    }
}

void chartInject( std::shared_ptr<HierGeom> _g,  scene_t& _outputScene,
                            const std::vector<Thekla::Atlas_Output_Vertex>& _va,
                            const Thekla::Atlas_Input_Vertex* _inputVerts,
                            int& _vi, int& _fi ) {
    if ( _g->Geom() ) {
        auto& vData = _g->Geom()->vData();
        for ( size_t q = 0; q < vData.numVerts(); q++ ) {
            Vector2f newUV{ _va[_vi+q].uv[0], _va[_vi+q].uv[1] };
            vData.setVUV2s( q, newUV );
            for ( auto i = 0; i < 3; i++ ) _outputScene.vertices[_vi+q].p[i] = _inputVerts[_vi+q].position[i];
            newUV.fill( _outputScene.vertices[_vi+q].t );
        }
        for ( size_t q = 0; q < vData.numIndices(); q+=3 ) {
            _outputScene.indices[_fi + q]   = static_cast<unsigned short>(_vi + vData.getVIndices()[q]);
            _outputScene.indices[_fi + q+1] = static_cast<unsigned short>(_vi + vData.getVIndices()[q+1]);
            _outputScene.indices[_fi + q+2] = static_cast<unsigned short>(_vi + vData.getVIndices()[q+2]);
        }
        _vi += vData.numVerts();
        _fi += vData.numIndices();
    }

    for ( const auto& c : _g->Children() ) {
        chartInject( c, _outputScene, _va, _inputVerts, _vi, _fi );
    }
}

bool aovcompare(Thekla::Atlas_Output_Vertex lhs, Thekla::Atlas_Output_Vertex rhs) {
    return lhs.xref < rhs.xref;
}

bool packGeometryForLightmaps( std::unordered_map<std::string, std::shared_ptr<HierGeom>>& _geoms ) {
    Thekla::Atlas_Input_Mesh inputMesh{};

    inputMesh.vertex_count = 0;
    inputMesh.face_count = 0;
    for ( const auto&[k, v] : _geoms ) {
        chartCount( v, inputMesh );
    }

    LOGR( "Total Verts: %d", inputMesh.vertex_count );
    LOGR( "Total Faces: %d", inputMesh.face_count );

    inputMesh.vertex_array = new Thekla::Atlas_Input_Vertex[inputMesh.vertex_count];
    inputMesh.face_array = new Thekla::Atlas_Input_Face[inputMesh.face_count];

    // Restart with calculus loop
    inputMesh.vertex_count = 0;
    inputMesh.face_count = 0;
    for ( const auto&[k, v] : _geoms ) {
        chart( v, inputMesh, inputMesh.vertex_count, inputMesh.face_count );
    }

//    for ( auto q = 0; q < inputMesh.vertex_count; q++ ) {
//        LOGR("Vertex %d: %f, %f, %f", q, inputMesh.vertex_array[q].position[0], inputMesh.vertex_array[q]
//        .position[1], inputMesh.vertex_array[q].position[2] );
//        LOGR("UV %d: %f, %f", q, inputMesh.vertex_array[q].uv[0], inputMesh.vertex_array[q]
//                .uv[1] );
//    }

//    for ( auto q = 0; q < inputMesh.face_count; q++ ) {
//        LOGR("Face %d: %d, %d, %d", q, inputMesh.face_array[q].vertex_index[0],
//             inputMesh.face_array[q].vertex_index[1], inputMesh.face_array[q].vertex_index[2] );
//    }

    Thekla::Atlas_Options atlas_options{};
    atlas_set_default_options(&atlas_options);

    // Avoid brute force packing, since it can be unusably slow in some situations.
    atlas_options.packer_options.witness.packing_quality = 1;

    Thekla::Atlas_Error error = Thekla::Atlas_Error_Success;

    Thekla::Atlas_Output_Mesh * output_mesh = atlas_generate(&inputMesh, &atlas_options, &error);

    if ( error == Thekla::Atlas_Error_Success ) {
        LOGR("Atlas mesh has %d verts", output_mesh->vertex_count);
        LOGR("Atlas mesh has %d triangles", output_mesh->index_count / 3);

        std::vector<Thekla::Atlas_Output_Vertex> vos;
        for ( auto q = 0; q < output_mesh->vertex_count; q++ ) {
//            LOGR( "VertexUV %d: %f %f, %d", q, output_mesh->vertex_array[q].uv[0],
//                  output_mesh->vertex_array[q].uv[1], output_mesh->vertex_array[q].xref );
            vos.push_back( output_mesh->vertex_array[q] );
        }

//        for ( auto q = 0; q < output_mesh->index_count-3; q+=3 ) {
//            LOGR( "FaceOutput %d: %d, %d, %d", q, output_mesh->index_array[q], output_mesh->index_array[q+1],
//                  output_mesh->index_array[q+2]);
//        }

        std::sort(vos.begin(), vos.end(), aovcompare);

        for ( size_t q = 0; q < vos.size(); q++ ) {
            vos[q].uv[0] /= output_mesh->atlas_width;
            vos[q].uv[1] /= output_mesh->atlas_height;
        }

        // Inject chart mapping to UV2
        int vi = 0;
        int fi = 0;
        _outputScene.indexCount = output_mesh->index_count;
        _outputScene.vertexCount = output_mesh->vertex_count;
        _outputScene.indices = new unsigned short[_outputScene.indexCount];
        _outputScene.vertices = new vertex_t[_outputScene.vertexCount];
        _outputScene.w = output_mesh->atlas_width * 8;
        _outputScene.h = output_mesh->atlas_height * 8;

        for ( auto& [k,v] : _geoms ) {
            chartInject( v, _outputScene, vos, inputMesh.vertex_array, vi, fi );
        }

//    for ( auto q = 0; q < output_mesh->vertex_count; q++ ) {
//        LOGR("Vertex %d: %d", q, output_mesh->index_array[q]);
//    }

//    LOGR("Produced debug_packer_final.tga");
        return true;
    } else {
        LOGR("[ERROR] Failed to chart mesh with thekla");
        return false;
    }
}

static GLuint loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0)
    {
        fprintf(stderr, "Could not create shader!\n");
        return 0;
    }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        fprintf(stderr, "Could not compile shader!\n");
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen)
        {
            char* infoLog = (char*)malloc(infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            fprintf(stderr, "%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
static GLuint loadProgram(const char *vp, const char *fp, const char **attributes, int attributeCount)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vp);
    if (!vertexShader)
        return 0;
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fp);
    if (!fragmentShader)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program == 0)
    {
        fprintf(stderr, "Could not create program!\n");
        return 0;
    }
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    for (int i = 0; i < attributeCount; i++)
        glBindAttribLocation(program, i, attributes[i]);

    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        fprintf(stderr, "Could not link program!\n");
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            fprintf(stderr, "%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

int initScene(scene_t *scene)
{
    // load mesh
//    if ( !loadSimpleObjFile("gazebo.obj", &scene->vertices, &scene->vertexCount, &scene->indices, &scene->indexCount) )
//    {
//        fprintf(stderr, "Error loading obj file\n");
//        return 0;
//    }

    glGenVertexArrays(1, &scene->vao);
    glBindVertexArray(scene->vao);

    glGenBuffers(1, &scene->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, scene->vbo);
    glBufferData(GL_ARRAY_BUFFER, scene->vertexCount * sizeof(vertex_t), scene->vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &scene->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene->indexCount * sizeof(unsigned short), scene->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, p));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, t));

    // create lightmap texture
//    scene->w = 654;
//    scene->h = 654;
    glGenTextures( 1, &scene->lightmap );
    glBindTexture(GL_TEXTURE_2D, scene->lightmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unsigned char emissive[] = { 0, 0, 0, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emissive);

    // load shader
    const char *vp =
            "#version 150 core\n"
            "in vec3 a_position;\n"
            "in vec2 a_texcoord;\n"
            "uniform mat4 u_view;\n"
            "uniform mat4 u_projection;\n"
            "out vec2 v_texcoord;\n"

            "void main()\n"
            "{\n"
            "gl_Position = u_projection * (u_view * vec4(a_position, 1.0));\n"
            "v_texcoord = a_texcoord;\n"
            "}\n";

    const char *fp =
            "#version 150 core\n"
            "in vec2 v_texcoord;\n"
            "uniform sampler2D u_lightmap;\n"
            "out vec4 o_color;\n"

            "void main()\n"
            "{\n"
            "o_color = vec4(texture(u_lightmap, v_texcoord).rgb, gl_FrontFacing ? 1.0 : 0.0);\n"
            "}\n";

    const char *attribs[] =
            {
                    "a_position",
                    "a_texcoord"
            };

    scene->program = loadProgram(vp, fp, attribs, 2);
    if (!scene->program)
    {
        fprintf(stderr, "Error loading shader\n");
        return 0;
    }
    scene->u_view = glGetUniformLocation(scene->program, "u_view");
    scene->u_projection = glGetUniformLocation(scene->program, "u_projection");
    scene->u_lightmap = glGetUniformLocation(scene->program, "u_lightmap");

    return 1;
}

void drawScene(scene_t *scene, float *view, float *projection)
{
    glEnable(GL_DEPTH_TEST);

    glUseProgram(scene->program);
    glUniform1i(scene->u_lightmap, 0);
    glUniformMatrix4fv(scene->u_projection, 1, GL_FALSE, projection);
    glUniformMatrix4fv(scene->u_view, 1, GL_FALSE, view);

    glBindTexture(GL_TEXTURE_2D, scene->lightmap);

    glBindVertexArray(scene->vao);
    glDrawElements(GL_TRIANGLES, scene->indexCount, GL_UNSIGNED_SHORT, 0);
}

void destroyScene(scene_t *scene)
{
    free(scene->vertices);
    free(scene->indices);
    glDeleteVertexArrays(1, &scene->vao);
    glDeleteBuffers(1, &scene->vbo);
    glDeleteBuffers(1, &scene->ibo);
    glDeleteTextures(1, &scene->lightmap);
    glDeleteProgram(scene->program);
}

int loadSimpleObjFile(const char *filename, vertex_t **vertices, unsigned int *vertexCount, unsigned short **indices, unsigned int *indexCount)
{
    FILE *file = fopen(filename, "rt");
    if (!file)
        return 0;
    char line[1024];

    // first pass
    unsigned int np = 0, nn = 0, nt = 0, nf = 0;
    while (!feof(file))
    {
        fgets(line, 1024, file);
        if (line[0] == '#') continue;
        if (line[0] == 'v')
        {
            if (line[1] == ' ') { np++; continue; }
            if (line[1] == 'n') { nn++; continue; }
            if (line[1] == 't') { nt++; continue; }
            assert(!"unknown vertex attribute");
        }
        if (line[0] == 'f') { nf++; continue; }
        assert(!"unknown identifier");
    }
    assert(np && np == nn && np == nt && nf); // only supports obj files without separately indexed vertex attributes

    // allocate memory
    *vertexCount = np;
    *vertices = (vertex_t*)calloc(np, sizeof(vertex_t));
    *indexCount = nf * 3;
    *indices = (unsigned short*)calloc(nf * 3, sizeof(unsigned short));

    // second pass
    fseek(file, 0, SEEK_SET);
    unsigned int cp = 0, ct = 0, cf = 0;
    while (!feof(file))
    {
        fgets(line, 1024, file);
        if (line[0] == '#') continue;
        if (line[0] == 'v')
        {
            if (line[1] == ' ') { float *p = (*vertices)[cp++].p; char *e1, *e2; p[0] = (float)strtod(line + 2, &e1); p[1] = (float)strtod(e1, &e2); p[2] = (float)strtod(e2, 0); continue; }
            if (line[1] == 'n') { /*float *n = (*vertices)[cn++].n; char *e1, *e2; n[0] = (float)strtod(line + 3, &e1); n[1] = (float)strtod(e1, &e2); n[2] = (float)strtod(e2, 0);*/ continue; } // no normals needed
            if (line[1] == 't') { float *t = (*vertices)[ct++].t; char *e1;      t[0] = (float)strtod(line + 3, &e1); t[1] = (float)strtod(e1, 0);                                continue; }
            assert(!"unknown vertex attribute");
        }
        if (line[0] == 'f')
        {
            unsigned short *tri = (*indices) + cf;
            cf += 3;
            char *e1, *e3 = line + 1;
#ifndef NDEBUG
            char *e2;
#endif
            for (int i = 0; i < 3; i++)
            {
                unsigned long pi = strtoul(e3 + 1, &e1, 10);
#ifndef NDEBUG
                assert(e1[0] == '/');
                unsigned long ti = strtoul(e1 + 1, &e2, 10);
                assert(e2[0] == '/');
                unsigned long ni = strtoul(e2 + 1, &e3, 10);
                assert(pi == ti && pi == ni);
#endif
                tri[i] = (unsigned short)(pi - 1);
            }
            continue;
        }
        assert(!"unknown identifier");
    }

    fclose(file);
    return 1;
}

int RenderSceneGraph::bake(scene_t *scene)
{
    lm_context *ctx = lmCreate(
            64,               // hemisphere resolution (power of two, max=512)
            0.001f, 100.0f,   // zNear, zFar of hemisphere cameras
            1.0f, 1.0f, 1.0f, // background color (white for ambient occlusion)
            2, 0.01f,         // lightmap interpolation threshold (small differences are interpolated rather than sampled)
            // check debug_interpolation.tga for an overview of sampled (red) vs interpolated (green) pixels.
            0.0f);            // modifier for camera-to-surface distance for hemisphere rendering.
    // tweak this to trade-off between interpolated normals quality and other artifacts (see declaration).

    if (!ctx)
    {
        fprintf(stderr, "Error: Could not initialize lightmapper.\n");
        return 0;
    }

    int w = scene->w, h = scene->h;
    float *data = (float*)calloc(w * h * 4, sizeof(float));
    lmSetTargetLightmap(ctx, data, w, h, 4);

    lmSetGeometry(ctx, NULL,                                                                 // no transformation in this example
                  LM_FLOAT, (unsigned char*)scene->vertices + offsetof(vertex_t, p), sizeof(vertex_t),
                  LM_NONE , NULL                                                   , 0               , // no interpolated normals in this example
                  LM_FLOAT, (unsigned char*)scene->vertices + offsetof(vertex_t, t), sizeof(vertex_t),
                  scene->indexCount, LM_UNSIGNED_SHORT, scene->indices);

    int vp[4];
    float view[16], projection[16];
    double lastUpdateTime = 0.0;
    while (lmBegin(ctx, vp, view, projection))
    {
        // render to lightmapper framebuffer
        glViewport(vp[0], vp[1], vp[2], vp[3]);
        drawScene(scene, view, projection);

        // display progress every second (printf is expensive)
        double time = glfwGetTime();
        if (time - lastUpdateTime > 1.0)
        {
            lastUpdateTime = time;
            printf("\r%6.2f%%", lmProgress(ctx) * 100.0f);
            fflush(stdout);
        }

        lmEnd(ctx);
    }
    printf("\rFinished baking %d triangles.\n", scene->indexCount / 3);

    lmDestroy(ctx);

    // postprocess texture
    float *temp = (float*)calloc(w * h * 4, sizeof(float));
    for (int i = 0; i < 16; i++)
    {
        lmImageDilate(data, temp, w, h, 4);
        lmImageDilate(temp, data, w, h, 4);
    }
    lmImageSmooth(data, temp, w, h, 4);
    lmImageDilate(temp, data, w, h, 4);
    lmImagePower(data, w, h, 4, 1.0f / 2.2f, 0x7); // gamma correct color channels
    free(temp);

    // save result to a file
    if (lmImageSaveTGAf("lightmap_result.tga", data, w, h, 4, 1.0f))
        printf("Saved lightmap_result.tga\n");

    // upload result
    glBindTexture(GL_TEXTURE_2D, scene->lightmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, data);
    free(data);

    rr.clearTargets();
    rr.TM().addTextureWithGPUHandle( "lightMap_t", scene->lightmap, TSLOT_LIGHTMAP );

    for ( const auto&[k, v] : geoms ) {
        v->generateSOA();
    }

    return 1;
}


void RenderSceneGraph::cmdCalcLightmapsImpl( const std::vector<std::string>& _params ) {

//    loadSimpleObjFile("/Users/Dado/Documents/49View/external/thekla_atlas_b/gazebo.obj",
//                      &_outputScene.vertices, &_outputScene.vertexCount, &_outputScene.indices, &_outputScene.indexCount);
//    _outputScene.w = 654;
//    _outputScene.h = 654;
//    initScene( &_outputScene );
//    bake( &_outputScene );
    if ( packGeometryForLightmaps( geoms ) ) {
        initScene( &_outputScene );
        bake( &_outputScene );
    }
}
