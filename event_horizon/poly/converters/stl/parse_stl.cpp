#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "parse_stl.h"
#include "core/node.hpp"
#include "poly/geom_data.hpp"
#include "poly/poly.hpp"

namespace stl {

    std::ostream& operator<<( std::ostream& out, const triangle& t ) {
        out << "---- TRIANGLE ----" << std::endl;
        out << t.normal << std::endl;
        out << t.v1 << std::endl;
        out << t.v2 << std::endl;
        out << t.v3 << std::endl;
        return out;
    }

    float parse_float( std::ifstream& s ) {
        char f_buf[sizeof( float )];
        s.read( f_buf, 4 );
        auto *fptr = (float *) f_buf;
        return *fptr;
    }

    Vector3f parse_point( std::ifstream& s ) {
        float x = parse_float( s );
        float y = parse_float( s );
        float z = parse_float( s );
        return Vector3f( x, y, z );
    }

    void addNormalToDic( const Vector3f& v1, const Vector3f& normal,
                          std::unordered_map<uint64_t, Vector3f>& normalsDic) {
        if ( auto v = normalsDic.find(v1.hash()); v != normalsDic.end() ) {
            normalsDic[v1.hash()] = v->second + normal;
        } else {
            normalsDic[v1.hash()] = normal;
        }
    }

    stl_data parse_stl( const std::string& stl_path ) {
        std::ifstream stl_file( stl_path.c_str(), std::ios::in | std::ios::binary );
        if ( !stl_file ) {
            std::cout << "ERROR: COULD NOT READ FILE" << std::endl;
            assert( false );
        }
        
        auto mGeom = std::make_shared<GeomData>();
        auto geom = std::make_shared<GeomAsset>(mGeom);

        VData& gd = mGeom->vData();
        
        char header_info[80] = "";
        char n_triangles[4];
        stl_file.read( header_info, 80 );
        stl_file.read( n_triangles, 4 );
        std::string h( header_info );
        stl_data info( h );
        auto *r = (unsigned int *) n_triangles;
        unsigned int num_triangles = *r;
        AABB box = AABB::INVALID;

        std::unordered_map<uint64_t, Vector3f> normalsDic;

        for ( unsigned int i = 0; i < num_triangles; i++ ) {
            auto normal = parse_point( stl_file );
            auto v1 = parse_point( stl_file );
            auto v2 = parse_point( stl_file );
            auto v3 = parse_point( stl_file );
            if ( normal == Vector3f::ZERO ) {
                normal = normalize( crossProduct( v1, v3, v2 ));
            }
            addNormalToDic( v1, normal, normalsDic );
            addNormalToDic( v2, normal, normalsDic );
            addNormalToDic( v3, normal, normalsDic );
            info.triangles.emplace_back( normal, v1, v2, v3 );
            char dummy[2];
            stl_file.read( dummy, 2 );

            box.expand(v1);
            box.expand(v2);
            box.expand(v3);
        }

        mGeom->BBox3d( box );

        int i = 0;
        Vector2f bs{ box.calcWidth(), -box.calcHeight() };
        for ( const auto& t : info.triangles ) {
            Vector3f tangent[3];
            Vector3f bitangent[3];
            Vector2f uv1 = t.v1.xy() / bs;
            Vector2f uv2 = t.v2.xy() / bs;
            Vector2f uv3 = t.v3.xy() / bs;

            tbCalc( t.v1, t.v2, t.v3, uv1, uv2, uv3, tangent[0], tangent[1], tangent[2], bitangent[0], bitangent[1],
                    bitangent[2] );

            gd.add( i++, t.v1, normalize(normalsDic[t.v1.hash()]), uv1, uv1, tangent[0], bitangent[0], Color4f::RED );
            gd.add( i++, t.v3, normalize(normalsDic[t.v3.hash()]), uv3, uv3, tangent[2], bitangent[2], Color4f::GREEN );
            gd.add( i++, t.v2, normalize(normalsDic[t.v2.hash()]), uv2, uv2, tangent[1], bitangent[1], Color4f::BLUE );
        }

        auto fname = getFileNameOnly(stl_path);

        return info;
    }

}
