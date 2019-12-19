//#pragma once
//#ifndef CLOTH_SPRING
//#define CLOTH_SPRING
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <set>
//#include <vector>
//#include <algorithm>
//#include "ClothParticle.h"
//
//class ClothSpring{
//public:
//	ClothSpring() {
//
//	}
//
//	void setSpringConstant(float springConstant) {
//		this->springConstant = springConstant;
//	}
//	float getSpringConstant() {
//		return this->springConstant;
//	}
//
//	void setDampeningConstant(float dampeningConstant) {
//		this->dampConstant = dampeningConstant;
//	}
//	float getDampeningConstant() {
//		return this->dampConstant;
//	}
//
//	void setEndParticles(std::pair<ClothParticle*, ClothParticle*> endParticles) {
//		this->endParticles = endParticles;
//	}
//
//	void setEndParticles(ClothParticle* endA, ClothParticle* endB) {
//		this->endParticles = std::pair<ClothParticle*, ClothParticle*>(endA, endB);
//	}
//	std::pair<ClothParticle*, ClothParticle*> getEndParticles() {
//		return this->endParticles;
//	}
//
//	void setRestLength(float restLength) {
//		this->restLength = restLength;
//	}
//	float getRestLength() {
//		return this->restLength;
//	}
//
//	void updateGeometry(float deltaTime) {
//		float length = glm::length(this->endParticles.first->getPosition() - this->endParticles.second->getPosition());
//
//		glm::vec3 forceSpring;
//		if (length == 0) {
//			forceSpring = glm::vec3(0.0f, 0.0f, 0.0f);
//		}
//		else {
//			glm::vec3 coilAxis = glm::normalize(this->endParticles.first->getPosition() - this->endParticles.second->getPosition());
//			float deltaLength;
//			if (length > this->restLength) {
//				/*deltaLength = std::min(length - mRestLength, 1.1f*mRestLength - mRestLength);*/
//				deltaLength = std::min<float>((length - this->restLength), ((1.1f) * this->restLength) - this->restLength);
//			}
//			else {
//				deltaLength = std::max<float>((length - this->restLength), ((0.9f) * this->restLength) - this->restLength);
//			}
//
//			forceSpring = this->springConstant * coilAxis * (deltaLength);
//		}
//
//		//Compute the dampening force
//		glm::vec3 forceDampening1 = -this->dampConstant * this->endParticles.first->getVelocity();
//		glm::vec3 forceDampening2 = -this->dampConstant * this->endParticles.second->getVelocity();
//
//		this->endParticles.first->addForce(-forceSpring + forceDampening1);
//		this->endParticles.second->addForce(forceSpring + forceDampening2);
//	}
//
//	void setModel(glm::mat4 model) {
//		this->model = model;
//	}
//
//	void applyTransformations() {
//		this->setModel(glm::mat4(1.0f));
//
//		glm::vec3 coilAxis = this->endParticles.first->getPosition() - this->endParticles.second->getPosition();
//		float coilLength = glm::length(coilAxis);
//
//		this->model = (glm::scale(glm::mat4(1.0f), glm::vec3(1.0, coilLength, 1.0))) * this->model;
//
//		if (coilLength > 0) {
//			coilAxis /= coilLength;
//
//			//we rotate the entire spring so that it is algined with its correct axis
//			glm::vec3 w = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), coilAxis);
//			if (w != glm::vec3(0.0f, 0.0f, 0.0f)) {
//				float theta = acos(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), coilAxis));
//				this->model = (glm::rotate(glm::mat4(1.0f), theta, w)) * this->model;
//			}
//
//		}
//
//		//Move the spring to its correct location
//		glm::vec3 midpoint = 0.5f * (this->endParticles.first->getPosition() + this->endParticles.second->getPosition());
//		this->model = (glm::translate(glm::mat4(1.0f), midpoint)) * this->model;
//	}
//
//	void checkStretch() {
//		ClothParticle* first = this->endParticles.first;
//		ClothParticle* second = this->endParticles.second;
//
//		if (first->getFixed() && second->getFixed()) {
//			return;
//		}
//		float stretch = glm::length(first->getPosition() - second->getPosition());
//		glm::vec3 firstToSecond = glm::normalize(second->getPosition() - first->getPosition());
//
//		float stretchFactor = 1.0001f;
//		if (stretchFactor * this->restLength < stretch) {
//			float offset = stretch - stretchFactor * this->restLength;
//
//			if (!first->getFixed() && !second->getFixed()) {
//				offset *= 0.5f;
//
//				first->setPosition(first->getPosition() + offset * firstToSecond);
//				second->setPosition(second->getPosition() - offset * firstToSecond);
//				return;
//			}
//
//			if (!first->getFixed()) {
//				first->setPosition(first->getPosition() + offset * firstToSecond);
//				return;
//			}
//
//			second->setPosition(second->getPosition() - offset * firstToSecond);
//			return;
//		}
//
//		//if particles are too close to each other
//		else if (0.9999f * this->restLength > stretch) {
//			float offset = 0.9999f * this->restLength - stretch;
//
//			if (!first->getFixed() && !second->getFixed()) {
//				offset *= 0.50f;
//
//				first->setPosition(first->getPosition() - offset * firstToSecond);
//				second->setPosition(second->getPosition() + offset * firstToSecond);
//				return;
//			}
//
//			if (!first->getFixed()) {
//				first->setPosition(first->getPosition() - offset * firstToSecond);
//				return;
//			}
//
//			second->setPosition(second->getPosition() + offset * firstToSecond);
//			return;
//		}
//	}
//
//
//private:
//	float springConstant;
//	float dampConstant;
//	float restLength;
//	std::pair<ClothParticle*, ClothParticle*> endParticles;
//
//	GLuint VAO;
//	GLuint vbuffer, cbuffer, ibuffer;
//	Shaders* springProgram;
//	glm::mat4 model;
//
//};
//
//#endif // !CLOTH_SPRING
//

