//
// Created by Dado on 2019-05-17.
//

#include "lightmap_manager.hpp"

#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <core/lightmap_exchange_format.h>
#include <core/game_time.h>

#include <graphics/opengl/gl_headers.hpp>

#define LIGHTMAPPER_IMPLEMENTATION
//#define LM_DEBUG_INTERPOLATION

#include <graphics/opengl/lightmapper_opengl.h>
#include <graphics/renderer.h>
#include <graphics/texture.h>
#include <poly/scene_graph.h>
#include <poly/baking/xatlas_client.hpp>

[[maybe_unused]] static void drawScene( LightmapSceneExchanger *scene, float *view, float *projection ) {
    glEnable(GL_DEPTH_TEST);

    glUseProgram(scene->program);
    glUniform1i(scene->u_lightmap, 0);
    glUniformMatrix4fv(scene->u_projection, 1, GL_FALSE, projection);
    glUniformMatrix4fv(scene->u_view, 1, GL_FALSE, view);

    glBindTexture(GL_TEXTURE_2D, scene->lightmap);

    glBindVertexArray(scene->vao);
    glDrawElements(GL_TRIANGLES, scene->indexCount, GL_UNSIGNED_INT, 0);
}

//static void destroyScene(scene_t *scene)
//{
//    free(scene->vertices);
//    free(scene->indices);
//    glDeleteVertexArrays(1, &scene->vao);
//    glDeleteBuffers(1, &scene->vbo);
//    glDeleteBuffers(1, &scene->ibo);
//    glDeleteTextures(1, &scene->lightmap);
//    glDeleteProgram(scene->program);
//}

