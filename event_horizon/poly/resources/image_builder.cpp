#include "image_builder.h"
#include <core/image_util.h>
#include <core/raw_image.h>

void ImageBuilder::finalise( std::shared_ptr<RawImage> _elem ) {
    _elem->ttm = imageParams.ttm;
    _elem->wrapMode = imageParams.wrapMode;
    _elem->filterMode = imageParams.filterMode;
}

void ImageBuilder::serializeInternal( std::shared_ptr<SerializeBin> writer ) const {

}

void ImageBuilder::deserializeInternal( std::shared_ptr<DeserializeBin> reader ) {

}
