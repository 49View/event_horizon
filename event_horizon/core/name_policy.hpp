//
// Created by Dado on 2019-01-29.
//

#pragma once

#include <string>

class NamePolicy {
public:
    inline const std::string& Name() const {
        return name;
    }
    inline std::string Name() {
        return name;
    }
    inline void Name( const std::string& _name ){
        name = _name;
    }

private:
    std::string name;
};



