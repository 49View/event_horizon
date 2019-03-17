#include "image_builder.h"
#include <core/image_util.h>
#include <core/file_manager.h>
#include <core/raw_image.h>
#include <core/entity_factory.hpp>

const ImageParams& ImageManager::ip( const std::string& _key ) {
    return imageParamsMap[_key];
}

bool ImageBuilder::finalizaMake( std::unique_ptr<uint8_t[]>&& decodedData ) {
    setFormatFromChannels(imageParams.channels);
    mm.add( std::make_shared<RawImage>(Name(), imageParams, std::move(decodedData)) );
    return true;
}

bool ImageBuilder::makeDirect( const uint8_p& _data ) {
    return makeDirectImpl( ucchar_p{ _data.first.get(), _data.second} );
}

bool ImageBuilder::makeDirect( const SerializableContainer& _data ) {
    return makeDirectImpl( ucchar_p{ _data.data(), _data.size()} );
}

bool ImageBuilder::makeDirectImpl( const ucchar_p& _data ) {

    if ( mm.exists( Name() ) ) return false;

    std::unique_ptr<uint8_t[]> decodedData = imageUtil::decodeFromMemory( _data, imageParams.width, imageParams
            .height, imageParams.channels, imageParams.bpp, mbIsRaw );

    return finalizaMake( std::move( decodedData ) );
}

void ImageBuilder::makeDefault() {

    auto decodedData = imageUtil::memsetImage( backup_color,
                                               imageParams.width,
                                               imageParams.height,
                                               imageParams.channels );

    finalizaMake( std::move( decodedData ) );
}
