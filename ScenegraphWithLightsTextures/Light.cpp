#include "Light.h"

Light::Light(void)
{
	init();
}

Light::~Light(void)
{
}

void Light::init()
{
    ambient = diffuse = specular = glm::vec3(0,0,0);
    position = glm::vec4(0,0,0,1);
    spot_cutoff = 180; //by default not a spotlight
}


void Light::setAmbient(float r,float g,float b)
{
    ambient = glm::vec3(r,g,b);
}

void Light::setDiffuse(float r,float g,float b)
{
    diffuse = glm::vec3(r,g,b);
}

void Light::setSpecular(float r,float g,float b)
{
    specular = glm::vec3(r,g,b);
}

void Light::setPosition(float x,float y,float z)
{
    position = glm::vec4(x,y,z,1.0f);
}

void Light::setDirection(float x,float y,float z)
{
    position = glm::vec4(x,y,z,0.0f);
}

void Light::setSpotDirection(glm::vec3 spotdir)
{
	spot_direction = glm::vec4(spotdir,0);
}

void Light::setAmbient(glm::vec3 color)
{
    ambient = color;
}

void Light::setDiffuse(glm::vec3 color)
{
    diffuse = color;
}

void Light::setSpecular(glm::vec3 color)
{
    specular = color;
}

void Light::setPosition(glm::vec3 pos)
{
    position = glm::vec4(pos,1.0f);
}
void Light::setPosition(glm::vec4 pos)
{
	pos.w =1.0f;
    position = pos;

}
void Light::setDirection(glm::vec3 dir)
{
    position = glm::vec4(dir,0.0f);
}

void Light::setSpotDirection(float x,float y,float z)
{
    spot_direction = glm::vec4(x,y,z,0.0f);
}

void Light::setSpotAngle(float angle)
{
	spot_cutoff = angle;
}


glm::vec3 Light::getAmbient()
{
	return ambient;
}

glm::vec3 Light::getDiffuse()
{
	return diffuse;
}

glm::vec3 Light::getSpecular()
{
	return specular;
}

glm::vec4 Light::getPosition()
{
	return position;
}

glm::vec4 Light::getSpotDirection()
{
	return spot_direction;
}

float Light::getSpotAngle()
{
	return spot_cutoff;
}

