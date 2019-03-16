#include "image_builder.h"
#include "image_util.h"
#include "file_manager.h"
#include "raw_image.h"

//bool ImageManager::add( ImageBuilder& tbd, std::unique_ptr<uint8_t[]>& _data ) {
//    imageParamsMap[tbd.Name()] = tbd.imageParams;
//    return addImpl( tbd, _data );
//}

const ImageParams& ImageManager::ip( const std::string& _key ) {
    return imageParamsMap[_key];
}

bool ImageBuilder::finalizaMake( std::unique_ptr<uint8_t[]>&& decodedData ) {
//    setWidth(imageParams.width).setHeight(imageParams.height).setChannels(imageParams.channels);

    setFormatFromChannels(imageParams.channels);
    mm.add( std::make_shared<RawImage>(Name(), imageParams, std::move(decodedData)) );
    return true;
}

// ### MakeImpl on image builders needs to have proper EF constructor

//bool ImageBuilder::makeImpl( uint8_p&& _data, const DependencyStatus _status ) {
//
//    std::unique_ptr<uint8_t[]> decodedData = ( _status == DependencyStatus::LoadedSuccessfully ) ?
//                  imageUtil::decodeFromMemory( std::move(_data), imageParams.width, imageParams.height, imageParams.channels, imageParams.bpp ) :
//                  imageUtil::zeroImage( backup_color, imageParams.width, imageParams.height, imageParams.channels);
//
//    return finalizaMake( std::move( decodedData ) );
//}

bool ImageBuilder::makeDirect( const ucchar_p& _data ) {

    if ( mm.exists( Name() ) ) return false;

    std::unique_ptr<uint8_t[]> decodedData = imageUtil::decodeFromMemory( _data, imageParams.width, imageParams
    .height, imageParams.channels, imageParams.bpp, mbIsRaw );

    return finalizaMake( std::move( decodedData ) );
}

bool ImageBuilder::makeDirect( const uint8_p& _data ) {
    return makeDirect( ucchar_p{ _data.first.get(), _data.second} );
}

bool ImageBuilder::makeDirect( const RawImage& _data ) {
    mbIsRaw = true;
    imageParams.width = _data.width;
    imageParams.height = _data.height;
    imageParams.channels = _data.channels;
    return makeDirect( ucchar_p{_data.data(), _data.size()} );
}

void ImageBuilder::makeDefault() {

    auto decodedData = imageUtil::memsetImage( backup_color,
                                               imageParams.width,
                                               imageParams.height,
                                               imageParams.channels );

    finalizaMake( std::move( decodedData ) );
}


