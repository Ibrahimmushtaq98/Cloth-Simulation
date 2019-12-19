#include "ClothParticleSpring.h"
#include <algorithm>


ClothSpring::ClothSpring() {}

void ClothSpring::setSpringConstant(float springConstant){
	this->springConstant = springConstant;
}

float ClothSpring::getSpringConstant(){
	return this->springConstant;
}

void ClothSpring::setDampeningConstant(float dampeningConstant){
	this->dampConstant = dampeningConstant;
}

float ClothSpring::getDampeningConstant(){
	return this->dampConstant;
}

void ClothSpring::setEndParticles(std::pair<ClothParticle*, ClothParticle*> const& endParticles){
	this->endParticles = endParticles;
}

void ClothSpring::setEndParticles(ClothParticle* endA, ClothParticle* endB){
	this->endParticles = std::pair<ClothParticle*, ClothParticle*>(endA, endB);
}

std::pair<ClothParticle*, ClothParticle*> ClothSpring::getEndParticles() const{
	return this->endParticles;
}

void ClothSpring::setRestLength(float restLength){
	this->restLength = restLength;
}

float ClothSpring::getRestLength(){
	return this->restLength;
}

void ClothSpring::updateGeometry(float t){
	float length = glm::length(this->endParticles.first->getPosition() - this->endParticles.second->getPosition());

	glm::vec3 forceSpring;
	if (length == 0){
		forceSpring = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	else{
		glm::vec3 coilAxis = glm::normalize(this->endParticles.first->getPosition() - this->endParticles.second->getPosition());
		float deltaLength;
		if (length > this->restLength){
			deltaLength = std::min<float>((length - this->restLength), ((1.1f) * this->restLength) - this->restLength);
		}
		else{
			deltaLength = std::max<float>((length - this->restLength), ((0.9f) * this->restLength) - this->restLength);
		}

		forceSpring = this->springConstant * coilAxis * (deltaLength);
	}

	//Compute the dampening force
	glm::vec3 forceDampening1 = -this->dampConstant * this->endParticles.first->getVelocity();
	glm::vec3 forceDampening2 = -this->dampConstant * this->endParticles.second->getVelocity();

	this->endParticles.first->addForce(-forceSpring + forceDampening1);
	this->endParticles.second->addForce(forceSpring + forceDampening2);
}

void ClothSpring::checkStretch(){
	ClothParticle* first = this->endParticles.first;
	ClothParticle* second = this->endParticles.second;

	if (first->isFixed() && second->isFixed()){
		return;
	}
	float stretch = glm::length(first->getPosition() - second->getPosition());
	glm::vec3 firstToSecond = glm::normalize(second->getPosition() - first->getPosition());

	float stretchFactor = 1.0001f;
	if (stretchFactor * this->restLength < stretch){
		float offset = stretch - stretchFactor * this->restLength;

		if (!first->isFixed() && !second->isFixed()){
			offset *= 0.5f;

			first->setPosition(first->getPosition() + offset * firstToSecond);
			second->setPosition(second->getPosition() - offset * firstToSecond);
			return;
		}

		if (!first->isFixed()){
			first->setPosition(first->getPosition() + offset * firstToSecond);
			return;
		}

		second->setPosition(second->getPosition() - offset * firstToSecond);
		return;
	}

	else if (0.9999f * this->restLength > stretch){
		float offset = 0.9999f * this->restLength - stretch;

		if (!first->isFixed() && !second->isFixed()){
			offset *= 0.50f;

			first->setPosition(first->getPosition() - offset * firstToSecond);
			second->setPosition(second->getPosition() + offset * firstToSecond);
			return;
		}

		if (!first->isFixed()){
			first->setPosition(first->getPosition() - offset * firstToSecond);
			return;
		}

		//second is not fixed
		second->setPosition(second->getPosition() + offset * firstToSecond);
		return;
	}
}
