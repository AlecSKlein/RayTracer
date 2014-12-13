#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <map>
#include <stack>
#include <vector>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include "Object.h"
#include "Node.h"
#include "Texture.h"
#include "Ray.h"

class Scenegraph
{    
    friend class LeafNode;
	friend class GroupNode;
	friend class TransformNode;
		typedef struct
    {
        GLint positionLocation;
        GLint ambientLocation;
        GLint diffuseLocation;
        GLint specularLocation;
    } LightLocation;
public:

    Scenegraph();
    ~Scenegraph();
    void makeScenegraph(Node *root);
    void initShaderProgram(GLint shaderProgram);
    void draw(stack<glm::mat4>& modelView, bool stationary, glm::mat4 transforms,glm::mat4 trackball);
	vector<float> raytrace(int h, int w, stack<glm::mat4>& modelView);
	bool raycast(Ray r, stack<glm::mat4>& modelview, sf::Color& color, int iterations);
	sf::Color Scenegraph::shade(HitRecord theRec, vector<Light> lights, stack<glm::mat4>& modelview);

	void addInstance(Object *in)
	{
		instances.push_back(in);
	}

	void addTexture(Texture *tex)
	{
		cout << "Texture " << tex->getName() << " added" << endl;
		//textures.push_back(tex);
		textureMap[tex->getName()] = tex;
	}

	Object *getInstance(string name)
	{
		int i;

		i = 0;
		while ((i<instances.size()) && (instances[i]->getName()!=name))
			i++;

		if (i<instances.size())
		{
			return instances[i];
		}
		else
		{
			return NULL;
		}
	}

	Texture *getTexture(string name)
	{
		cout << "Texture " << name << " looked up" << endl;
		return textureMap[name];
	}

private:
    Node *root;
	//vector<Texture *> textures;
	std::map<string, Texture *> textureMap;
	vector<Object *> instances;
	vector<Light> lights;
	vector<LightLocation> lightlocation;
	GLint objectColorLocation;
	GLint projectionLocation,modelviewLocation,normalMatrixLocation,textureMatrixLocation, textureLocation;
    GLint numLightsLocation;
    GLint mat_ambientLocation,mat_diffuseLocation,mat_specularLocation,mat_shininessLocation;
	GLint program;
	bool image;

	sf::Color theColor;
};

#endif // SCENEGRAPH_H
