#include "Particle.h"
#include "Precision.h"
#include <cassert>


void Particle::setMass(const real mass) {
	this->mass = mass;
}
void Particle::setInverseMass(const real inverseMass) {
	this->inverseMass = inverseMass;
}

void Particle::integrate(real duration) {
	assert(duration > 0.0);

	// update position
	// p += vt
	position.addScaledVector(velocity, duration);

	Vector3 resultingAcceleration = acceleration;
	resultingAcceleration.addScaledVector(forceAccum, inverseMass);

	velocity.addScaledVector(resultingAcceleration, duration);

	//impose drag
	velocity *= real_pow(damping, duration);

	forceAccum.clear();
}

void Particle::setPosition(const Vector3& Newposition) {
	position = Newposition;
}

Vector3 Particle::getPosition() {
	return position;
}

void Particle::setVelocity(const Vector3& velocity) {
	this->velocity = velocity;
}

Vector3 Particle::getVelocity() {
	return this->velocity;
}

void Particle::setAcceleration(const Vector3& acceleration) {
	this->acceleration = acceleration;
}

Vector3 Particle::getAcceleration() {
	return this->acceleration;
}

void Particle::addForce(const Vector3& force) {
	forceAccum += force;
}

void Particle::setDamping(real scalar) {
	damping = scalar;
}