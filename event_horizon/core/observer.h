#pragma once

#include  <string>

template <typename T>
struct Observer {
    virtual void notified( T& _source, const std::string& generator ) = 0;
};

template <typename T>
struct ObserverShared {
    virtual void notified( std::shared_ptr<T> _source, const std::string& generator ) = 0;
};