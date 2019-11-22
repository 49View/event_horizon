//
// Created by Dado on 2019-01-28.
//

#include "heterogeneous_map.hpp"

HeterogeneousMap::HeterogeneousMap( const HeterogeneousMap& _source ) {
    inject( _source );
}

HeterogeneousMap::HeterogeneousMap( std::shared_ptr<HeterogeneousMap> _source ) {
    inject( *_source.get() );
}

const std::string& HeterogeneousMap::Type() const {
    return mType;
}

void HeterogeneousMap::Type( const std::string& _type ) {
    HeterogeneousMap::mType = _type;
}

void HeterogeneousMap::inject( const HeterogeneousMap& source ) {
    mType = source.mType;
    for ( auto& pu : source.mStrings ) assign( pu.first, pu.second );
    for ( auto& pu : source.mInts )    assign( pu.first, pu.second );
    for ( auto& pu : source.mFloats )  assign( pu.first, pu.second );
    for ( auto& pu : source.mV2fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mV3fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mV3fvs )   assign( pu.first, pu.second );
    for ( auto& pu : source.mV4fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mM3fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mM4fs )    assign( pu.first, pu.second );
}

void HeterogeneousMap::injectIfNotPresent( const HeterogeneousMap& source ) {
    for ( auto& pu : source.mStrings  ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mInts     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mFloats   ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV2fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV3fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV3fvs    ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV4fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mM3fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mM4fs     ) assignIfNotPresent( pu.first, pu.second );
}

std::shared_ptr<HeterogeneousMap> HeterogeneousMap::clone() {
    std::shared_ptr<HeterogeneousMap> ret = std::make_shared<HeterogeneousMap>();

    ret->clone( *this );

    return ret;
}

void HeterogeneousMap::clone( const HeterogeneousMap& _source ) {
    mType        = _source.mType;
    mStrings     = _source.mStrings;
    mFloats      = _source.mFloats;
    mInts        = _source.mInts;
    mV2fs        = _source.mV2fs;
    mV3fs        = _source.mV3fs;
    mV4fs        = _source.mV4fs;
    mM3fs        = _source.mM3fs;
    mM4fs        = _source.mM4fs;
    mV3fvs       = _source.mV3fvs;
}
