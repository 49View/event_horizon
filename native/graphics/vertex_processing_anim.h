#pragma once

#include <graphics/vertex_processing.h>
#include <core/math/anim.h>

template <typename M>
void parseFader( VPListFlatContainer& vp, AnimEndCallback& aec, const M& _param ) {

    if constexpr ( std::is_same_v<M, std::shared_ptr<VPList>> ) {
        if ( _param ) vp.emplace_back( _param );
    }
    if constexpr ( std::is_same_v<M, VPListFlatContainer> ) {
        std::copy (_param.begin(), _param.end(), std::back_inserter(vp));
    }
    if constexpr ( std::is_same_v<M, AnimEndCallback> ) {
        aec = _param;
    }

}

template<typename ...Args>
void fader( float _duration, float _value, Args&& ...args ) {

    VPListFlatContainer vplists{};
    AnimEndCallback aec{};

    ( parseFader( vplists, aec, std::forward<Args>(args)),... );

    Timeline::intermezzo( _duration, 0, AnimUpdateCallback([vplists, _value, _duration](float _elapsed) {
        float current = (_elapsed / _duration);
        for ( auto& vl : vplists ) {
            if ( _elapsed > 0.0f ) {
                if ( _value > 0.0f ) {
                    vl->setMaterialConstantAlpha( min( _value, max( vl->getMaterialConstantAlpha(), current) ) );
                } else {
                    vl->setMaterialConstantAlpha( max( _value, min( vl->getMaterialConstantAlpha(),(1.0f-_value)-current )) );
                }
            }
        }
    }), aec );
}

