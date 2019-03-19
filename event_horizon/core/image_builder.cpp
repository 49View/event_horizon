#include "image_builder.h"
#include <core/image_util.h>
#include <core/file_manager.h>
#include <core/raw_image.h>
#include <core/entity_factory.hpp>

std::shared_ptr<RawImage> ImageBuilder::makeInternal( const SerializableContainer& _data ) {

    auto decodedData = imageUtil::decodeFromMemory(  ucchar_p{ _data.data(), _data.size()},
            imageParams.width, imageParams.height, imageParams.channels, imageParams.bpp, mbIsRaw );

    setFormatFromChannels(imageParams.channels);
    auto res = std::make_shared<RawImage>(Name(), imageParams, std::move(decodedData));
    mm.add( res, Name(), Hash() );

    return res;
}

std::string ImageBuilder::generateThumbnail() const {
    return std::string();
}

void ImageBuilder::serializeInternal( std::shared_ptr<SerializeBin> writer ) const {

}

void ImageBuilder::deserializeInternal( std::shared_ptr<DeserializeBin> reader ) {

}
