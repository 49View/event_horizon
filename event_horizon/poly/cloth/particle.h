/*
 * Particle.h
 *
 *  Created on: 15/10/2014
 *      Author: sam
 */

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Particle {
public:
	Particle(glm::vec3 pos, float mass, float damping, glm::vec2 texCoord);

	bool isStatic;
	glm::vec3 normal;
	glm::vec3 pos;
	glm::vec3 lastPos;
	glm::vec2 texCoord;
	float damping;
	float mass;
	glm::vec3 force;

	void move(glm::vec3 delta);
	void step(float timeStep);

};
