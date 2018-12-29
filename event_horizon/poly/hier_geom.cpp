//
//  hier_geom.cpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#include "hier_geom.hpp"


//template<typename D>
//void Hier<D>::extractHier( std::vector<std::shared_ptr<Hier>>& geoms, const char *name, ExtractFlags ef ) {
//    if ( strcmp( mName.c_str(), name ) == 0 ) {
//        geoms.push_back( clone());
//        if ( ef == ExtractFlags::RemoveAfterExtract ) {
//            if ( father != nullptr )
//                father->eraseChildren( name );
//        }
//    }
//
//    for ( auto& c : children ) {
//        c->extractHier( geoms, name, ef );
//    }
//}
//
//template<typename D>
//void Hier<D>::commandReposition( const std::vector<std::string>& itr ) {
//    std::vector<std::string> meshesToReposition;
//
//    auto it = itr.begin();
//    while ( *( it ) != "to" ) {
//        meshesToReposition.push_back( *it );
//        ++it;
//    }
//    std::string dest = *( ++it );
//    Hier *gd = find( dest.c_str());
//    if ( !gd ) return;
//
//    for ( auto mesh : meshesToReposition ) {
//        std::vector<std::shared_ptr<Hier>> geoms;
//        extractHier( geoms, mesh.c_str(), ExtractFlags::RemoveAfterExtract );
//        for ( auto gs : geoms ) {
//            gd->addChildren( gs );
//        }
//    }
//}


//template<typename D>
//void Hier<D>::numVertsRec( int& currNumVerts ) {
//    currNumVerts += mData ? mData->numVerts() : 0;
//
//    for ( auto& c : children ) {
//        c->numVertsRec( currNumVerts );
//    }
//}
//
//template<typename D>
//int Hier<D>::numVerts() {
//    int currNumVerts = 0;
//
//    numVertsRec( currNumVerts );
//
//    return currNumVerts;
//}
