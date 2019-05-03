/*
 * Cloth.h
 *
 *  Created on: 15/10/2014
 *      Author: sam
 */

#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

class Particle;
class Constraint;
class Triangle;

class Cloth {
public:
	Cloth(float width, float height, int particleWidth, int particleHeight, float weight, float damping, glm::mat4 transform);

    void display();
	void step(float timeStep);
	std::vector<Particle> particles;

private:
	std::vector<Constraint> constraints;
	std::vector<Triangle> triangles;
	int particleWidth;
	int particleHeight;
};
