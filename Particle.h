#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vector3.h"
#include "Precision.h"

using namespace Physics;

class Particle
{


public:
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;

	Vector3 forceAccum;

	// todo 
	// make private ->get/set ??
	real damping;
	real mass;
	real inverseMass;

	void setMass(const real mass);
	void setInverseMass(const real inverseMass);

	void setPosition(const Vector3& position);
	Vector3 getPosition();

	void setVelocity(const Vector3& velocity);
	Vector3 getVelocity();

	void setAcceleration(const Vector3& acceleration);
	Vector3 getAcceleration();

	void setDamping(real scalar);

	void addForce(const Vector3& force);

	void integrate(real duration);


private:


};


#endif