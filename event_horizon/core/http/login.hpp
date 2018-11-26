//
// Created by Dado on 2018-11-25.
//

#pragma once

#include <core/serialization.hpp>

namespace LoginSandbox {
    const static std::string Name = "eh_sandbox";
}

JSONDATA( LoginFields, project, email, password )
    std::string project  = LoginSandbox::Name;
    std::string email    = "guest";
    std::string password = "guest";

    explicit LoginFields( const std::string& _username,  const std::string& _password ) {
        project = LoginSandbox::Name;
        email = _username;
        password = _password;
    }

    static LoginFields Computer( const std::string& _projectName = LoginSandbox::Name ) {
        static LoginFields ret;
        ret.project = _projectName;
        ret.email = userComputerName();
        ret.password = std::to_string(cpuID());
        return ret;
    }
};
