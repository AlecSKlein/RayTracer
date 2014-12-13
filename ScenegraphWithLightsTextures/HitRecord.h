#ifndef HIT_RECORD_H_
#define HIT_RECORD_H_
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/gl.h>

#include "Material.h"
#include "Texture.h"



class HitRecord{

protected:
	float time;
	glm::vec4 intersection;
	glm::vec4 normal;
	Material mat;
	Texture *tex;
	float s;
	float t;


public:
	HitRecord()
	{
	}

	~HitRecord()
	{
	}

	void setTime(float t)
	{ 
		this->time = t; 
	}

	float getTime()
	{
		return this->time;
	}

	void setIntersection(glm::vec4 inter)
	{
		this->intersection = inter;
	}

	glm::vec4 getIntersection()
	{
		return this->intersection;
	}

	void setNormal(glm::vec4 n)
	{
		this->normal = n;
	}

	glm::vec4 getNormal()
	{
		return this->normal;
	}

	void setMaterial(Material mat)
	{
		this->mat = mat;
	}

	Material getMaterial()
	{
		return this->mat;
	}

	void setTexture(Texture *tex)
	{
		this->tex = tex;
	}

	Texture* getTexture()
	{
		return this->tex;
	}

	void setS(float s)
	{
		this->s = s;
	}

	float getS()
	{
		return this->s;
	}

	void setT(float t)
	{
		this->t = t;
	}

	float getT()
	{
		return this->t;
	}
};
#endif