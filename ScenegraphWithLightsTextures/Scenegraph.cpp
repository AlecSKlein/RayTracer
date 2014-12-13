#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include "Ray.h"
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <SFML\Graphics.hpp>
#include "Node.h"
#include "Ray.h"
#include "HitRecord.h"
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>

Scenegraph::Scenegraph()
{
	image = false;
    root = NULL;
}

void Scenegraph::makeScenegraph(Node *r)
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
    this->root = r;
}



Scenegraph::~Scenegraph()
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
}

void Scenegraph::initShaderProgram(GLint shaderProgram)
{
	program = shaderProgram;

	textureMatrixLocation = glGetUniformLocation(shaderProgram,"texturematrix");
	textureLocation = glGetUniformLocation(shaderProgram, " ");

	projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    modelviewLocation = glGetUniformLocation(shaderProgram, "modelview");
    normalMatrixLocation = glGetUniformLocation(shaderProgram, "normalmatrix");
    numLightsLocation = glGetUniformLocation(shaderProgram, "numLights");

	mat_shininessLocation = glGetUniformLocation(shaderProgram, "material.shininess");
    mat_ambientLocation = glGetUniformLocation(shaderProgram, "material.ambient");
    mat_diffuseLocation = glGetUniformLocation(shaderProgram, "material.diffuse");
    mat_specularLocation = glGetUniformLocation(shaderProgram, "material.specular");
    
	

}

vector<float> Scenegraph::raytrace(int width, int height, stack<glm::mat4>& modelview)
{
	vector<float> buffer;
	Ray ray;
	sf::Image pic;
	pic.create(width, height);
	lights.clear();
    root->returnLights(lights, modelview);

	cout << "Beginning to trace" << endl;
	cout << "0% done tracing!" << endl;
	for(int w=0; w<width; w++)
	{
		if(width/4 == w)
			cout << "25% done tracing!" << endl;
		else if(width/2 == w)
			cout << "50% done tracing!" << endl;
		else if(width/4 + width/2 == w)
			cout << "75% done tracing!" << endl;
		for(int h=0; h<height; h++)
		{
			ray.setPoint(glm::vec4(0,0,0,1));
			ray.setDirection(glm::vec4(w-(width/2), (height/2)-h, -height/(2*tan(glm::radians(60.0))), 0));

			raycast(ray, modelview, theColor, 0);
			pic.setPixel(w, h, theColor);
		}
	}
	pic.saveToFile("output.png");
	cout << "100% done tracing, file saved!" << endl;
	return buffer;
}

bool Scenegraph::raycast(Ray r, stack<glm::mat4>& modelview, sf::Color& color, int iterations)
{
	HitRecord hr;
	if(root->intersect(r, hr, modelview))
	{
		color = shade(hr, lights, modelview);

		if(hr.getMaterial().getReflection() > 0 && iterations < 5)
		{
			Ray reflectRay;
			sf::Color colora; //absorb

			reflectRay.setDirection(glm::normalize(glm::reflect(glm::normalize(r.getRay()), glm::normalize(hr.getNormal()))));
			reflectRay.setPoint(hr.getIntersection() + (glm::normalize(hr.getNormal()))*0.01f);

			raycast(reflectRay, modelview, colora, iterations+1);
			color.r = (color.r * hr.getMaterial().getAbsorption()) + (colora.r * hr.getMaterial().getReflection());
			color.g = (color.g * hr.getMaterial().getAbsorption()) + (colora.g * hr.getMaterial().getReflection());
			color.b = (color.b * hr.getMaterial().getAbsorption()) + (colora.b * hr.getMaterial().getReflection());
		}
		return true;
	}
	else
	{
		color = color.Black;
		return false;
	}
}

