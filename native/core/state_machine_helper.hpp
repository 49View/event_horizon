//
// Created by Dado on 2019-04-19.
//

#pragma once

#include <memory>
#include <core/sml.hpp>
using namespace boost::sml;

template <typename T>
class BackEndService {
public:
    T *BackEnd() {
        return backEnd.get();
    }
    void setBackEnd( const std::shared_ptr<T>& be ) {
        backEnd = be;
    }

protected:
    std::shared_ptr<T> backEnd;
};
