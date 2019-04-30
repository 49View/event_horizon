//
// Created by Dado on 2019-04-30.
//

#pragma once

#include <core/htypes_shared.hpp>
#include <core/app_globals.h>
#include <core/math/quad_vertices.h>

typedef std::pair<Vector2f, Vector2f> TextureFillModeScalers;

static inline TextureFillModeScalers getTextureFillModeScalers( const RectFillMode fm,
                                                                const Vector2f& size, float _aspectRation ) {
    // Those 2 variables handle basically a "AspectFit" scaling, in which the image is scaled in a way the whole image is present maintaining the aspect ratio.
    Vector2f texAspectVector = size.vectorRatio();
    Vector2f texAspectVectorOffset = Vector2f::ZERO;

    if ( fm == RectFillMode::Scale ) {
        texAspectVector = Vector2f::ONE;
    }

    if ( fm == RectFillMode::AspectFill ) {
        float aoh = _aspectRation;
        float sar = size.ratio();
        if ( aoh > sar ) {
            texAspectVector = Vector2f{ (1.0f/aoh) * getScreenAspectRatio, 1.0f };
            texAspectVectorOffset = { (aoh / getScreenAspectRatio) * 0.5f + 0.5f, 0.0f };
        } else {
            texAspectVector = Vector2f{ 1.0f, aoh / (getScreenAspectRatio) };
            texAspectVectorOffset = { 0.0f, (aoh / getScreenAspectRatio) * -0.5f + 0.5f };
        }
    }
    if ( fm == RectFillMode::AspectFit || fm == RectFillMode::AspectFitLeft ||
         fm == RectFillMode::AspectFitRight ) {
        float sizeTexRatio = size.ratio() / _aspectRation;
        if ( sizeTexRatio > 1.0f ) {
            texAspectVector = { sizeTexRatio, 1.0f };
            texAspectVectorOffset = { sizeTexRatio * -0.5f + 0.5f, 0.0f };
            if ( fm == RectFillMode::AspectFitLeft ) texAspectVectorOffset = Vector2f::ZERO;
            if ( fm == RectFillMode::AspectFitRight )
                texAspectVectorOffset.setX( texAspectVectorOffset.x() * 2.0f );
        } else {
            float invSizeTexRatio = 1.0f / sizeTexRatio;
            texAspectVector = { 1.0f, invSizeTexRatio };
            texAspectVectorOffset = { 0.0f, ( invSizeTexRatio * -0.5f + 0.5f ) };
        }
    }

    return std::make_pair( texAspectVector, texAspectVectorOffset );
}

static inline TextureFillModeScalers getTextureFillModeScalers( const RectFillMode fm, const Vector2f& size ) {
    // Those 2 variables handle basically a "AspectFit" scaling, in which the image is scaled in a way the whole image is present maintaining the aspect ratio.
    Vector2f texAspectVector = Vector2f::ONE;// size.vectorRatio();
    Vector2f texAspectVectorOffset = Vector2f::ZERO;

    switch ( fm ) {
        case RectFillMode::Scale:
            texAspectVector = Vector2f::ONE;
            break;
        case RectFillMode::AspectFit:
        case RectFillMode::AspectFill:
        case RectFillMode::AspectFitLeft:
        case RectFillMode::AspectFitRight:
        case RectFillMode::AspectFitTop:
        case RectFillMode::AspectFitBottom:
            texAspectVector = size.vectorRatio();
            break;
        default:
            break;
    }

    return std::make_pair( texAspectVector, texAspectVectorOffset );
}

static inline Vector2f textureFillModeMapping( const Rect2f& rect, const Vector2f pos, TextureFillModeScalers tfms ) {
    return rect.normalizeWithinRect( pos ) * tfms.first + tfms.second;
}

static inline QuadVertices2 textureQuadFillModeMapping( const RectFillMode fm, const Rect2f& rect,
                                                        float _imageAspectRatio ) {
    TextureFillModeScalers tmfs = getTextureFillModeScalers( fm, rect.size(), _imageAspectRatio );
    QuadVertices2 qvt(
            textureFillModeMapping( rect, rect.topRight(), tmfs ),
            textureFillModeMapping( rect, rect.bottomRight(), tmfs ),
            textureFillModeMapping( rect, rect.topLeft(), tmfs ),
            textureFillModeMapping( rect, rect.bottomLeft(), tmfs )
    );
    return qvt;
}
