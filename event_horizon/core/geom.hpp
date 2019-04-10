//
// Created by Dado on 2019-04-09.
//

#pragma once

#include <core/recursive_transformation.hpp>
#include <core/htypes_shared.hpp>

class Geom : public RecursiveTransformation<Geom> {
public:
    using RecursiveTransformation::RecursiveTransformation;

    Geom() = default;
    virtual ~Geom() = default;
    explicit Geom( const std::string& _name ) : RecursiveTransformation( _name ) {}

    bool empty() const override;

    const ResourceRef& MaterialRef() const;
    const ResourceRef& VDataRef() const;
    void MaterialRef( const ResourceRef& material );
    void VDataRef( const ResourceRef& vData );

private:
    ResourceRef material;
    ResourceRef vData;
};



