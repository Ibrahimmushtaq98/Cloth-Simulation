#ifndef CLOTH_H
#define CLOTH_H

#include "ClothParticleSpring.h"
#include "ClothSpring.h"
#include "ShadersV2.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>

class Cloth {
public:
	Cloth(float w, float h, float n, float m) {
		this->mass = m;
		float particlesPerUnitLength = 2;
		this->particleWidth = w * particlesPerUnitLength;
		this->particleHeight = h * particlesPerUnitLength;

		this->particleNum = this->particleWidth * this->particleHeight;
		const float massPerParticle = 0.001;

		this->vertexPosition = std::vector<glm::vec3>(this->particleNum);
		this->vertextNormal = std::vector<glm::vec3>(this->particleNum);
		this->vertexColor = std::vector<glm::vec3>(this->particleNum);
		this->vertexTextColor = std::vector<glm::vec2>(this->particleNum);
		this->vertextIndex = std::vector<GLint>((this->particleWidth - 1) * (this->particleHeight - 1) * 6);

		this->particles = std::vector<std::unique_ptr<ClothParticle>>(this->particleNum);

		int currentParticle = 0;
		for (int i = 0; i < this->particleHeight; i++) {
			for (int j = 0; j < this->particleWidth; j++) {
				const int subscriptOffset = i * this->particleWidth + 3 * j;

				std::unique_ptr<ClothParticle> particle = std::make_unique<ClothParticle>();
				particle->setPosition(glm::vec3(j / particlesPerUnitLength, 0.0f, i / particlesPerUnitLength));
				particle->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
				particle->setMass(massPerParticle);

				//if ((i == this->particleHeight - 1 && j == 0) /*|| 
				//	(i == this->particleHeight - 1 && j == this->particleWidth - 1)||
				//	(i == 0 && j == this->particleWidth - 1)||
				//	(i == 0 && j == 0)*/) {
				//	particle->setFixed(true);
				//}

				if ((i ==( this->particleHeight - 1)/2 && j == (this->particleWidth - 1)/2) /*||
					(i == this->particleHeight - 1 && j == this->particleWidth - 1)||
					(i == 0 && j == this->particleWidth - 1)||
					(i == 0 && j == 0)*/) {
					particle->setFixed(true);
				}

				this->particles[currentParticle] = std::move(particle);
				currentParticle++;
			}
		}

		for (int i = 0; i < this->particleHeight; i++) {
			for (int j = 0; j < this->particleWidth; j++) {
				const int current = i * this->particleWidth + j;
				ClothParticle* currentParticle = this->particles[current].get();

				const float structuralSpringConstant = 0.5f;
				const float structuralDampeningConstant = 0.002f;
				const float shearSpringConstant = 0.5f;
				const float shearDampeningConstant = 0.0f;
				const float bendingSpringConstant = 1.0f;
				const float bendingDampeningConstant = 0.0f;

				//Add Structural Springs
				const int left = current - 1;
				const int right = current + 1;
				const int top = current - this->particleHeight;
				const int bottom = current + this->particleHeight;
				const int topLeft = top - 1;
				const int topRight = top + 1;
				const int bottomLeft = bottom - 1;
				const int bottomRight = bottom + 1;
				const int rightRight = right + 1;
				const int bottomBottom = bottom + this->particleHeight;

				//Set the structural springs
				if (j < this->particleWidth - 1){
					ClothParticle* otherParticle = this->particles[right].get();
					std::unique_ptr<ClothSpring> coil = std::make_unique<ClothSpring>();
					coil->setRestLength(glm::length(currentParticle->getPosition() - otherParticle->getPosition()));
					coil->setEndParticles(currentParticle, otherParticle);
					coil->setSpringConstant(structuralSpringConstant);
					coil->setDampeningConstant(structuralDampeningConstant);
					this->structural.push_back(std::move(coil));
				}
				if (i < this->particleHeight - 1){
					ClothParticle* otherParticle = this->particles[bottom].get();
					std::unique_ptr<ClothSpring> coil = std::make_unique<ClothSpring>();
					coil->setRestLength(glm::length(currentParticle->getPosition() - otherParticle->getPosition()));
					coil->setEndParticles(currentParticle, otherParticle);
					coil->setSpringConstant(structuralSpringConstant);
					coil->setDampeningConstant(structuralDampeningConstant);
					this->structural.push_back(std::move(coil));
				}

//Set the shear springs
if (i > 0 && j < this->particleWidth - 1){
	ClothParticle* otherParticle = this->particles[topRight].get();
	std::unique_ptr<ClothSpring> coil = std::make_unique<ClothSpring>();
	coil->setRestLength(glm::length(currentParticle->getPosition() - otherParticle->getPosition()));
	coil->setEndParticles(currentParticle, otherParticle);
	coil->setSpringConstant(shearSpringConstant);
	coil->setDampeningConstant(shearDampeningConstant);
	this->shear.push_back(std::move(coil));
}
if (i < this->particleHeight - 1 && j < this->particleWidth - 1){
	ClothParticle* otherParticle = this->particles[bottomRight].get();
	std::unique_ptr<ClothSpring> coil = std::make_unique<ClothSpring>();
	coil->setRestLength(glm::length(currentParticle->getPosition() - otherParticle->getPosition()));
	coil->setEndParticles(currentParticle, otherParticle);
	coil->setSpringConstant(shearSpringConstant);
	coil->setDampeningConstant(shearDampeningConstant);
	this->shear.push_back(std::move(coil));
}


//Add Bending Springs
if (j < this->particleWidth - 2){
	ClothParticle* otherParticle = this->particles[rightRight].get();
	std::unique_ptr<ClothSpring> coil = std::make_unique<ClothSpring>();
	coil->setRestLength(glm::length(currentParticle->getPosition() - otherParticle->getPosition()));
	coil->setEndParticles(currentParticle, otherParticle);
	coil->setSpringConstant(bendingSpringConstant);
	coil->setDampeningConstant(bendingDampeningConstant);
	this->bend.push_back(std::move(coil));
}
if (i < this->particleHeight - 2){
	ClothParticle* otherParticle = this->particles[bottomBottom].get();
	std::unique_ptr<ClothSpring> coil = std::make_unique<ClothSpring>();
	coil->setRestLength(glm::length(currentParticle->getPosition() - otherParticle->getPosition()));
	coil->setEndParticles(currentParticle, otherParticle);
	coil->setSpringConstant(bendingSpringConstant);
	coil->setDampeningConstant(bendingDampeningConstant);
	this->bend.push_back(std::move(coil));
}
			}
		}

		int k = 0;
		for (int i = 0; i < this->particleHeight - 1; ++i) {
			for (int j = 0; j < this->particleWidth; ++j) {
				this->vertextIndex[k++] = i * this->particleWidth + j;
				this->vertextIndex[k++] = (i + 1) * this->particleWidth + j;
			}
		}

		glGenVertexArrays(1, &this->objectVAO);
		glGenBuffers(1, &this->vbuffer);
		glGenBuffers(1, &this->nbuffer);
		glGenBuffers(1, &this->ibuffer);
		glGenBuffers(1, &this->tbuffer);

		Texture* texture = loadTexture("Texture/texture.jpg");
		glGenTextures(1, &this->textureID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->textureID);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texture->width, texture->height);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		clothShaders = new Shaders((char*)"Shader/clothShadervs.vert", (char*)"Shader/clothShaderfs.frag");

