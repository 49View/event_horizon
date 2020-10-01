//
//  shapes.cpp
//  6thView
//
//  Created by Dado on 99/99/1970.
//
//

#include "poly_shapes.hpp"
#include <unordered_map>
#include <core/soa_utils.h>
#include <core/string_util.h>

Vector3f Topology::normalFromSmartSmoothing( uint32_t index, int vIndex ) {
    size_t idx = vertices[index].hash();
    bool bSmooth = false;
    Vector3f n = vertexNormals[vIndex];
    for ( const auto& v : smoothing[idx] ) {
        bSmooth = true;
        float odot = dot(n, v);
        float ndot = acos(odot) - M_PI_2;
        if ( fabs(ndot) < 0.05f ) {
            bSmooth = false;
            break;
        }
    }
    if ( bSmooth ) {
        Vector3f tn = V3fc::ZERO;
        for ( const auto& v : smoothing[idx] ) {
            tn += v;
        }
        return normalize( tn );
    } else {
        return n;
    }
}

void Topology::addTriangle( uint32_t a, uint32_t b, uint32_t c ) {
    triangles.emplace_back( a );
    triangles.emplace_back( b );
    triangles.emplace_back( c );
    Vector3f n = normalize( crossProduct( vertices[a], vertices[c], vertices[b] ) );
    vertexNormals.emplace_back( n );
    vertexNormals.emplace_back( n );
    vertexNormals.emplace_back( n );
    smoothing[vertices[a].hash()].emplace_back( n );
    smoothing[vertices[b].hash()].emplace_back( n );
    smoothing[vertices[c].hash()].emplace_back( n );
}

void Topology::addQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d ) {
    addTriangle( a, b, c );
    addTriangle( c, b, d );
}

void Topology::addQuadAlt( uint32_t a, uint32_t b, uint32_t c, uint32_t d ) {
    addTriangle( a, b, d );
    addTriangle( b, c, d );
}

void Topology::clear() {
    vertices.clear();
    triangles.clear();
}

double Topology::distance( const Vector3f& p, uint32_t tidX ) const {
    const uint32_t idx0 = triangles[tidX];
    const uint32_t idx1 = triangles[tidX + 1];
    const uint32_t idx2 = triangles[tidX + 2];
    const Vector3f v0 = vertices[idx0];
    const Vector3f v1 = vertices[idx1];
    const Vector3f v2 = vertices[idx2];
    const Vector3f bv = v0;
    const Vector3f e0 = v1 - v0;
    const Vector3f e1 = v2 - v0;
    const Vector3f dv = bv - p;
    const double a = dot( e0, e0 );
    const double b = dot( e0, e1 );
    const double c = dot( e1, e1 );
    const double d = dot( e0, dv );
    const double e = dot( e1, dv );
    //const double f = dot( dv, dv );

    const double det = a * c - b * b;
    double s = b * e - c * d;
    double t = b * d - a * e;

    if ( s + t <= det ) {
        if ( s < 0.0 ) {
            if ( t < 0.0 ) {
                // region 4
                if ( d < 0.0 ) {
                    t = 0.0;
                    s = -d >= a ? 1.0 : -d / a;
                } else {
                    s = 0.0;
                    t = e >= 0.0 ? 0.0 : ( -e >= c ? 1.0 : -e / c );
                }
            } else {
                // region 3
                s = 0.0;
                t = e >= 0.0 ? 0.0 : ( -e >= c ? 1.0 : -e / c );
            }
        } else if ( t < 0.0 ) {
            // region 5
            s = d >= 0.0 ? 0.0 : ( -d >= a ? 1.0 : -d / a );
            t = 0.0;
        } else {
            // region 0
            const double invDet = 1.0 / det;
            s *= invDet;
            t *= invDet;
        }
    } else {
        if ( s < 0.0 ) {
            // region 2
            const double tmp0 = b + d;
            const double tmp1 = c + e;
            if ( tmp1 > tmp0 ) {
                const double numer = tmp1 - tmp0;
                const double denom = a - 2.0 * b + c;
                s = numer >= denom ? 1.0 : numer / denom;
                t = 1.0 - s;
            } else {
                s = 0.0;
                t = ( tmp1 <= 0.0 ? 1.0 : ( e >= 0.0 ? 0.0 : -e / c ));
            }
        } else if ( t < 0.0 ) {
            // region 6
            const double tmp0 = b + e;
            const double tmp1 = a + d;
            if ( tmp1 > tmp0 ) {
                const double numer = tmp1 - tmp0;
                const double denom = a - 2.0 * b + c;
                t = numer >= denom ? 1.0 : numer / denom;
                s = 1.0 - t;
            } else {
                s = ( tmp1 <= 0.0 ? 1.0 : ( d >= 0.0 ? 0.0 : -d / a ));
                t = 0.0;
            }
        } else {
            // region 1
            const double numer = c + e - b - d;
            if ( numer <= 0 ) {
                s = 0.0;
            } else {
                const double denom = a - 2.0 * b + c;
                s = numer >= denom ? 1.0 : numer / denom;
            }
            t = 1.0 - s;
        }
    }

    return length( p - ( v0 + Vector3f( s ) * e0 + Vector3f( t ) * e1 ));
}

