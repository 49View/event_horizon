//
// Created by Dado on 2019-04-09.
//

#pragma once

#include <core/recursive_transformation.hpp>
#include <core/htypes_shared.hpp>

struct GeomData {
    ResourceRef vData;
    ResourceRef material;
};

using Geom = RecursiveTransformation<GeomData>;

//class Geom : public RecursiveTransformation<GeomData> {
//public:
//    using RecursiveTransformation::RecursiveTransformation;
//    Geom() = default;
//    virtual ~Geom() = default;
//    explicit Geom( const std::string& _name ) : RecursiveTransformation( _name ) {}
//
//    const ResourceRef& MaterialRef( size_t _index = 0 ) const;
//    const ResourceRef& VDataRef( size_t _index = 0 ) const;
//};
