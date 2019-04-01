//
// Created by Dado on 19/08/2018.
//

#pragma once

#include <vector>
#include <string>
#include <functional>

#include <core/http/curl_utils.h>
#include <core/resources/material.h>

template < typename T >
void findDataEnclosed( const std::string& _curlString, size_t& ps, const std::string& startDelimeter,
                       const std::string& endDelimeter, const std::string& regExpString,
                       const size_t regStringNumMatches, T& _data, std::function<T(const std::smatch&)> _f ) {
    const size_t sl = startDelimeter.length();
    size_t p = _curlString.find( startDelimeter, ps );
    if ( p == std::string::npos ) { ps = p; return; }

    ps = p + sl;
    auto pe = _curlString.find( endDelimeter, ps );
    if ( pe == std::string::npos ) { ps = pe; return; }
    auto rgbCol = _curlString.substr( ps, pe-ps );

    // Regexp of colors
    auto res = regEx( regExpString, rgbCol );
    if ( res.size() == regStringNumMatches ) {
        _data = _f( res );
    }

    ps = pe;
}

void scanValsparCat( const std::string& _curlUrl,
                                                 const std::string& _cat,
                                                 std::vector<MaterialColor>& colors ) {

    auto _curlString = CurlUtil::read( _curlUrl );

    size_t ps = 0;
    while ( ps != std::string::npos ) {
        MaterialColor col{};
        col.brand = "Valspar";
        col.category = _cat;

        findDataEnclosed<Color4f>( _curlString,
                                   ps,
                                   "<div class=colour-category-grid-item style=background-color:",
                                   ">",
                                   "rgb\\((\\d+),(\\d+),(\\d+)\\)", 4, col.color,
                                   []( const std::smatch& res ) -> Color4f {
                                       return Vector4f::ITORGB( std::stoi(res[1].str()),
                                                                std::stoi(res[2].str()),
                                                                std::stoi(res[3].str()) );
                                   } );

        if ( ps == std::string::npos ) break;

        findDataEnclosed<std::string>( _curlString,
                                       ps,
                                       "<b><span>",
                                       "</span></b>",
                                       "(.*)", 2, col.name,
                                       []( const std::smatch& res ) -> std::string {
                                           return htmlDecode( res[1].str() );
                                       } );

        if ( ps == std::string::npos ) break;

        findDataEnclosed<std::string>( _curlString,
                                       ps,
                                       "<b><span>",
                                       "</span></b>",
                                       "(.*)", 2, col.code,
                                       []( const std::smatch& res ) -> std::string {
                                           return htmlDecode( res[1].str() );
                                       } );

        if ( ps == std::string::npos ) break;

        LOGR( "%s ", col.toString().c_str() );

        colors.emplace_back(col);
    }

}

namespace Valspar {
    void scanValsparColors() {
        std::vector<MaterialColor> colors;

        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/reds/", "red", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/oranges/", "orange", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/yellows/", "yellow", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/greens/", "green", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/blues/", "blue", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/purples/", "purple", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/blacks/", "black", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/teals/", "teal", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/greys/", "grey", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/pinks/", "pink", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/creams/", "cream", colors );
//        scanValsparCat( "https://www.valsparpaint.co.uk/colours/pre-selected-colours/whites/", "white", colors );

        for ( const auto& c : colors ) {
            Http::post( Url{ Http::restEntityPrefix( MaterialColor::entityGroup(), c.toURLPathEncoded() ) },
                             c.toMetaData() );
        }
    }
}

