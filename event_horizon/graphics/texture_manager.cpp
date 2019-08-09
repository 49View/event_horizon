#include "texture_manager.h"

#include <stb/stb_image.h>

#include <core/raw_image.h>

#include "texture_converter.h"
#include "renderer.h"
#include "graphic_functions.hpp"

void TextureManager::removeTexture( const std::string& id ) {
    auto it = mData.find( id );
    if ( it != mData.end()) {
        //		GLuint tid = ( it->second->getHandle() );
        //		GLCALL( glDeleteTextures( 1, &tid ) );
        mData.erase( it );
    }
}

std::shared_ptr<Texture> TextureManager::createTexture( TextureRenderData& tb ) {
    auto ltexture = std::make_shared<Texture>( tb );

    add( tb.names, ltexture );

    return ltexture;
}

std::shared_ptr<Texture> TextureManager::createTexture( const std::string& _name, unsigned int _gpuHandle,
                                                        TextureSlots _gpuSecondaryHandle ) {
    TextureRenderData tb{ {_name} };
    tb.GPUId( _gpuHandle );
    tb.GPUSlot( _gpuSecondaryHandle );
    return createTexture( tb );
}

std::shared_ptr<Texture> TextureManager::addTextureFromCallback( TextureRenderData& tb,
                                                                 std::unique_ptr<uint8_t []>& _data ) {
    auto ret = createTexture(tb);
    ret->init_r( _data.get() );
    return ret;
}

std::shared_ptr<Texture> TextureManager::addTextureImmediate( TextureRenderData& tb, const uint8_t* _data ) {
    auto ret = createTexture(tb);
    ret->init_r(_data);
    return ret;
}

std::shared_ptr<Texture> TextureManager::addCubemapTexture( TextureRenderData& tb ) {
    tb.target(TEXTURE_CUBE_MAP);

    auto ret = createTexture(tb);
    ret->init_cubemap_r();

    return ret;
}

std::shared_ptr<Texture> TextureManager::addTextureRef( const std::string& _name ) {
    TextureRenderData tb{_name};
    auto ltexture = std::make_shared<Texture>( tb );

    add( tb.names, ltexture );
    ltexture->init_null();

    return ltexture;
}

std::shared_ptr<Texture> TextureManager::addTextureNoData( TextureRenderData& tb ) {
    std::unique_ptr<uint8_t []> _data;
    return addTextureImmediate( tb, _data.get() );
}

std::shared_ptr<Texture> TextureManager::addTextureWithData( const RawImage& rawImage,
                                                             const std::string& _name,
                                                             TextureSlots _tslot ) {
    return addTextureWithData( rawImage, StringUniqueCollection{ _name }, _tslot );
}

std::shared_ptr<Texture> TextureManager::addTextureWithData( const RawImage& rawImage,
                                                             const StringUniqueCollection& _names,
                                                             TextureSlots _tslot ) {
    auto trd = ImageParams{}.size( rawImage.width, rawImage.height, rawImage.depth ).format(rawImage.outFormat).setBpp(rawImage.bpp).target(rawImage.ttm).setWrapMode(rawImage.wrapMode);
    auto tb = TextureRenderData{ _names, trd }.GPUSlot(_tslot);
    if ( rawImage.ttm == TextureTargetMode::TEXTURE_3D ) tb.setGenerateMipMaps(false);
    tb.format( channelsToFormat( rawImage.channels, rawImage.bpp ) );

    for ( const auto& _name : _names ) {
        if ( mData.find( _name ) != mData.end() ) {
            removeTexture( _name );
        }
    }
    return addTextureImmediate( tb, rawImage.rawBtyes.get() );
}

std::shared_ptr<Texture> TextureManager::addTextureWithGPUHandle( const std::string& id, unsigned int _handle,
                                                                  unsigned int _secondaryHandle ) {

    if ( mData.find( id ) != mData.end() ) {
        removeTexture( id );
    }

    auto ret = createTexture( id, _handle, static_cast<TextureSlots>(_secondaryHandle) );
    ret->setFilter( FILTER_NEAREST );
    ret->init_r( nullptr );
    return ret;
}