static GLuint loadShader( GLenum type, const char *source ) {
    GLuint shader = glCreateShader(type);
    if ( shader == 0 ) {
        fprintf(stderr, "Could not create shader!\n");
        return 0;
    }
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if ( !compiled ) {
        fprintf(stderr, "Could not compile shader!\n");
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if ( infoLen ) {
            char *infoLog = (char *) malloc(infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            fprintf(stderr, "%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
static GLuint loadProgram( const char *vp, const char *fp, const char **attributes, int attributeCount ) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vp);
    if ( !vertexShader )
        return 0;
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fp);
    if ( !fragmentShader ) {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    if ( program == 0 ) {
        fprintf(stderr, "Could not create program!\n");
        return 0;
    }
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    for ( int i = 0; i < attributeCount; i++ )
        glBindAttribLocation(program, i, attributes[i]);

    glLinkProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if ( !linked ) {
        fprintf(stderr, "Could not link program!\n");
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if ( infoLen ) {
            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            fprintf(stderr, "%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

//static void multiplyMatrices(float *out, float *a, float *b)
//{
//    for (int y = 0; y < 4; y++)
//        for (int x = 0; x < 4; x++)
//            out[y * 4 + x] = a[x] * b[y * 4] + a[4 + x] * b[y * 4 + 1] + a[8 + x] * b[y * 4 + 2] + a[12 + x] * b[y * 4 + 3];
//}
//static void translationMatrix(float *out, float x, float y, float z)
//{
//    out[ 0] = 1.0f; out[ 1] = 0.0f; out[ 2] = 0.0f; out[ 3] = 0.0f;
//    out[ 4] = 0.0f; out[ 5] = 1.0f; out[ 6] = 0.0f; out[ 7] = 0.0f;
//    out[ 8] = 0.0f; out[ 9] = 0.0f; out[10] = 1.0f; out[11] = 0.0f;
//    out[12] = x;    out[13] = y;    out[14] = z;    out[15] = 1.0f;
//}
//static void rotationMatrix(float *out, float angle, float x, float y, float z)
//{
//    angle *= (float)M_PI / 180.0f;
//    float c = cosf(angle), s = sinf(angle), c2 = 1.0f - c;
//    out[ 0] = x*x*c2 + c;   out[ 1] = y*x*c2 + z*s; out[ 2] = x*z*c2 - y*s; out[ 3] = 0.0f;
//    out[ 4] = x*y*c2 - z*s; out[ 5] = y*y*c2 + c;   out[ 6] = y*z*c2 + x*s; out[ 7] = 0.0f;
//    out[ 8] = x*z*c2 + y*s; out[ 9] = y*z*c2 - x*s; out[10] = z*z*c2 + c;   out[11] = 0.0f;
//    out[12] = 0.0f;         out[13] = 0.0f;         out[14] = 0.0f;         out[15] = 1.0f;
//}
//static void transformPosition(float *out, float *m, float *p)
//{
//    float d = 1.0f / (m[3] * p[0] + m[7] * p[1] + m[11] * p[2] + m[15]);
//    out[2] =     d * (m[2] * p[0] + m[6] * p[1] + m[10] * p[2] + m[14]);
//    out[1] =     d * (m[1] * p[0] + m[5] * p[1] + m[ 9] * p[2] + m[13]);
//    out[0] =     d * (m[0] * p[0] + m[4] * p[1] + m[ 8] * p[2] + m[12]);
//}
//static void transposeMatrix(float *out, float *m)
//{
//    out[ 0] = m[0]; out[ 1] = m[4]; out[ 2] = m[ 8]; out[ 3] = m[12];
//    out[ 4] = m[1]; out[ 5] = m[5]; out[ 6] = m[ 9]; out[ 7] = m[13];
//    out[ 8] = m[2]; out[ 9] = m[6]; out[10] = m[10]; out[11] = m[14];
//    out[12] = m[3]; out[13] = m[7]; out[14] = m[11]; out[15] = m[15];
//}
//static void perspectiveMatrix(float *out, float fovy, float aspect, float zNear, float zFar)
//{
//    float f = 1.0f / tanf(fovy * (float)M_PI / 360.0f);
//    float izFN = 1.0f / (zNear - zFar);
//    out[ 0] = f / aspect; out[ 1] = 0.0f; out[ 2] = 0.0f;                       out[ 3] = 0.0f;
//    out[ 4] = 0.0f;       out[ 5] = f;    out[ 6] = 0.0f;                       out[ 7] = 0.0f;
//    out[ 8] = 0.0f;       out[ 9] = 0.0f; out[10] = (zFar + zNear) * izFN;      out[11] = -1.0f;
//    out[12] = 0.0f;       out[13] = 0.0f; out[14] = 2.0f * zFar * zNear * izFN; out[15] = 0.0f;
//}
//
//static void fpsCameraViewMatrix(GLFWwindow *window, float *view)
//{
//    // initial camera config
//    static float position[] = { 0.0f, 0.3f, 1.5f };
//    static float rotation[] = { 0.0f, 0.0f };
//
//    // mouse look
//    static double lastMouse[] = { 0.0, 0.0 };
//    double mouse[2];
//    glfwGetCursorPos(window, &mouse[0], &mouse[1]);
//    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//    {
//        rotation[0] += (float)(mouse[1] - lastMouse[1]) * -0.2f;
//        rotation[1] += (float)(mouse[0] - lastMouse[0]) * -0.2f;
//    }
//    lastMouse[0] = mouse[0];
//    lastMouse[1] = mouse[1];
//
//    float rotationY[16], rotationX[16], rotationYX[16];
//    rotationMatrix(rotationX, rotation[0], 1.0f, 0.0f, 0.0f);
//    rotationMatrix(rotationY, rotation[1], 0.0f, 1.0f, 0.0f);
//    multiplyMatrices(rotationYX, rotationY, rotationX);
//
//    // keyboard movement (WSADEQ)
//    float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 0.1f : 0.01f;
//    float movement[3] = {0};
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement[2] -= speed;
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement[2] += speed;
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement[0] -= speed;
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement[0] += speed;
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) movement[1] -= speed;
//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) movement[1] += speed;
//
//    float worldMovement[3];
//    transformPosition(worldMovement, rotationYX, movement);
//    position[0] += worldMovement[0];
//    position[1] += worldMovement[1];
//    position[2] += worldMovement[2];
//
//    // construct view matrix
//    float inverseRotation[16], inverseTranslation[16];
//    transposeMatrix(inverseRotation, rotationYX);
//    translationMatrix(inverseTranslation, -position[0], -position[1], -position[2]);
//    multiplyMatrices(view, inverseRotation, inverseTranslation); // = inverse(translation(position) * rotationYX);
//}

namespace LightmapManager {

    static constexpr int sLightMapSize = 512;

    int bake( LightmapSceneExchanger *scene, Renderer& rr ) {

        int w = scene->w, h = scene->h;

        lm_context *ctx = lmCreate(
                128,               // hemisphere resolution (power of two, max=512)
                0.001f, 100.0f,   // zNear, zFar of hemisphere cameras
                1.0f, 1.0f, 1.0f, // background color (white for ambient occlusion)
                2,
                0.01f,         // lightmap interpolation threshold (small differences are interpolated rather than sampled)
                // check debug_interpolation.tga for an overview of sampled (red) vs interpolated (green) pixels.
                0.0f);            // modifier for camera-to-surface distance for hemisphere rendering.
        // tweak this to trade-off between interpolated normals quality and other artifacts (see declaration).

        if ( !ctx ) {
            fprintf(stderr, "Error: Could not initialize lightmapper.\n");
            return 0;
        }

        constexpr int numBounces = 3;
        for ( int bounce = 0; bounce < numBounces; bounce++ ) {

            float *data = (float *) calloc(w * h * 4, sizeof(float));
            float *temp = (float *) calloc(w * h * 4, sizeof(float));

            glDisable(GL_CULL_FACE);

            lmSetTargetLightmap(ctx, data, w, h, 4);

            Matrix4f mIdentity{Matrix4f::MIDENTITY()};
            lmSetGeometry(ctx,
                          mIdentity.rawPtr(),                                                                 // no transformation in this example
                          LM_FLOAT, (unsigned char *) scene->vertices + offsetof(LightmapVertexExchanger, p), sizeof(LightmapVertexExchanger),
                          LM_NONE, NULL, 0, // no interpolated normals in this example
                          LM_FLOAT, (unsigned char *) scene->vertices + offsetof(LightmapVertexExchanger, t), sizeof(LightmapVertexExchanger),
                          scene->indexCount, LM_UNSIGNED_INT, scene->indices);

            int vp[4];
            float view[16], projection[16];
            auto lastUpdateTime = std::chrono::system_clock::now();
//            int iLoop = 0;
            while ( lmBegin(ctx, vp, view, projection) ) {
                // render to lightmapper framebuffer
                glViewport(vp[0], vp[1], vp[2], vp[3]);
                drawScene(scene, view, projection);

                // display progress every second (printf is expensive)
                auto time = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = time - lastUpdateTime;
//                LOGRS("Loops: " << iLoop++ );
                if ( elapsed_seconds.count() > 1.0 ) {
                    lastUpdateTime = time;
                    printf("\r%6.2f%%", lmProgress(ctx) * 100.0f);
                    fflush(stdout);
                }

                lmEnd(ctx);
            }
            printf("\rFinished baking %d triangles.\n", scene->indexCount / 3);

            // postprocess texture
            for ( int i = 0; i < 16; i++ ) {
                lmImageDilate(data, temp, w, h, 4);
                lmImageDilate(temp, data, w, h, 4);
            }

            lmImageSmooth(data, temp, w, h, 4);
            lmImageDilate(temp, data, w, h, 4);

            if ( bounce == numBounces-1 ) {
                lmImagePower(data, w, h, 4, 1.0f / 1.75f, 0x7); // gamma correct color channels
#ifndef ANDROID
                // save result to a file
                if ( lmImageSaveTGAf("result.tga", data, w, h, 4, 1.0f) )
                    printf("Saved result.tga\n");
#endif
            }

            // upload result
            glBindTexture(GL_TEXTURE_2D, scene->lightmap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, data);

            free(data);
            free(temp);
        }

        lmDestroy(ctx);

        glEnable(GL_BLEND);

        return 1;
    }

    int initScene( LightmapSceneExchanger *scene, Renderer& rr ) {

        glGenVertexArrays(1, &scene->vao);
        glBindVertexArray(scene->vao);

        glDisable(GL_BLEND);

        glGenBuffers(1, &scene->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, scene->vbo);
        glBufferData(GL_ARRAY_BUFFER, scene->vertexCount * sizeof(LightmapVertexExchanger), scene->vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &scene->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scene->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene->indexCount * sizeof(uint32_t), scene->indices,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LightmapVertexExchanger), (void *) offsetof(LightmapVertexExchanger, p));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(LightmapVertexExchanger), (void *) offsetof(LightmapVertexExchanger, t));

        // create lightmap texture
        auto sceneTexture = rr.TD(FBNames::lightmap);
        scene->lightmap = sceneTexture->getHandle();
        scene->w = sLightMapSize;
        scene->h = sLightMapSize;
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
        if ( !scene->program ) {
            fprintf(stderr, "Error loading shader\n");
            return 0;
        }
        scene->u_view = glGetUniformLocation(scene->program, "u_view");
        scene->u_projection = glGetUniformLocation(scene->program, "u_projection");
        scene->u_lightmap = glGetUniformLocation(scene->program, "u_lightmap");

        return 1;
    }

    LightmapSceneExchanger fillLightmapScene( SceneGraph& sg, FlattenGeomSP lightmapNodes ) {
        LightmapSceneExchanger _lightmapScene;

        // Get counting
        for ( const auto& gg : lightmapNodes ) {
            for ( const auto& dd : gg->DataV() ) {
                auto vData = sg.VL().get(dd.vData);
                _lightmapScene.vertexCount += vData->numVerts();
                _lightmapScene.indexCount += vData->numIndices();
            }
        }

        _lightmapScene.vertices = (LightmapVertexExchanger *)calloc(_lightmapScene.vertexCount, sizeof(LightmapVertexExchanger));
        _lightmapScene.indices = (uint32_t *)calloc(_lightmapScene.indexCount, sizeof(uint32_t));

        unsigned int vOff = 0;
        unsigned int iOff = 0;
        for ( const auto& gg : lightmapNodes ) {

            auto vStride = sizeof(LightmapVertexExchanger);
            for ( const auto& dd : gg->DataV() ) {
                auto vData = sg.VL().get(dd.vData);

                for (uint32_t v = 0; v < vData->numVerts(); v++) {
                    auto pos = vData->vertexAt(v);
                    auto mat = gg->getLocalHierTransform();
                    pos = mat->transform(pos);
                    auto uv = vData->uv2At(v);
                    memcpy( (char*)_lightmapScene.vertices + vOff + vStride*v + 0, (const void*)&pos, sizeof(float) * 3 );
                    memcpy( (char*)_lightmapScene.vertices + vOff + vStride*v + sizeof(float) * 3, (const void*)&uv, sizeof(float) * 2 );
                }
                for (uint32_t f = 0; f < vData->numIndices(); f++) {
                    _lightmapScene.indices[f+iOff] = iOff + vData->vIndexAt(f);
                }
                vOff += vData->numVerts() * sizeof(LightmapVertexExchanger);
                iOff += vData->numIndices();
            }
        }

        return _lightmapScene;
    }

    void bakeLightmaps( SceneGraph& sg, Renderer& rr, const std::unordered_set<uint64_t>& _exclusionTags ) {

        auto lightmapNodes = sg.getFlattenNodes(_exclusionTags);
        xAtlasParametrize(sg, lightmapNodes);
        LightmapSceneExchanger scene = fillLightmapScene(sg, lightmapNodes);
        if ( scene.vertexCount > 0 ) {
            LightmapManager::initScene(&scene, rr);
            LightmapManager::bake(&scene, rr);
            sg.updateNodes(lightmapNodes, GTBucket::Near);
        }
    }

}
