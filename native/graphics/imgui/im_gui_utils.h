//
// Created by dado on 20/06/2020.
//

#ifndef HOUSE_MAKER_IM_GUI_UTILS_H
#define HOUSE_MAKER_IM_GUI_UTILS_H

template<typename T>
static ImTextureID ImGuiRenderTexture( const T& im ) {
    return reinterpret_cast<ImTextureID *>(im);
};

static constexpr int thumbSize = 128;

#endif //HOUSE_MAKER_IM_GUI_UTILS_H
