#ifndef RAY_H_
#define RAY_H_
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/gl.h>

class Ray{

protected:
	glm::vec4 point;
	glm::vec4 direction;

public:
	Ray(){}

	void setPoint(glm::vec4 pt){
		this->point = pt;
	}

	glm::vec4 getPoint(){
		return this->point;
	}

	void setDirection(glm::vec4 dir){
		this->direction = dir;
	}

	glm::vec4 getRay(){
		return this->direction;
	}

};

#endif