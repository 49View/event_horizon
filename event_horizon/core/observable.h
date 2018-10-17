#pragma once

#include <memory>
#include <string>
#include <vector>

#include "observer.h"

template <typename T>
struct Observable {
    std::vector< std::shared_ptr<Observer<T>>> observers;
    std::vector< Observer<T>* > observersCref;

    void notify( T& _source, const std::string& _name ) {
        for ( auto& o : observers ) {
            o->notified( _source, _name );
        }
        for ( auto& o : observersCref ) {
            o->notified( _source, _name );
        }
    }

    void subscribe( Observer<T>* _val ) {
        observersCref.push_back(_val);
    }

    void subscribe( std::shared_ptr<Observer<T>> _val ) {
        observers.push_back(_val);
    }

    void unsubscribe( std::shared_ptr<Observer<T>> _val ) {
        observers.erase( remove( observers.begin(), observers.end(), _val), observers.end() );
    }

};


template <typename T>
struct ObservableShared {
    std::shared_ptr<ObserverShared<T>> observer;

    void notify( std::shared_ptr<T> _source, const std::string& _name ) {
        observer->notified( _source, _name );
    }

    void subscribe( std::shared_ptr<ObserverShared<T>> _val ) {
        observer = _val;
    }

    void unsubscribe( std::shared_ptr<ObserverShared<T>> _val ) {
        observer = nullptr;
//        observers.erase( remove( observers.begin(), observers.end(), _val), observers.end() );
    }

};
