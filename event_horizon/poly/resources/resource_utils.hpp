//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <memory>
#include <string>
#include <set>
#include <tuple>
#include <unordered_map>

using ResourceRef = std::string;

template <typename T>
using ResourceSignalsAddSignature = std::tuple<std::shared_ptr<T>, std::string>;

template <typename C>
using ResourceManagerContainer = std::unordered_map<std::string,std::shared_ptr<C>>;

template <typename T>
using SignalsDeferredContainer = std::set<ResourceSignalsAddSignature<T>>;
