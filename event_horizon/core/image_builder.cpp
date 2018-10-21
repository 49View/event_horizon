#include "image_builder.h"
#include "image_util.h"
#include "file_manager.h"
#include "raw_image.h"

bool ImageDepencencyMaker::add( ImageBuilder& tbd, std::unique_ptr<uint8_t[]>& _data ) {
    imageList.insert( tbd.Name() );
    imageParamsMap[tbd.Name()] = tbd.imageParams;
    return addImpl( tbd, _data );
}

const ImageParams& ImageDepencencyMaker::ip( const std::string& _key ) {
    return imageParamsMap[_key];
}

bool ImageBuilder::finalizaMake( ImageDepencencyMaker& sg, std::unique_ptr<uint8_t[]>&& decodedData ) {
    setFormatFromChannels(imageParams.channels);
    setWidth(imageParams.width).setHeight(imageParams.height).setChannels(imageParams.channels);

    return sg.add( *this, decodedData );
}

bool ImageBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    std::unique_ptr<uint8_t[]> decodedData = ( _status == DependencyStatus::LoadedSuccessfully ) ?
                  imageUtil::decodeFromMemory( std::move(_data), imageParams.width, imageParams.height, imageParams.channels, imageParams.bpp ) :
                  imageUtil::zeroImage( backup_color, imageParams.width, imageParams.height, imageParams.channels);

    return finalizaMake( static_cast<ImageDepencencyMaker&>(_md), std::move( decodedData ) );
}

bool ImageBuilder::makeDirect( DependencyMaker& _md, const ucchar_p& _data ) {

    if ( _md.exists( Name() ) ) return false;

    std::unique_ptr<uint8_t[]> decodedData = imageUtil::decodeFromMemory( _data, imageParams.width, imageParams
    .height, imageParams.channels, imageParams.bpp, mbIsRaw );

    return finalizaMake( static_cast<ImageDepencencyMaker&>(_md), std::move( decodedData ) );
}

bool ImageBuilder::makeDirect( DependencyMaker& _md, const uint8_p& _data ) {
    return makeDirect( _md, ucchar_p{ _data.first.get(), _data.second} );
}

bool ImageBuilder::makeDirect( DependencyMaker& _md, const RawImage& _data ) {
    mbIsRaw = true;
    return makeDirect( _md, {_data.data(), _data.size()} );
}

bool ImageBuilder::makeDefault( DependencyMaker& _md ) {

    if ( _md.exists( Name() ) ) return false;

    auto decodedData = imageUtil::zeroImage( backup_color, imageParams.width, imageParams.height, imageParams.channels);

    return finalizaMake( static_cast<ImageDepencencyMaker&>(_md), std::move( decodedData ) );
}


