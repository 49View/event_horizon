#pragma once

#include <map>
#include <string>
#include <core/service_factory.h>

#include "core/util.h"
#include "core/raw_image.h"
#include "core/file_manager.h"
#include "texture.h"

//====================================
// TextureManager
//====================================

typedef std::map<std::string, std::shared_ptr<Texture>> TextureMap;
typedef TextureMap::iterator TextureMapIt;
typedef TextureMap::const_iterator TextureMapCIt;

#define avcbTM (std::bind(&TextureManager::preparingStremingTexture, &RSG().RR().TM(), std::placeholders::_1, std::placeholders::_2))

class TextureManager {
public:
    TextureManager() = default;
    TextureManager( TextureManager const& ) = delete;
private:
    void operator=( TextureManager const& ) = delete;

public:
    std::shared_ptr<Texture> addTextureWithData( const std::string& id, const RawImage& rawImage,
                                                 TextureSlots _tslot = TSLOT_COLOR);
    std::shared_ptr<Texture> addTextureNoData( TextureRenderData& tb );
    std::shared_ptr<Texture> addCubemapTexture( TextureRenderData& tb );
    std::shared_ptr<Texture> addTextureWithGPUHandle( const std::string& id, unsigned int _handle,
                                                      unsigned int _secondaryHandle = 0 );
    void preparingStremingTexture( const std::string& _streamName, const V2i& sd );
    void updateTexture( const std::string& id, const uint8_t *data );
    void updateTexture( const RawImage& _image );
    void updateTexture( const std::string& id, const uint8_t *data, int width, int height );
    void updateTexture( const std::string& id, uint8_t *data, int width, int height, PixelFormat inFormat,
                        PixelFormat outFormat );
    void removeTexture( const std::string& id );

    int getTextureCount() const { return static_cast<int>( mTextures.size()); }

    bool isTexture( const std::string& id ) const;

//    void T( TextureRenderData& tb );

    /// Query texture direct without going through creation of new texture if name is not present
    /// MUST have texture present
    /// \param tname
    /// \param tSlot
    /// \return
    std::shared_ptr<Texture> TD( const std::string& tname, const int tSlot = -1 );

    static std::string textureName( const std::string input );

    std::shared_ptr<Texture> addTextureFromCallback( TextureRenderData& tb, std::unique_ptr<uint8_t []>& _data );
    std::shared_ptr<Texture> addTextureImmediate( TextureRenderData& tb, const uint8_t* _data );

    TextureMapIt begin();
    TextureMapIt end();
    TextureMapCIt begin() const;
    TextureMapCIt end() const;
private:

    std::shared_ptr<Texture> createTexture( TextureRenderData& tb );
    std::shared_ptr<Texture> createTexture( const std::string& _name, unsigned int _gpuHandle,
                                            TextureSlots _gpuSecondaryHandle );

private:
    static void convertToRgbaQuadrants( uint8_t *data, uint8_t *outData, int width, int height, PixelFormat inFormat,
                                        PixelFormat outFormat );
    static void convertPixelFormat( uint8_t *data, uint8_t *outData, int width, int height, PixelFormat inFormat,
                                    PixelFormat outFormat );

    TextureMap mTextures;
};