double Topology::distance( const Vector3f& p ) const {
    double min = 10e10;
    for ( uint32_t i = 0; i < triangles.size(); i += 3 ) {
        min = std::fmin( min, distance( p, i ));
    }
    return min;
}

[[maybe_unused]] void Topology::addVertex( const V3f& _vertex ) {
    vertices.emplace_back(_vertex);
}

[[maybe_unused]] void Topology::addVertex( const V3f& _vertex, const V2f& _uv ) {
    vertices.emplace_back(_vertex);
    vertexUVs.emplace_back(V4f{_uv, V2fc::ZERO});
}

[[maybe_unused]] void Topology::addVertex( const V3f& _vertex, const V4f& _uv ) {
    vertices.emplace_back(_vertex);
    vertexUVs.emplace_back(_uv);
}

[[maybe_unused]] void Topology::addVertex( const V3f& _vertex, const V4f& _uv, const C4f& _color ) {
    vertices.emplace_back(_vertex);
    vertexUVs.emplace_back(_uv);
    colors.emplace_back(_color);
}

[[maybe_unused]] void Topology::addVertexOfTriangle( const V3f& _vertex, const V4f& _uv, const C4f& _color ) {
    vertices.emplace_back(_vertex);
    vertexUVs.emplace_back(_uv);
    colors.emplace_back(_color);
    triangles.emplace_back( triangles.size() );
    if ( triangles.size() % 3 == 0 && !triangles.empty() ) {
        auto a = triangles.size()-3;
        auto b = triangles.size()-2;
        auto c = triangles.size()-1;
        Vector3f n = normalize( crossProduct( vertices[a], vertices[c], vertices[b] ) );
        vertexNormals.emplace_back( n );
        vertexNormals.emplace_back( n );
        vertexNormals.emplace_back( n );
//        smoothing[vertices[a].hash()].emplace_back( n );
//        smoothing[vertices[b].hash()].emplace_back( n );
//        smoothing[vertices[c].hash()].emplace_back( n );
    }
}

ShapeType shapeTypeFromString( const std::string& value ) {

    if ( toLower(value) == "cube" ) return ShapeType::Cube;
    if ( toLower(value) == "sphere" ) return ShapeType::Sphere;

    return ShapeType::None;
}

std::string shapeTypeToString( const ShapeType value ) {

    switch (value) {
        case ShapeType::None:
            return "None";
        case ShapeType::Sphere:
            return "Sphere";
        case ShapeType::Cylinder:
            return "Cylinder";
        case ShapeType::Pyramid:
            return "Pyramid";
        case ShapeType::Cube:
            return "Cube";
        case ShapeType::RoundedCube:
            return "RoundedCube";
        case ShapeType::Panel:
            return "Panel";
        case ShapeType::Pillow:
            return "Pillow";
        case ShapeType::Arrow:
            return "Arrow";
        case ShapeType::AABB:
            return "AABB";
    }

    return {};
}

uint32_t subdivideEdge( uint32_t f0, uint32_t f1, const Vector3f& v0, const Vector3f& v1, Topology& io_mesh,
                        std::map<Edge, uint32_t>& io_divisions ) {
    const Edge edge( f0, f1 );
    auto it = io_divisions.find( edge );
    if ( it != io_divisions.end()) {
        return it->second;
    }

    const Vector3f v = normalize( Vector3f( 0.5 ) * ( v0 + v1 ));
    const auto f = static_cast<uint32_t>(io_mesh.vertices.size());
    io_mesh.vertices.emplace_back( v );
    io_divisions.emplace( edge, f );
    return f;
}

Topology SubdivideMesh( const Topology& meshIn ) {
    Topology meshOut;
    meshOut.vertices = meshIn.vertices;

    std::map<Edge, uint32_t> divisions; // Edge -> new vertex

    for ( uint32_t i = 0; i < meshIn.triangleCount(); ++i ) {
        const uint32_t f0 = meshIn.triangles[i * 3];
        const uint32_t f1 = meshIn.triangles[i * 3 + 1];
        const uint32_t f2 = meshIn.triangles[i * 3 + 2];

        const Vector3f v0 = meshIn.vertices[f0];
        const Vector3f v1 = meshIn.vertices[f1];
        const Vector3f v2 = meshIn.vertices[f2];

        const uint32_t f3 = subdivideEdge( f0, f1, v0, v1, meshOut, divisions );
        const uint32_t f4 = subdivideEdge( f1, f2, v1, v2, meshOut, divisions );
        const uint32_t f5 = subdivideEdge( f2, f0, v2, v0, meshOut, divisions );

        meshOut.addTriangle( f0, f3, f5 );
        meshOut.addTriangle( f3, f1, f4 );
        meshOut.addTriangle( f4, f2, f5 );
        meshOut.addTriangle( f3, f4, f5 );
    }
    return meshOut;
}