		this->model = glm::mat4(1.0f);
	}

	void renderGeometry(glm::mat4 projection, glm::mat4 view) {
		int vPosition, vNormal, vTextCoord;
		for (int i = 0; i < this->particleHeight; i++) {
			for (int j = 0; j < this->particleWidth; j++) {
				const int current = i * this->particleWidth + j;
				this->vertexPosition[current] = this->particles[current]->getPosition();
				this->vertexTextColor[current] = glm::vec2((float)j / (this->particleWidth - 1), (float)i / (this->particleHeight - 1));
			}
		}

		int index = 0;
		for (int i = 0; i < this->particleHeight - 1; i++) {
			for (int j = 0; j < this->particleWidth - 1; j++) {
				glm::vec3 normal;

				const int current = i * this->particleWidth + j;
				const int right = current + 1;
				const int bottom = current + this->particleWidth;
				const int bottomRight = bottom + 1;

				this->vertextIndex[index++] = current;
				this->vertextIndex[index++] = bottom;
				this->vertextIndex[index++] = bottomRight;

				normal = glm::normalize(
					glm::cross(this->vertexPosition[bottomRight] - this->vertexPosition[current], this->vertexPosition[bottom] - this->vertexPosition[current])
				);
				this->vertextNormal[current] += normal;
				this->vertextNormal[bottom] += normal;
				this->vertextNormal[bottomRight] += normal;


				this->vertextIndex[index++] = current;
				this->vertextIndex[index++] = bottomRight;
				this->vertextIndex[index++] = right;

				normal = glm::normalize(
					glm::cross(this->vertexPosition[right] - this->vertexPosition[current], this->vertexPosition[bottomRight] - this->vertexPosition[current])
				);
				this->vertextNormal[current] += normal;
				this->vertextNormal[bottomRight] += normal;
				this->vertextNormal[right] += normal;
			}
		}

		for (int i = 0; i < this->particleNum; i++) {
			this->vertextNormal[i] = normalize(this->vertextNormal[i]);
		}

		clothShaders->useShader();

		glBindVertexArray(this->objectVAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->vbuffer);
		glBufferData(GL_ARRAY_BUFFER, 3 * this->vertexPosition.size() * sizeof(GLfloat), &this->vertexPosition[0].x, GL_STATIC_DRAW);
		vPosition = glGetAttribLocation(this->clothShaders->getShaderID(), "vPosition");
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(vPosition);

		glBindBuffer(GL_ARRAY_BUFFER, this->nbuffer);
		glBufferData(GL_ARRAY_BUFFER, 3 * this->vertextNormal.size() * sizeof(GLfloat), &this->vertextNormal[0].x, GL_STATIC_DRAW);
		vNormal = glGetAttribLocation(this->clothShaders->getShaderID(), "vNormal");
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(vNormal);

		glBindBuffer(GL_ARRAY_BUFFER, this->tbuffer);
		glBufferData(GL_ARRAY_BUFFER, 2 * this->vertexTextColor.size() * sizeof(GLfloat), &this->vertexTextColor[0].x, GL_STATIC_DRAW);
		vTextCoord = glGetAttribLocation(this->clothShaders->getShaderID(), "vTextCoord");
		glVertexAttribPointer(vTextCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(vTextCoord);

		clothShaders->useShader();

		glm::mat4 modelViewProjection = projection * view * this->model;

		clothShaders->setMat4("modelViewProjection", modelViewProjection);
		//clothShaders->setMat4("model", this->model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->textureID);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(this->objectVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->vertextIndex.size() * sizeof(GLint), &this->vertextIndex[0], GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES, this->vertextIndex.size(), GL_UNSIGNED_INT, (void*)0);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void updateGeometry(float t) {
		float newTime = t / 10.0f;

		for (int i = 0; i < 10; i++) {
			for (auto& coil : this->structural) {
				coil->updateGeometry(newTime);
			}
			for (auto& coil : this->shear) {
				coil->updateGeometry(newTime);
			}
			for (auto& coil : this->bend) {
				coil->updateGeometry(newTime);
			}

			for (auto& particle : this->particles) {
				particle->updateGeometry(newTime);
			}

			for (int i = 0; i < 4; ++i) {
				for (auto& coil : this->structural) {
					coil->checkStretch();
				}
				for (auto& coil : this->shear) {
					coil->checkStretch();
				}
			}
		}
	}

private:
	std::vector<std::unique_ptr<ClothParticle>> particles;
	std::vector<std::unique_ptr<ClothSpring>> structural;
	std::vector<std::unique_ptr<ClothSpring>> shear;
	std::vector<std::unique_ptr<ClothSpring>> bend;

	GLuint objectVAO;
	GLuint vbuffer, nbuffer, cbuffer, tbuffer, ibuffer;
	GLuint textureID;
	glm::mat4 model;

	std::vector<glm::vec3> vertexPosition;
	std::vector<glm::vec3> vertextNormal;
	std::vector<glm::vec3> vertexColor;
	std::vector<glm::vec2> vertexTextColor;
	std::vector<GLint> vertextIndex;

	Shaders* clothShaders;

	int particleNum;
	int particleWidth, particleHeight;

	float mass;
};

#endif