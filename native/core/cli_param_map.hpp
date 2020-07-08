//
// Created by dado on 19/06/2020.
//

#pragma once

#include <string>
#include <optional>
#include <core/htypes_shared.hpp>

class CLIParamMap {
public:
    CLIParamMap() = default;
    CLIParamMap( int argc, char *argv[] );
    [[nodiscard]] std::optional<std::string> getParam( const std::string& key ) const;
    std::string printAll() const;
private:
    KVStringMap params;
};
