#ifndef CLOTH_PARTICLE_SPRING_H
#define CLOTH_PARTICLE_SPRING_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <set>
#include <memory>
#include <algorithm>
#include <vector>

class ClothParticle;

class ClothSpring{
public:
	ClothSpring();

	void setSpringConstant(float springConstant);
	float getSpringConstant();

	void setDampeningConstant(float dampeningConstant);
	float getDampeningConstant();

	void setEndParticles(std::pair<ClothParticle*, ClothParticle*> const& endParticles);
	void setEndParticles(ClothParticle* endA, ClothParticle* endB);
	std::pair<ClothParticle*, ClothParticle*> getEndParticles() const;

	void setRestLength(float restLength);
	float getRestLength();

	void updateGeometry(float t);
	void checkStretch();

private:
	float springConstant;
	float dampConstant;
	float restLength;
	std::pair<ClothParticle*, ClothParticle*> endParticles;
};

class ClothParticle{
public:

	ClothParticle();

	void updateGeometry(float t);

	void setMass(float mass);
	float getMass();

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();

	void setVelocity(glm::vec3 velocity);
	glm::vec3 getVelocity();

	void setAcceleration(glm::vec3 acceleration);
	glm::vec3 getAcceleration();

	void setDampeningConstant(float dampeningConstant);
	float getDampeningConstant();

	void setFixed(bool fixed);
	bool isFixed();

	void addForce(glm::vec3 force);


private:
	std::vector<std::shared_ptr<ClothSpring>> springs;

	float mass;
	glm::vec3 position, velocity, acceleration;

	glm::vec3 computeVelocity(float deltaTime, glm::vec3 const& velocity);
	glm::vec3 computeVelocity(float deltaTime);
	glm::vec3 computeAcceleration(float deltaTime, glm::vec3 const& velocity);
	glm::vec3 computeAcceleration(glm::vec3 const& position, glm::vec3 const& velocity);

	bool posFixed;

	glm::vec3 netForce;
	glm::vec3 normalForce;

	float dampConstant;
};

#endif