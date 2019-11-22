/*
 * Constraint.h
 *
 *  Created on: 15/10/2014
 *      Author: sam
 */

#pragma once

class Particle;

class Constraint {
public:
	Constraint(Particle *p1, Particle *p2);

	void satisfy();

private:
	Particle *p1, *p2;
	float distance;
};

