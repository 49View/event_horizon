/*
 * Cloth.h
 *
 *  Created on: 15/10/2014
 *      Author: sam
 */

#pragma once

#include <vector>
#include <glm/mat4x4.hpp>
#include <poly/cloth/particle.h>
#include <poly/cloth/constraint.h>
#include <poly/cloth/triangle.h>

class Cloth {
public:
	Cloth( float width, float height, int particleWidth, int particleHeight, float weight, float damping,
	       glm::mat4 transform);

    void display();
	void step(float timeStep);
	std::vector<Particle> particles;

    int getParticleWidth() const {
        return particleWidth;
    }

    void setParticleWidth( int _particleWidth ) {
        Cloth::particleWidth = _particleWidth;
    }

    int getParticleHeight() const {
        return particleHeight;
    }

    void setParticleHeight( int _particleHeight ) {
        Cloth::particleHeight = _particleHeight;
    }

    const std::vector<Triangle>& Triangles() const {
        return triangles;
    }

private:
	std::vector<Constraint> constraints;
	std::vector<Triangle> triangles;
	int particleWidth;
	int particleHeight;
};
