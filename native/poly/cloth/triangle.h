#pragma once

class Particle;

class Triangle {
public:
	Triangle(Particle* p1, Particle* p2, Particle* p3);
	void display();

	Particle* particles[3];
};
