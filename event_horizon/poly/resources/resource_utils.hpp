//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <tuple>
#include <unordered_map>

template <typename T>
using ResourceSignalsAddSignature = std::tuple<std::shared_ptr<T>, std::string>;

template <typename C>
using ResourceManagerContainer = std::unordered_map<std::string,std::shared_ptr<C>>;

template <typename T>
using SignalsDeferredContainer = std::set<ResourceSignalsAddSignature<T>>;

inline static size_t resourcePriority( const ResourceRef& ref ) {
    if ( ref == ResourceGroup::Image ||
         ref == ResourceGroup::Font ||
         ref == ResourceGroup::Color ||
         ref == ResourceGroup::CameraRig ||
         ref == ResourceGroup::Profile ) return 0;

    if ( ref == ResourceGroup::Material ) return 10;

    if ( ref == ResourceGroup::Geom || ref == ResourceGroup::UI ) return 20;

        return 0;
}