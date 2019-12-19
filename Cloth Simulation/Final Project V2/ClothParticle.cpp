#include "ClothParticleSpring.h"
#include <iostream>


ClothParticle::ClothParticle() {
	this->posFixed = false;
}

void ClothParticle::setFixed(bool fixed) {
	this->posFixed = fixed;
}

void ClothParticle::setMass(float mass) {
	this->mass = mass;
}

float ClothParticle::getMass(){
	return this->mass;
}

void ClothParticle::setPosition(glm::vec3 position) {
	this->position = position;
}

glm::vec3 ClothParticle::getPosition(){
	return this->position;
}

void ClothParticle::setVelocity(glm::vec3 velocity) {
	this->velocity = velocity;
}

glm::vec3 ClothParticle::getVelocity(){
	return this->velocity;
}

void ClothParticle::setAcceleration(glm::vec3 acceleration) {
	this->acceleration = acceleration;
}

glm::vec3 ClothParticle::getAcceleration(){
	return this->acceleration;
}

void ClothParticle::setDampeningConstant(float dampeningConstant) {
	this->dampConstant = dampeningConstant;
}

float ClothParticle::getDampeningConstant(){
	return this->dampConstant;
}

void ClothParticle::updateGeometry(float deltaTime) {
	if (this->posFixed){
		this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		this->netForce = glm::vec3(0.0f, 0.0f, 0.0f);

		return;
	}

	glm::vec3 newPosition;
	glm::vec3 newVelocity;

	//https://www.intmath.com/differential-equations/12-runge-kutta-rk4-des.php 
	glm::vec3 v1 = deltaTime * computeVelocity(0);
	glm::vec3 v2 = deltaTime * computeVelocity(0.5 * deltaTime);
	glm::vec3 v3 = deltaTime * computeVelocity(0.5 * deltaTime);
	glm::vec3 v4 = deltaTime * computeVelocity(deltaTime);
	newPosition = this->position + (1.0f / 6.0f) * (v1 + 2.0f * v2 + 2.0f * v3 + v4);

	glm::vec3 a1 = deltaTime * computeAcceleration(0, this->velocity);
	glm::vec3 a2 = deltaTime * computeAcceleration(0.5 * deltaTime, this->velocity + 0.5f * a1);
	glm::vec3 a3 = deltaTime * computeAcceleration(0.5 * deltaTime, this->velocity + 0.5f * a2);
	glm::vec3 a4 = deltaTime * computeAcceleration(deltaTime, this->velocity + a3);
	newVelocity = this->velocity + (1.0f / 6.0f) * (a1 + 2.0f * a2 + 2.0f * a3 + a4);

	this->acceleration = (newVelocity - this->velocity) / deltaTime;


	this->position = newPosition;
	this->velocity = newVelocity;

	this->netForce = glm::vec3(0.0f, 0.0f, 0.0f);
}

bool ClothParticle::isFixed() {
	return this->posFixed;
}

glm::vec3 ClothParticle::computeVelocity(float deltaTime, glm::vec3 const& velocity) {
	return velocity + this->acceleration * deltaTime;
}

glm::vec3 ClothParticle::computeVelocity(float deltaTime) {
	return computeVelocity(deltaTime, this->velocity);
}

glm::vec3 ClothParticle::computeAcceleration(float deltaTime, glm::vec3 const& velocity) {
	glm::vec3 position = this->position + velocity * deltaTime;
	return computeAcceleration(position, velocity);
}

glm::vec3 ClothParticle::computeAcceleration(glm::vec3 const& position, glm::vec3 const& velocity) {
	const float g = 9.81;
	glm::vec3 forceGravity(0.0f, -g * this->mass, 0.0f);

	glm::vec3 forceWind = this->mass * glm::vec3(0.0,0.0,0.0);

	this->netForce += forceGravity + forceWind + this->normalForce;

	return this->netForce / this->mass;
}

void ClothParticle::addForce(glm::vec3 force) {
	this->netForce += force;
}
