#include "font_builder.h"

#include "core/math/matrix4f.h"
#include "core/file_manager.h"

//const Utility::TTFCore::Font& FontManager::operator[]( const std::string& _fontName ) {
//    if ( auto ret = Resources().find( _fontName ); ret != Resources().end()) {
//        return *ret->second.get();
//    }
//
//    // didn't find the font, rollback to defualt
//    auto ret = Resources().find( defaultFontName );
//    return *ret->second.get();
//    // ok default it's not loaded, load it
//    //return addFont( FontManager::defaultFont, FontManager::defaultFontName );
//}
//
//const Utility::TTFCore::Font& FontManager::operator()() {
//    return operator[]( defaultFontName );
//}

void FontBuilder::serializeInternal( std::shared_ptr<SerializeBin> writer ) const {

}

void FontBuilder::deserializeInternal( std::shared_ptr<DeserializeBin> reader ) {

}

void FontBuilder::finalise( std::shared_ptr<Utility::TTFCore::Font> _elem ) {

}
