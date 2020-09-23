//
// Created by dado on 22/09/2020.
//

//struct VertexOffsetScene {
//    VertexOffsetScene( const std::string& uuid, size_t voffset, size_t vsize, size_t ioffset, size_t isize ) : uuid(
//            uuid ), voffset( voffset ), vsize( vsize ), ioffset( ioffset ), isize( isize ) {}
//
//    std::string uuid;
//    size_t voffset = 0;
//    size_t vsize = 0;
//    size_t ioffset = 0;
//    size_t isize = 0;
//};

//void writeFace( size_t base, size_t i1, size_t i2, size_t i3, std::ostringstream& ssf ) {
//    size_t pi1 = base + i1;
//    size_t pi2 = base + i2;
//    size_t pi3 = base + i3;
//    ssf << "f " << pi1 << "/" << pi1 << "/" << pi1 << " ";
//    ssf << pi2 << "/" << pi2 << "/" << pi2 << " ";
//    ssf << pi3 << "/" << pi3 << "/" << pi3;
//    ssf << std::endl;
//}
//
//void SceneGraph::chartMeshes( scene_t& scene ) {
//    std::ostringstream ss;
//    std::ostringstream ssf;
//    size_t totalVerts = 1;
//
//    std::vector<VertexOffsetScene> unchart;
//
//    size_t currUnchartOffset = 0;
//    for ( const auto&[k, gg] : nodes ) {
//        if ( !gg->empty()) {
//            auto mat = gg->getLocalHierTransform();
//            auto vData = vl.get(gg->Data(0).vData);
//            unchart.emplace_back(gg->UUiD(), currUnchartOffset, vData->numVerts(), currUnchartOffset,
//                                 vData->numVerts());
//            currUnchartOffset += vData->numVerts();
//            for ( size_t t = 0; t < vData->numVerts(); t++ ) {
//                auto v = vData->vertexAt(t);
//                v = mat->transform(v);
//                ss << v.toStringObj("v");
//                auto n = vData->normalAt(t);
//                n = mat->transform(n);
//                n = normalize(n);
//                ss << n.toStringObj("vn");
//                ss << vData->uvAt(t).toStringObj("vt");
//            }
//            for ( size_t t = 0; t < vData->numIndices(); t += 3 ) {
//                writeFace(totalVerts,
//                          vData->getVIndices()[t],
//                          vData->getVIndices()[t + 1],
//                          vData->getVIndices()[t + 2],
//                          ssf);
//            }
//            totalVerts += vData->numIndices();
//        }
//    }
//
//    ss << ssf.str();
//    LOGRS(ss.str());
//    std::istringstream ssi(ss.str());
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string basePath;
//    tinyobj::MaterialFileReader matFileReader(basePath);
//    std::string err;
//    if ( !tinyobj::LoadObj(shapes, materials, err, ssi, matFileReader, 0)) {
//        LOGR("Error: %s\n", err.c_str());
//    }
//
//    xatlasParametrize(shapes, &scene);
////    FM::writeLocalFile("house.obj", ss.str() );
//
//}

//void saveToObj( xatlas::Atlas *atlas, std::vector<tinyobj::shape_t>& shapes ) {
//    // Write meshes.
//    char filename[256];
//    snprintf(filename, sizeof(filename), "output.obj");
//    printf("Writing '%s'...\n", filename);
//    FILE *file;
//    FOPEN(file, filename, "w");
//    if (file) {
//        uint32_t firstVertex = 0;
//        for (uint32_t i = 0; i < atlas->meshCount; i++) {
//            const xatlas::Mesh &mesh = atlas->meshes[i];
//            for (uint32_t v = 0; v < mesh.vertexCount; v++) {
//                const xatlas::Vertex &vertex = mesh.vertexArray[v];
//                const float *pos = &shapes[i].mesh.positions[vertex.xref * 3];
//                fprintf(file, "v %g %g %g\n", pos[0], pos[1], pos[2]);
//                if (!shapes[i].mesh.normals.empty()) {
//                    const float *normal = &shapes[i].mesh.normals[vertex.xref * 3];
//                    fprintf(file, "vn %g %g %g\n", normal[0], normal[1], normal[2]);
//                }
//                fprintf(file, "vt %g %g\n", vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height);
//            }
//            fprintf(file, "o mesh%03u\n", i);
//            fprintf(file, "s off\n");
//            for (uint32_t f = 0; f < mesh.indexCount; f += 3) {
//                fprintf(file, "f ");
//                for (uint32_t j = 0; j < 3; j++) {
//                    const uint32_t index = firstVertex + mesh.indexArray[f + j] + 1; // 1-indexed
//                    fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
//                }
//            }
//            fprintf(file, "g charts\n");
//            for (uint32_t c = 0; c < mesh.chartCount; c++) {
//                const xatlas::Chart *chart = &mesh.chartArray[c];
//                fprintf(file, "o chart%04u\n", c);
//                fprintf(file, "s off\n");
//                for (uint32_t f = 0; f < chart->faceCount; f += 3) {
//                    fprintf(file, "f ");
//                    for (uint32_t j = 0; j < 3; j++) {
//                        const uint32_t index = firstVertex + chart->faceArray[f + j] + 1; // 1-indexed
//                        fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
//                    }
//                }
//            }
//            firstVertex += mesh.vertexCount;
//        }
//        fclose(file);
//    }
//}
//
//void saveToSceneT( xatlas::Atlas *atlas, std::vector<tinyobj::shape_t>& shapes, scene_t* scene ) {
//
//    // allocate memory
//    scene->vertexCount = atlas->meshes[0].vertexCount;
//    scene->vertices = (vertex_t *)calloc(scene->vertexCount, sizeof(vertex_t));
//    scene->indexCount = atlas->meshes[0].indexCount;
//    scene->indices = (unsigned short *)calloc(scene->indexCount, sizeof(unsigned short));
//    scene->xrefs   = (uint32_t *)calloc(scene->indexCount, sizeof(uint32_t));
//
//    uint32_t firstVertex = 0;
//    for (uint32_t i = 0; i < atlas->meshCount; i++) {
//        const xatlas::Mesh &mesh = atlas->meshes[i];
//        for (uint32_t v = 0; v < mesh.vertexCount; v++) {
//            const xatlas::Vertex &vertex = mesh.vertexArray[v];
//            const float *pos = &shapes[i].mesh.positions[vertex.xref * 3];
//            size_t voff = (v * sizeof(vertex_t));
//            float uvs[2];
//            uvs[0] = vertex.uv[0] / atlas->width;
//            uvs[1] = vertex.uv[1] / atlas->height;
//            memcpy( (char*)scene->vertices + voff + 0, pos, sizeof(float) * 3 );
//            memcpy( (char*)scene->vertices + voff + sizeof(float) * 3, uvs, sizeof(float) * 2 );
//            scene->xrefs[v] = vertex.xref;
//        }
//        for (uint32_t f = 0; f < mesh.indexCount; f += 3) {
//            for (uint32_t j = 0; j < 3; j++) {
//                const uint32_t index = firstVertex + mesh.indexArray[f + j]; // add +1 for obj file indexed
//                scene->indices[f+j] = index;
//            }
//        }
//        firstVertex += mesh.vertexCount;
//    }
//}