void TextureManager::preparingStremingTexture( const std::string& _streamName, const V2i& sd ) {
    V2i sd2 = sd / 2;
    TextureRenderData sdty { _streamName + "_y", sd.x(), sd.y(),   1, 8 };
    TextureRenderData sdtu { _streamName + "_u", sd2.x(), sd2.y(), 1, 8 };
    TextureRenderData sdtv { _streamName + "_v", sd2.x(), sd2.y(), 1, 8 };

    sdty.generateMipMaps = false;
    sdtu.generateMipMaps = false;
    sdtv.generateMipMaps = false;

    addTextureNoData( sdty );
    addTextureNoData( sdtu );
    addTextureNoData( sdtv );
}

void TextureManager::updateTexture( const RawImage& _image, const std::string& _name ) {
    if ( auto it = mData.find( _name ); it != mData.end()) {
        Texture * toBeUpdated = it->second.get();
        toBeUpdated->refresh( _image.data(), 0, 0, toBeUpdated->getWidth(), toBeUpdated->getHeight());
    } else {
        addTextureWithData( _image, _name );
    }

}

void TextureManager::updateTexture( const std::string& id, const uint8_t *data ) {
    if ( auto it = mData.find( id ); it != mData.end()) {
        Texture * toBeUpdated = it->second.get();
        toBeUpdated->refresh( data, 0, 0, toBeUpdated->getWidth(), toBeUpdated->getHeight());
    }
}

void TextureManager::updateTexture( const std::string& id, const uint8_t *data, int width, int height ) {
    auto it = mData.find( id );
    if ( mData.find( id ) == mData.end()) {
        ASSERT( false );
    }
    Texture * toBeUpdated = it->second.get();
    toBeUpdated->refresh( data, 0, 0, width, height );
}

void TextureManager::updateTexture( const std::string& id, uint8_t *data, int width, int height, PixelFormat inFormat,
                                    PixelFormat outFormat ) {
    auto it = mData.find( id );
    if ( mData.find( id ) == mData.end()) {
        LOGE( "Texture not in the manager, skipping it: %s", id.c_str());
        return;
    }

    Texture * toBeUpdated = it->second.get();

    if ( outFormat == PIXEL_FORMAT_RGBA_QUADRANTS ) {
        // Special case, this is a quadrants texture
        int dataSize = ( width / 2 ) * ( height / 2 ) * 4;
        uint8_t *outData = new uint8_t[dataSize];

        LOGI( "Updating texture: (%s) %dx%d %s -> %s", id.c_str(), width, height, pixelFormatToString( inFormat ),
              "RGBA (quadrants)" );
        convertToRgbaQuadrants( data, outData, width, height, inFormat, outFormat );

        toBeUpdated->refresh( outData, 0, 0, width / 2, height / 2 );
        delete[] outData;
    } else {
        // Normal case, all other textures
        int dataSize = width * height * getPixelFormatChannels( outFormat );
        uint8_t *outData = new uint8_t[dataSize];

        LOGI( "Updating texture: (%s) %dx%d %s -> %s", id.c_str(), width, height, pixelFormatToString( inFormat ),
              pixelFormatToString( outFormat ));
        convertPixelFormat( data, outData, width, height, inFormat, outFormat );

        toBeUpdated->refresh( outData, 0, 0, width, height );
        delete[] outData;
    }
}

void
TextureManager::convertToRgbaQuadrants( uint8_t *data, uint8_t *outData, int width, int height, PixelFormat inFormat,
                                        PixelFormat /*outFormat*/ ) {
    //	ASSERT( outFormat == PIXEL_FORMAT_RGBA_QUADRANTS );
    if ( inFormat == PIXEL_FORMAT_LUMINANCE ) {
        TextureConverter::convertLuminanceQuadrantsToRgba( data, outData, width, height );
    } else if ( inFormat == PIXEL_FORMAT_BGRA || inFormat == PIXEL_FORMAT_RGBA ) {
        TextureConverter::convertQuadrantsToRgba( data, outData, width, height );
    } else {
        LOGI( "Invalid quadrants input format: %s", pixelFormatToString( inFormat ));
        ASSERT( false );
    }
}

