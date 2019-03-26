//
// Created by Dado on 2019-03-26.
//

#pragma once

#include <string>

class Renderer;
class HeterogeneousMap;
class Program;

class ShaderMaterial {
public:
    explicit ShaderMaterial( std::string _sn );
    ShaderMaterial( std::string shaderName, Renderer& _rr );
    ShaderMaterial( std::string shaderName, std::shared_ptr<HeterogeneousMap> values );
    void activate( Renderer& _rr );

    const std::string& SN() const;
    const std::shared_ptr<Program> P() const;
    const std::shared_ptr<HeterogeneousMap>& Values() const;

private:
    std::string shaderName;
    std::shared_ptr<Program> shaderProgram = nullptr;
    std::shared_ptr<HeterogeneousMap> values;
};