sf::Color Scenegraph::shade(HitRecord hr, vector<Light> lights, stack<glm::mat4>& modelview)
{
	float nDotL,rDotV;
	glm::vec4 fColor;
	fColor.x = 0;
	fColor.y = 0;
	fColor.z = 0;
	fColor.w = 1;
	glm::vec3 normal, view, light, reflect, ambient, diffuse, specular;
	
	for(Light l : lights)
	{
		Ray rshade;
		HitRecord hrshade;

		rshade.setPoint(hr.getIntersection() + FLT_MIN * (l.getPosition() - hr.getIntersection()));
		rshade.setDirection(l.getPosition() - hr.getIntersection());
		hrshade.setTime(FLT_MAX);

		root->intersect(rshade, hrshade, modelview);

		if(!(hrshade.getTime() >= 0 && hrshade.getTime() <= 1))
		{
			if(l.getPosition().w != 0)
				light = glm::normalize(l.getPosition().xyz() - hr.getIntersection().xyz());
			else
				light = glm::normalize(-l.getPosition().xyz());

			glm::vec3 tNormal = hr.getNormal().xyz();
			normal = glm::normalize(tNormal);
			nDotL = glm::dot(normal, light);
			
			reflect = glm::reflect(-light, normal);
			reflect = glm::normalize(reflect);
			
			view = -hr.getIntersection().xyz();
			view = glm::normalize(view);

			rDotV = glm::max(glm::dot(reflect, view), (float)0.0);

			ambient = hr.getMaterial().getAmbient().xyz() * l.getAmbient().xyz();
			diffuse = hr.getMaterial().getDiffuse().xyz() * l.getDiffuse().xyz() * glm::max(nDotL,(float)0.0);
			if(nDotL > 0)
				specular = hr.getMaterial().getSpecular().xyz() * l.getSpecular().xyz() * glm::pow(rDotV, hr.getMaterial().getShininess());
			else
				specular = glm::vec3(0,0,0);

			fColor = fColor + glm::vec4(ambient+diffuse+specular, 1.0);
		}
	}

	Texture *tex = hr.getTexture();
	float rColor = 0.0f;
	float gColor = 0.0f;
	float bColor = 0.0f;

	tex->lookup(hr.getS(), hr.getT(), rColor, gColor, bColor);
	fColor.x = fColor.x * rColor;
	fColor.y = fColor.y * gColor;
	fColor.z = fColor.z * bColor;

	if(fColor.x > 1)
		fColor.x = 1;
	if(fColor.y > 1)
		fColor.y = 1;
	if(fColor.z > 1)
		fColor.z = 1;

	sf::Color col(fColor.x*255, fColor.y*255, fColor.z*255, fColor.w*255);
	return col;
}


void Scenegraph::draw(stack<glm::mat4>& modelView, bool stationary, glm::mat4 transforms, glm::mat4 trackball)
{
	
    if (root!=NULL)
    {
		lights.clear();
		lightlocation.clear();
		root->returnLights(lights,modelView);

		lightlocation.resize(lights.size());
		for( int i=0; i<lightlocation.size();i++){
		stringstream name;

        name << "light[" << i << "].ambient";

        lightlocation[i].ambientLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();
        name.str(std::string());

        name << "light[" << i << "].diffuse";
        lightlocation[i].diffuseLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();
        name.str(std::string());

        name << "light[" << i << "].specular";

        lightlocation[i].specularLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();
        name.str(std::string());

        name << "light[" << i << "].position";

        lightlocation[i].positionLocation = glGetUniformLocation(program,name.str().c_str());

        name.clear();
        name.str(std::string());
	}

	glUniform1i(numLightsLocation,lightlocation.size());

    for (int i=0;i<lights.size();i++)
    {
        glUniform3fv(lightlocation[i].ambientLocation,1,glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(lightlocation[i].diffuseLocation,1,glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(lightlocation[i].specularLocation,1,glm::value_ptr(lights[i].getSpecular()));
        glUniform4fv(lightlocation[i].positionLocation,1,glm::value_ptr(lights[i].getPosition()));
    }	
        root->draw(modelView);
    }

	if (root!=NULL)
	{
		root->updateBB();
		root->drawBB(modelView);
	}
}