[[maybe_unused]] void Icosahedron( Topology& mesh ) {
    const double t = ( 1.0 + std::sqrt( 5.0 )) / 2.0;

    // Vertices
    mesh.vertices.emplace_back( normalize( Vector3f( -1.0, t, 0.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( 1.0, t, 0.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( -1.0, -t, 0.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( 1.0, -t, 0.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( 0.0, -1.0, t )));
    mesh.vertices.emplace_back( normalize( Vector3f( 0.0, 1.0, t )));
    mesh.vertices.emplace_back( normalize( Vector3f( 0.0, -1.0, -t )));
    mesh.vertices.emplace_back( normalize( Vector3f( 0.0, 1.0, -t )));
    mesh.vertices.emplace_back( normalize( Vector3f( t, 0.0, -1.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( t, 0.0, 1.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( -t, 0.0, -1.0 )));
    mesh.vertices.emplace_back( normalize( Vector3f( -t, 0.0, 1.0 )));

    // Faces
    mesh.addTriangle( 0, 11, 5 );
    mesh.addTriangle( 0, 5, 1 );
    mesh.addTriangle( 0, 1, 7 );
    mesh.addTriangle( 0, 7, 10 );
    mesh.addTriangle( 0, 10, 11 );
    mesh.addTriangle( 1, 5, 9 );
    mesh.addTriangle( 5, 11, 4 );
    mesh.addTriangle( 11, 10, 2 );
    mesh.addTriangle( 10, 7, 6 );
    mesh.addTriangle( 7, 1, 8 );
    mesh.addTriangle( 3, 9, 4 );
    mesh.addTriangle( 3, 4, 2 );
    mesh.addTriangle( 3, 2, 6 );
    mesh.addTriangle( 3, 6, 8 );
    mesh.addTriangle( 3, 8, 9 );
    mesh.addTriangle( 4, 9, 5 );
    mesh.addTriangle( 2, 4, 11 );
    mesh.addTriangle( 6, 2, 10 );
    mesh.addTriangle( 8, 6, 7 );
    mesh.addTriangle( 9, 8, 1 );
}

void UVSphere( Topology& mesh ) {

    uint32_t meridians = 40;
    uint32_t parallels = 40;

    mesh.vertices.emplace_back(V3fc::Y_AXIS*0.5f);
    for (uint32_t j = 0; j < parallels - 1; ++j)
    {
        double const polar = M_PI * double(j+1) / double(parallels);
        double const sp = std::sin(polar);
        double const cp = std::cos(polar);
        for (uint32_t i = 0; i < meridians; ++i)
        {
            double const azimuth = 2.0 * M_PI * double(i) / double(meridians);
            double const sa = std::sin(azimuth);
            double const ca = std::cos(azimuth);
            double const x = sp * ca;
            double const y = cp;
            double const z = sp * sa;
            mesh.vertices.emplace_back( V3f{x, y, z}*0.5f );
        }
    }
    mesh.vertices.emplace_back(V3fc::Y_AXIS_NEG*0.5f);

    for (uint32_t i = 0; i < meridians; ++i)
    {
        uint32_t const a = i + 1;
        uint32_t const b = (i + 1) % meridians + 1;
        mesh.addTriangle(0, b, a);
    }

    for (uint32_t j = 0; j < parallels - 2; ++j)
    {
        uint32_t aStart = j * meridians + 1;
        uint32_t bStart = (j + 1) * meridians + 1;
        for (uint32_t i = 0; i < meridians; ++i)
        {
            const uint32_t a = aStart + i;
            const uint32_t a1 = aStart + (i + 1) % meridians;
            const uint32_t b = bStart + i;
            const uint32_t b1 = bStart + (i + 1) % meridians;
            mesh.addQuadAlt(a, a1, b1, b);
        }
    }

    for (uint32_t i = 0; i < meridians; ++i)
    {
        uint32_t const a = i + meridians * (parallels - 2) + 1;
        uint32_t const b = (i + 1) % meridians + meridians * (parallels - 2) + 1;
        mesh.addTriangle(mesh.vertices.size() - 1, a, b);
    }

}

void AxisAlignedBoundingBox( Topology& mesh, const AABB& aabb ) {
    // Vertices

    mesh.vertices.emplace_back( aabb.centre() + V3f( -0.500000*aabb.calcWidth(), -0.500000*aabb.calcHeight(),  0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( 0.500000 *aabb.calcWidth(), -0.500000*aabb.calcHeight(),  0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( -0.500000*aabb.calcWidth(), 0.500000 *aabb.calcHeight(),  0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( 0.500000 *aabb.calcWidth(), 0.500000 *aabb.calcHeight(),  0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( -0.500000*aabb.calcWidth(), 0.500000 *aabb.calcHeight(), -0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( 0.500000 *aabb.calcWidth(), 0.500000 *aabb.calcHeight(), -0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( -0.500000*aabb.calcWidth(), -0.500000*aabb.calcHeight(), -0.500000 * aabb.calcDepth() ));
    mesh.vertices.emplace_back( aabb.centre() + V3f( 0.500000 *aabb.calcWidth(), -0.500000*aabb.calcHeight(), -0.500000 * aabb.calcDepth() ));

    // Faces

    // bottom
    mesh.addTriangle( 0, 1, 2 );
    mesh.addTriangle( 2, 1, 3 );
    // top
    mesh.addTriangle( 2, 3, 4 );
    mesh.addTriangle( 4, 3, 5 );
    // right
    mesh.addTriangle( 4, 5, 6 );
    mesh.addTriangle( 6, 5, 7 );
    // left
    mesh.addTriangle( 6, 7, 0 );
    mesh.addTriangle( 0, 7, 1 );
    // front
    mesh.addTriangle( 1, 7, 3 );
    mesh.addTriangle( 3, 7, 5 );
    // back
    mesh.addTriangle( 6, 0, 4 );
    mesh.addTriangle( 4, 0, 2 );
}

void Cube( Topology& mesh ) {
    // Vertices

    mesh.vertices.emplace_back( Vector3f( -0.500000, -0.500000, 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , -0.500000, 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( -0.500000, 0.500000 , 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , 0.500000 , 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( -0.500000, 0.500000 , -0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , 0.500000 , -0.500000 ));
    mesh.vertices.emplace_back( Vector3f( -0.500000, -0.500000, -0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , -0.500000, -0.500000 ));

    // Faces

    // bottom
    mesh.addTriangle( 0, 1, 2 );
    mesh.addTriangle( 2, 1, 3 );
    // top
    mesh.addTriangle( 2, 3, 4 );
    mesh.addTriangle( 4, 3, 5 );
    // right
    mesh.addTriangle( 4, 5, 6 );
    mesh.addTriangle( 6, 5, 7 );
    // left
    mesh.addTriangle( 6, 7, 0 );
    mesh.addTriangle( 0, 7, 1 );
    // front
    mesh.addTriangle( 1, 7, 3 );
    mesh.addTriangle( 3, 7, 5 );
    // back
    mesh.addTriangle( 6, 0, 4 );
    mesh.addTriangle( 4, 0, 2 );
}

void Panel( Topology& mesh ) {
    // Vertices

    mesh.vertices.emplace_back( -0.500000, -0.500000, 0.000000 );
    mesh.vertices.emplace_back( 0.500000 , -0.500000, 0.000000 );
    mesh.vertices.emplace_back( -0.500000, 0.500000 , 0.000000 );
    mesh.vertices.emplace_back( 0.500000 , 0.500000 , 0.000000 );

    // Faces
    mesh.addQuad(1, 0, 3, 2);
}

void Cylinder( Topology& mesh, int edges ) {
    // Vertices

    std::vector<float> angles;
    angles.reserve( edges );
    float inc = 360.0f / static_cast<float>(edges);
    float angle = 0.0f;
    for ( int t = 0; t < edges; t++ ) {
        angles.emplace_back( angle );
        angle+=inc;
    }

    for ( const auto a : angles ) {
        auto a1 = degToRad(a);
        mesh.vertices.emplace_back( V3f( cosf(a1), -0.5f, sinf(a1) ) );
    }
    for ( const auto a : angles ) {
        auto a1 = degToRad(a);
        mesh.vertices.emplace_back( V3f( cosf(a1), 0.5f, sinf(a1) ) );
    }

    // Faces

    // bottom
    for ( int t = 1; t < edges-1; t++ ) {
        mesh.addTriangle( 0, t, t+1 );
    }
    // top
    for ( int t = 1; t < edges-1; t++ ) {
        mesh.addTriangle( edges+t, edges, edges+t+1 );
    }

    // sizes
    auto st = static_cast<uint32_t >( angles.size() );
    for ( uint32_t i = 0; i < st-1; i++ ) {
        mesh.addQuad( i+1,    i+0,
                      i+st+1, i+st+0 );
    }
    mesh.addQuad( 0, st-1,
                  st, st*2-1 );
}


void addPillowSide( Topology& mesh, uint32_t subdivs, uint32_t q, uint32_t a, uint32_t b, uint32_t c, uint32_t d ) {
    mesh.addQuad( a, b, q+1, q );
    uint32_t i =0;
    for ( ; i < subdivs-1; i+=2 ) {
        mesh.addQuad( i+q+1, i+q, i+q+3, i+q+2 );
    }
    mesh.addQuad( i+q+1, i+q, c, d );
}

void subdivPillowSide( Topology& mesh, int subdivs, float radius, const Vector3f& p1, const Vector3f& p2,
                       const Vector3f& m1, const Vector3f& m2 ) {
    float deltaI = (1.0f / static_cast<float>(subdivs));
    float deltaC = 0.0f;
    for ( int t = 1; t < subdivs; t++ ) {
        deltaC += deltaI;
        float delta = sin(deltaC * M_PI);
        Vector3f n1 = p1 * V3fc::MASK_Y_OUT + m1 * delta * radius;
        Vector3f n2 = p2 * V3fc::MASK_Y_OUT + m2 * delta * radius;
        mesh.vertices.emplace_back( n1 + V3fc::Y_AXIS*(0.5f-deltaC) );
        mesh.vertices.emplace_back( n2 + V3fc::Y_AXIS*(0.5f-deltaC) );
    }
}

void Pillow( Topology& mesh, uint32_t subdivs, float radius ) {
    // Vertices

    mesh.vertices.emplace_back( Vector3f( -0.500000, 0.500000 , -0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , 0.500000 , -0.500000 ));
    mesh.vertices.emplace_back( Vector3f( -0.500000, 0.500000 , 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , 0.500000 , 0.500000 ));

    mesh.vertices.emplace_back( Vector3f( -0.500000, -0.500000, 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , -0.500000, 0.500000 ));
    mesh.vertices.emplace_back( Vector3f( -0.500000, -0.500000, -0.500000 ));
    mesh.vertices.emplace_back( Vector3f( 0.500000 , -0.500000, -0.500000 ));

    // Vertices for the lateral bumps
    subdivs = 4;
    uint32_t subDivInc = (subdivs - 1) * 2;
    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[2], mesh.vertices[3], Vector3f{-1.0f, 0.0f, 1.0f}, Vector3f{1.0f, 0.0f, 1.0f} );
    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[3], mesh.vertices[1], Vector3f{1.0f, 0.0f, 1.0f}, Vector3f{1.0f, 0.0f, -1.0f} );
    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[1], mesh.vertices[0], Vector3f{1.0f, 0.0f, -1.0f}, Vector3f{-1.0f, 0.0f, -1.0f} );
    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[0], mesh.vertices[2], Vector3f{-1.0f, 0.0f, -1.0f}, Vector3f{-1.0f, 0.0f, 1.0f} );
    // Facesradius,

    // top
    mesh.addQuad( 2, 3, 0, 1 );
    // bottom
    mesh.addQuad( 6, 7, 4, 5 );

    // Side1
    addPillowSide( mesh, subdivs, 8, 3, 2, 5, 4 );
    addPillowSide( mesh, subdivs, 8+subDivInc, 1, 3, 7, 5 );
    addPillowSide( mesh, subdivs, 8+subDivInc*2, 0, 1, 6, 7 );
    addPillowSide( mesh, subdivs, 8+subDivInc*3, 2, 0, 4, 6 );
}

void addTopQuad( Topology& mesh, const V3f& sz ) {
    mesh.vertices.emplace_back( -sz.x(),  sz.y() , -sz.z()  );
    mesh.vertices.emplace_back(  sz.x() , sz.y() , -sz.z()  );
    mesh.vertices.emplace_back( -sz.x(),  sz.y() ,  sz.z()  );
    mesh.vertices.emplace_back(  sz.x() , sz.y() ,  sz.z()  );
}

void addBottomQuad( Topology& mesh, const V3f& sz ) {
    mesh.vertices.emplace_back( -sz.x(),  -sz.y() , sz.z()  );
    mesh.vertices.emplace_back(  sz.x() , -sz.y() , sz.z()  );
    mesh.vertices.emplace_back( -sz.x(),  -sz.y() , -sz.z()  );
    mesh.vertices.emplace_back(  sz.x() , -sz.y() , -sz.z()  );
}

void addLeftQuad( Topology& mesh, const V3f& sz ) {
    mesh.vertices.emplace_back( -sz.x(),  -sz.y() , -sz.z()  );
    mesh.vertices.emplace_back( -sz.x() ,  sz.y() , -sz.z()  );
    mesh.vertices.emplace_back( -sz.x(),  -sz.y() ,  sz.z()  );
    mesh.vertices.emplace_back( -sz.x() ,  sz.y() ,  sz.z()  );
}

void addRightQuad( Topology& mesh, const V3f& sz ) {
    mesh.vertices.emplace_back( sz.x(),  -sz.y() ,  sz.z()  );
    mesh.vertices.emplace_back( sz.x() ,  sz.y() ,  sz.z()  );
    mesh.vertices.emplace_back( sz.x(),  -sz.y() , -sz.z()  );
    mesh.vertices.emplace_back( sz.x() ,  sz.y() , -sz.z()  );
}

void addFrontQuad( Topology& mesh, const V3f& sz ) {
    mesh.vertices.emplace_back( -sz.x(),  -sz.y() , sz.z()  );
    mesh.vertices.emplace_back( -sz.x() ,  sz.y() , sz.z()  );
    mesh.vertices.emplace_back(  sz.x(),  -sz.y() , sz.z()  );
    mesh.vertices.emplace_back(  sz.x() ,  sz.y() , sz.z()  );
}

void addBackQuad( Topology& mesh, const V3f& sz ) {
    mesh.vertices.emplace_back(  sz.x(),  -sz.y() , -sz.z()  );
    mesh.vertices.emplace_back(  sz.x() ,  sz.y() , -sz.z()  );
    mesh.vertices.emplace_back( -sz.x(),  -sz.y() , -sz.z()  );
    mesh.vertices.emplace_back( -sz.x() ,  sz.y() , -sz.z()  );
}

void RoundedCube( Topology& mesh, [[maybe_unused]] uint32_t subdivs, float radius ) {
    // Vertices

    float quadLength = 0.5f * (1.0f - radius);
    addTopQuad( mesh, V3f{quadLength, 0.5f, quadLength} );
    addBottomQuad( mesh, V3f{quadLength, 0.5f, quadLength} );

    addLeftQuad( mesh, V3f{0.5f, quadLength, quadLength} );
    addRightQuad( mesh, V3f{0.5f, quadLength, quadLength} );

    addFrontQuad( mesh, V3f{quadLength, quadLength, 0.5f} );
    addBackQuad( mesh, V3f{quadLength, quadLength, 0.5f} );

    // Vertices for the lateral bumps
//    subdivs = 4;
//    uint32_t subDivInc = (subdivs - 1) * 2;
//    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[2], mesh.vertices[3], Vector3f{-1.0f, 0.0f, 1.0f}, Vector3f{1.0f, 0.0f, 1.0f} );
//    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[3], mesh.vertices[1], Vector3f{1.0f, 0.0f, 1.0f}, Vector3f{1.0f, 0.0f, -1.0f} );
//    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[1], mesh.vertices[0], Vector3f{1.0f, 0.0f, -1.0f}, Vector3f{-1.0f, 0.0f, -1.0f} );
//    subdivPillowSide( mesh, subdivs, radius, mesh.vertices[0], mesh.vertices[2], Vector3f{-1.0f, 0.0f, -1.0f}, Vector3f{-1.0f, 0.0f, 1.0f} );
//    // Facesradius,
//
    // top
    mesh.addQuad( 2, 3, 0, 1 );
    // bottom
    mesh.addQuad( 6, 7, 4, 5 );
    // left
    mesh.addQuad( 10, 11, 8, 9 );
    // right
    mesh.addQuad( 14, 15, 12, 13 );
    // front
    mesh.addQuad( 18, 19, 16, 17 );
    // back
    mesh.addQuad( 22, 23, 20, 21 );

//
//    // Side1
//    addPillowSide( mesh, subdivs, 8, 3, 2, 5, 4 );
//    addPillowSide( mesh, subdivs, 8+subDivInc, 1, 3, 7, 5 );
//    addPillowSide( mesh, subdivs, 8+subDivInc*2, 0, 1, 6, 7 );
//    addPillowSide( mesh, subdivs, 8+subDivInc*3, 2, 0, 4, 6 );

}

//void planarMapping( const Vector3f& normal, const Vector3f vs[], Vector2f vtcs[], int numVerts ) {
//    IndexPair pairMapping = normal.dominantPair();
//
//    for ( int t = 0; t < numVerts; t++ ) {
//        Vector2f tm = vs[t].pairMapped( pairMapping );
//        vtcs[t] = tm + mMappingOffset;
//    }
//
//    if ( mMappingDirection == MappingDirection::X_POS ) {
//        for ( uint64_t t = 0; t < numVerts; t++ ) vtcs[t].swizzle( 0, 1 );
//    }
//
//    if ( UnitMapping() ) {
//        float minC = std::numeric_limits<float>::max();
//        float maxC = std::numeric_limits<float>::lowest();
//        for ( uint64_t t = 0; t < numVerts; t++ ) { if ( vtcs[t].x() < minC ) minC = vtcs[t].x(); }
//        for ( uint64_t t = 0; t < numVerts; t++ ) { if ( vtcs[t].x() > maxC ) maxC = vtcs[t].x(); }
//        float mappingLength = maxC - minC;
//        for ( uint64_t t = 0; t < numVerts; t++ ) vtcs[t].setX( ( ( vtcs[t].x() - minC ) / mappingLength ) * uvScaleInv.x() );
//        minC = std::numeric_limits<float>::max();
//        maxC = std::numeric_limits<float>::lowest();
//        for ( uint64_t t = 0; t < numVerts; t++ ) { if ( vtcs[t].y() < minC ) minC = vtcs[t].y(); }
//        for ( uint64_t t = 0; t < numVerts; t++ ) { if ( vtcs[t].y() > maxC ) maxC = vtcs[t].y(); }
//        mappingLength = maxC - minC;
//        for ( uint64_t t = 0; t < numVerts; t++ ) vtcs[t].setY( ( ( vtcs[t].y() - minC ) / mappingLength ) * uvScaleInv.y() );
//    }
//}

//void GetTextureCoord(const Vector3f& pos, float *targetU, float *targetV){
//    float normalisedX = 0;
//    float normalisedZ = -1;
//    if (((normal->x * normal->x) + (normal->z * normal->z)) > 0){
//        normalisedX = sqrt((normal->x * normal->x) / ((normal->x * normal->x) + (normal->z * normal->z)));
//        if (normal->x < 0){
//            normalisedX = -normalisedX;
//        }
//        normalisedZ = sqrt((normal->z * normal->z) / ((normal->x * normal->x) + (normal->z * normal->z)));
//        if (normal->z < 0){
//            normalisedZ = -normalisedZ;
//        }
//    }
//    if (normalisedZ == 0){
//        *targetU = ((normalisedX * PI) / 2);
//    }
//    else {
//        *targetU = atan(normalisedX / normalisedZ);
//        if (normalisedZ < 0){
//            *targetU += PI;
//        }
//        if (*targetU < 0){
//            *targetU += 2 * PI;
//        }
//    }
//    *targetU /= 2 * PI;
//    *targetV = (-normal->y + 1) / 2;
//}

PolyStruct createGeom( Topology& mesh, const Vector3f& size, GeomMapping mt, int subdivs, ReverseFlag rf ) {

    for ( int j = 0; j < subdivs; ++j ) {
        mesh = SubdivideMesh( mesh );
    }

    PolyStruct ret;

    ret.numVerts = mesh.vertexCount();
    ret.numIndices = mesh.indexCount();

    ret.verts = std::make_unique<Vector3f[]>( ret.numVerts );
    ret.indices = std::make_unique<uint32_t[]>( ret.numIndices );
    ret.colors = std::make_unique<Vector4f[]>( ret.numIndices );

    int t = 0;
    for ( auto& v : mesh.vertices ) {
        ret.verts[t++] = v;
        ret.bbox3d.expand( v );
    }
    t = 0;
    for ( auto& tr : mesh.triangles ) {
        ret.indices[t] = tr;
        if ( mesh.colors.empty() ) {
            auto cv = C4fc::WHITE;// (t%3) == 0 ? C4fc::RED : ( (t%3) == 1 ? C4fc::GREEN : C4fc::BLUE );
            ret.colors[t] = cv;
        } else {
            ret.colors[t] = mesh.colors[tr];
        }
        ++t;
    }

    ret.normals = std::make_unique<Vector3f[]>( ret.numIndices );
    ret.tangents = std::make_unique<Vector4f[]>( ret.numIndices );
    ret.binormals = std::make_unique<Vector3f[]>( ret.numIndices );

    for ( int q = 0; q < ret.numIndices; q++ ) {
        ret.tangents[q] = V4fc::ONE;
        ret.binormals[q] = V3fc::ONE;
    }
    ret.uvs = std::make_unique<Vector2f[]>( ret.numIndices );
    ret.uv2s = std::make_unique<Vector2f[]>( ret.numIndices );

    for ( int q = 0; q < ret.numIndices; q++ ) {
        ret.normals[q] = mesh.normalFromSmartSmoothing( mesh.triangles[q], q );
    }
    if ( rf == ReverseFlag::True ) {
        for ( int q = 0; q < ret.numIndices; q++ ) {
            ret.normals[q] *= -1.0f;
        }
    }
//    for ( int q = 0; q < ret.numIndices; q++ ) {
//        ret.normals[q] = normalize( ret.normals[q] );
//    }

    if ( mt.type == GeomMappingT::PreBaked ) {
        for ( int q = 0; q < ret.numIndices; q++ ) {
            ret.uvs[q] = mesh.vertexUVs[ret.indices[q]].xy();
            ret.uv2s[q] = mesh.vertexUVs[ret.indices[q]].zw();
        }
    }

    if ( mt.type == GeomMappingT::Cube ) {
        for ( int q = 0; q < ret.numIndices; q++ ) {
            Vector3f v = ret.verts[ret.indices[q]] * mt.scaling;
            Vector3f n = ret.normals[q];
            ret.uvs[q] = dominantMapping( n, v, size );
        }
    }

    if ( mt.type == GeomMappingT::PlanarNoTile ) {
        const Vector3f off = ret.bbox3d.size() * 0.5f;
        for ( int q = 0; q < ret.numIndices; q++ ) {
            Vector3f v = ret.verts[ret.indices[q]] + off;
            Vector3f n = ret.normals[q];
            ret.uvs[q] = dominantMapping( n, v, size );
        }
    }

    if ( mt.type == GeomMappingT::Spherical ) {

        for ( int q = 0; q < ret.numIndices; q++ ) {
            Vector3f p = ret.verts[ret.indices[q]];
            float m = length( p );
            if ( p.x() == 0.0f && p.z() == 0.0f ) {
                ret.uvs[q] = { 0.0f, acos( p.y()/m ) / M_PI_2 };
            } else {
                ret.uvs[q] = { atan2(p.x(), p.z()), acos( p.y()/m ) };
                ret.uvs[q] += Vector2f{M_PI, 0.0f};
                ret.uvs[q] /= Vector2f{TWO_PI, M_PI_2};
            }
        }

        auto uvCorrection = std::make_unique<Vector2f[]>( ret.numIndices );
        for ( int q = 0; q < ret.numIndices; q+=3 ) {
            auto vi1 = q;
            auto vi2 = q+1;
            auto vi3 = q+2;
            uvCorrection[vi1] = V2fc::ZERO;
            uvCorrection[vi2] = V2fc::ZERO;
            uvCorrection[vi3] = V2fc::ZERO;
            auto uv1 = ret.uvs[vi1];
            auto uv2 = ret.uvs[vi2];
            auto uv3 = ret.uvs[vi3];
            float areaDiv = 0.35f;
            if ( uv2.x() - uv1.x() > areaDiv || uv3.x() - uv1.x() > areaDiv ) {
                uvCorrection[vi1] = V2fc::X_AXIS;
            }
            if ( uv1.x() - uv2.x() > areaDiv || uv3.x() - uv2.x() > areaDiv ) {
                uvCorrection[vi2] = V2fc::X_AXIS;
            }
            if ( uv1.x() - uv3.x() > areaDiv || uv2.x() - uv3.x() > areaDiv ) {
                uvCorrection[vi3] = V2fc::X_AXIS;
            }
        }

        for ( int q = 0; q < ret.numIndices; q++ ) {
            ret.uvs[q] += uvCorrection[q];
        }
    }

    if ( mt.type == GeomMappingT::SphericalUV ) {
        Vector2f uvSphericalNorm{ 3.0f, 1.5f };
        for ( int q = 0; q < ret.numIndices; q++ ) {
            Vector3f p = ret.verts[ret.indices[q]];
            Vector3f uv = cartasianToSpherical(XZY::C(p)) * Vector3f{1.0f/M_PI, 1.0f/M_PI_2, 1.0f};
            ret.uvs[q] = uv.xy();// * Vector2f(2.0f, 1.0f);
            ret.uvs[q] += V2f{1.0f, 0.0f};
            ret.uvs[q] *= uvSphericalNorm;
        }

        int fixeupCount = 0;
        auto uvCorrection = std::make_unique<Vector2f[]>( ret.numIndices );
        for ( int q = 0; q < ret.numIndices; q+=3 ) {
            auto vi1 = q;
            auto vi2 = q+1;
            auto vi3 = q+2;
            uvCorrection[vi1] = V2fc::ZERO;
            uvCorrection[vi2] = V2fc::ZERO;
            uvCorrection[vi3] = V2fc::ZERO;
            auto& uv1 = ret.uvs[vi1];
            auto& uv2 = ret.uvs[vi2];
            auto& uv3 = ret.uvs[vi3];
            float areaDiv = uvSphericalNorm.x();
            if ( fabs(uv2.x() - uv1.x()) > areaDiv ||
                 fabs(uv3.x() - uv1.x()) > areaDiv ||
                 fabs(uv3.x() - uv2.x()) > areaDiv ) {

                if ( uv1.x() == uv3.x() ) {
                    uv2.setX( ( uv1.x() < areaDiv ) ? 0.0f : uv1.x() + uv2.x() );
                } else if ( uv2.x() == uv3.x() ) {
                    uv1.setX( ( uv2.x() < areaDiv ) ? 0.0f : uv2.x() + uv1.x() );
                } else if ( uv2.x() == uv1.x() ) {
                    uv3.setX( ( uv2.x() < areaDiv ) ? 0.0f : uv2.x() + uv3.x() );
                } else {
                    uv2.setX( ( uv3.x() < areaDiv ) ? 0.0f : uv2.x() + uv3.x() );
                }
                ++fixeupCount;
            }
        }
    }

    for ( int q = 0; q < ret.numIndices; q+=3 ) {
        auto i1 = ret.indices[q];
        auto i2 = ret.indices[q+1];
        auto i3 = ret.indices[q+2];
        auto vi1 = q;
        auto vi2 = q+1;
        auto vi3 = q+2;
        tbCalc( ret.verts[i1], ret.verts[i2], ret.verts[i3],
                ret.uvs[vi1], ret.uvs[vi2], ret.uvs[vi3],
                ret.tangents[vi1], ret.tangents[vi2], ret.tangents[vi3],
                ret.binormals[vi1], ret.binormals[vi2], ret.binormals[vi3] );
    }

    return ret;
}

PolyStruct createGeomForSphere( const Vector3f& center, const float diameter, [[maybe_unused]] const int subdivs ) {

    Topology mesh;
//    Icosahedron( mesh );
//    return createGeom( mesh, center, Vector3f{ diameter }, GeomMappingT::Spherical, subdivs );
    UVSphere( mesh );
    return createGeom( mesh, Vector3f{ diameter }, GeomMappingT::SphericalUV, 0 );
}

PolyStruct createGeomForCube( const Vector3f& center, const Vector3f& size ) {

    Topology mesh;
    Cube( mesh );

    return createGeom( mesh, size, GeomMappingT::Cube, 0 );
}

PolyStruct createGeomForAABB( const AABB& aabb ) {

    Topology mesh;
    AxisAlignedBoundingBox( mesh, aabb );

    return createGeom( mesh, V3fc::ONE, GeomMappingT::Cube, 0 );
}

PolyStruct createGeomForPanel( const Vector3f& center, const Vector3f& size ) {

    Topology mesh;
    Panel( mesh );

    return createGeom( mesh, size, GeomMappingT::PlanarNoTile, 0 );
}

PolyStruct createGeomForCylinder( const Vector3f& center, const V2f& size, const int subdivs ) {

    Topology mesh;
    int edges = subdivs * 8;
    Cylinder( mesh, edges );

    return createGeom( mesh, Vector3f{ size.x(), size.y(), size.x() }, GeomMappingT::Cylindrical, 0 );
}

PolyStruct createGeomForPillow( const Vector3f& center, const Vector3f& size, const int subdivs, float radius ) {

    Topology mesh;
    Pillow( mesh, subdivs, radius * size.y() *0.05f );

    return createGeom( mesh, size, GeomMappingT::Cube, 0 );
}

PolyStruct createGeomForRoundedCube( const Vector3f& center, const Vector3f& size, const int subdivs, float radius ) {

    Topology mesh;
    RoundedCube( mesh, subdivs, radius * size.y() );

    return createGeom( mesh, size, GeomMappingT::Cube, 0 );
}
