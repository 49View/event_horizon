//
// Created by dado on 22/09/2020.
//

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