void TextureManager::convertPixelFormat( uint8_t *data, uint8_t *outData, int width, int height, PixelFormat inFormat,
                                         PixelFormat outFormat ) {
    ASSERT( outFormat != PIXEL_FORMAT_RGBA_QUADRANTS );
    if ( inFormat == outFormat ) {
        if ( data ) {
            int dataSize = width * height * getPixelFormatChannels( outFormat );
            memcpy( outData, data, dataSize );
        }
    } else {
        if ( inFormat == PIXEL_FORMAT_BGRA ) {
            switch ( outFormat ) {
                case PIXEL_FORMAT_RGB:
                    TextureConverter::convertBgraToRgb( data, outData, width, height );
                    break;
                case PIXEL_FORMAT_RGBA:
                    TextureConverter::convertBgraToRgba( data, outData, width, height );
                    break;

                case PIXEL_FORMAT_LUMINANCE:
                    TextureConverter::convertBgraToLuminance( data, outData, width, height );
                    break;

                case PIXEL_FORMAT_LUMINANCE_ALPHA:
                    TextureConverter::convertBgraToLuminanceAlpha( data, outData, width, height );
                    break;

                default:
                    ASSERT( false );
                    break;
            }
        } else if ( inFormat == PIXEL_FORMAT_RGBA ) {
            switch ( outFormat ) {
                case PIXEL_FORMAT_LUMINANCE:
                    // Just use the bgra function here, the conversion is the same
                    TextureConverter::convertBgraToLuminance( data, outData, width, height );
                    break;

                case PIXEL_FORMAT_LUMINANCE_ALPHA:
                    // Just use the bgra function here, the conversion is the same
                    TextureConverter::convertBgraToLuminanceAlpha( data, outData, width, height );
                    break;
                case PIXEL_FORMAT_RGB:
                    TextureConverter::convertRgbaToRgb( data, outData, width, height );
                    break;
                default:
                    LOGE( "Invalid in-out texture format combination from: %s to %s", pixelFormatToString( inFormat ),
                          pixelFormatToString( outFormat ));
                    ASSERT( false );
            }
        } else if ( inFormat == PIXEL_FORMAT_RGB ) {
            switch ( outFormat ) {
                case PIXEL_FORMAT_LUMINANCE:
                    // Just use the bgra function here, the conversion is the same
                    TextureConverter::convertRgbToLuminance( data, outData, width, height );
                    break;
                default:
                    LOGE( "Invalid in-out texture format combination from: %s to %s", pixelFormatToString( inFormat ),
                          pixelFormatToString( outFormat ));
                    ASSERT( false );
            }
        } else if ( inFormat == PIXEL_FORMAT_LUMINANCE ) {
            switch ( outFormat ) {
                case PIXEL_FORMAT_RGB:
                    TextureConverter::convertLuminanceToRgb( data, outData, width, height );
                    break;
                case PIXEL_FORMAT_RGBA:
                    TextureConverter::convertLuminanceToRgb( data, outData, width, height );
                    break;
                default:
                    LOGE( "Invalid in-out texture format combination from: %s to %s", pixelFormatToString( inFormat ),
                          pixelFormatToString( outFormat ));
                    ASSERT( false );
            }
        } else {
            LOGE( "Invalid in-out texture format combination from: %s to %s", pixelFormatToString( inFormat ),
                  pixelFormatToString( outFormat ));
            ASSERT( false );
        }
    }
}

bool TextureManager::isTexture( const std::string& id ) const {
    return mData.find( id ) != mData.end();
}

std::shared_ptr<Texture> TextureManager::TD( const std::string& tname, const int tSlot ) {

    std::string safeName = tname;
    if ( mData.find( safeName ) == mData.end() ) {
        safeName = S::WHITE;
    }

    auto ret = mData[safeName];
    if ( tSlot >= 0 ) {
        ret->textureSlot( tSlot );
    }

    return ret;
}
