#pragma once

#include <poly/resources/resource_builder.hpp>

class RawImage;

//class ImageBuilder : public ResourceBuilder3<ImageBuilder, RawImage> {
//public:
//    using ResourceBuilder3::ResourceBuilder3;
//    ImageParams imageParams;
//
//    ImageBuilder& setImageParams( const ImageParams& _ip ) {
//        imageParams = _ip;
//        return *this;
//    }
//
//    ImageBuilder& setSize( int s ) {
//        imageParams.width = s;
//        imageParams.height = s;
//        return *this;
//    }
//
//    ImageBuilder& setBpp( int s ) {
//        imageParams.bpp = s;
//        return *this;
//    }
//
//    ImageBuilder& size( int _width, int _height ) {
//        imageParams.width = _width;
//        imageParams.height = _height;
//        return *this;
//    }
//
//    ImageBuilder& setWidth( int _width ) {
//        imageParams.width = _width;
//        return *this;
//    }
//
//    ImageBuilder& setHeight( int _height ) {
//        imageParams.height = _height;
//        return *this;
//    }
//
//    ImageBuilder& setChannels( int _channels ) {
//        imageParams.channels = _channels;
//        return *this;
//    }
//
//    ImageBuilder& format( PixelFormat _outFormat ) {
//        imageParams.outFormat = _outFormat;
//        return *this;
//    }
//
//    ImageBuilder& target( TextureTargetMode _ttm ) {
//        imageParams.ttm = _ttm;
//        return *this;
//    }
//
//    ImageBuilder& setWrapMode( WrapMode _wrapMode ) {
//        imageParams.wrapMode = _wrapMode;
//        return *this;
//    }
//
//    ImageBuilder& setFilterMode( Filter _filterMode ) {
//        imageParams.filterMode = _filterMode;
//        return *this;
//    }
//
//};

using ImageBuilder = ResourceBuilder4<RawImage>;
using IB = ImageBuilder;
