//
// Created by Dado on 2018-11-25.
//

#pragma once

#include <core/serialization.hpp>

namespace LoginSandbox {
    const static std::string Name = "eh_sandbox";
    const static std::string Daemon = "daemon";
}

JSONDATA( LoginFields, project, email, password )
    std::string project  = LoginSandbox::Name;
    std::string email    = "guest";
    std::string password = "guest";

    explicit LoginFields( const LoginFields& _lf,  const std::string& _project ) {
        project = _project;
        email = _lf.email;
        password = _lf.password;
    }

    explicit LoginFields( const std::string& _username,  const std::string& _password ) {
        project = LoginSandbox::Name;
        email = _username;
        password = _password;
    }

    explicit LoginFields( const std::string& _username, const std::string& _password, const std::string& _proj ) {
        project = _proj;
        email = _username;
        password = _password;
    }

    static LoginFields Daemon( const std::string& _project = "" ) {
        static LoginFields ret;
        ret.project = _project.empty() ? LoginSandbox::Name : _project;
        ret.email = LoginSandbox::Daemon;
        ret.password = "luckycarrot1803";
        return ret;
    }

    bool isDaemon() const {
        return email == LoginSandbox::Daemon;
    }

};

