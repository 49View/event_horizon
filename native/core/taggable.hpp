//
// Created by Dado on 2019-02-20.
//

#pragma once

#include <set>
#include <string>
#include <core/name_policy.hpp>

template <typename T = std::string>
class Taggable : public virtual NamePolicy<T> {
    using taggableContainer = std::set<T>;
public:
    Taggable() = default;
    explicit Taggable( T _name ) {
        Name( std::move(_name) );
    }

    const T& Name() const override {
        return NamePolicy<T>::Name();
    }

    void Name( const T& _name ) override {
        nameClear();
        NamePolicy<T>::Name(_name);
        nameSplit();
    }

protected:
    void nameClear() {
        auto lcname = toLower( NamePolicy<T>::Name() );
        auto ltags = split( lcname, '_' );
        for ( const auto& v : ltags ) {
            tags.erase( v );
        }
    }

    void nameSplit() {
        auto lcname = toLower( NamePolicy<T>::Name() );
        auto ltags = split_words( lcname );
        for ( const auto& v : ltags ) {
            tags.emplace( v );
        }
    }

    void removeTag( const T& _tag ) { tags.erase(_tag); }
    void addTag( const T& _tag ) { tags.emplace(_tag); }

    const taggableContainer& Tags() const { return tags; }
    void Tags( const taggableContainer& _tags ) { tags = _tags; }

private:
    taggableContainer tags;
};