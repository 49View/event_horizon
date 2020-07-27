//
// Created by dado on 20/06/2020.
//

#pragma once

template<typename T>
static ImTextureID ImGuiRenderTexture( const T& im ) {
    return reinterpret_cast<ImTextureID *>(im);
};

static constexpr float thumbSize = 128.0f;
