#ifndef NODE_H
#define NODE_H

#include <string>
#include <map>
#include <stack>
#include <iostream>
using namespace std;
#include "Light.h"
#include <glm/glm.hpp>
#include <vector>
#include "Ray.h"
#include "HitRecord.h"
#include <glm/vec4.hpp>
class Scenegraph;

class Node
{
protected:
    string name;
    Scenegraph *scenegraph;
	bool bbDraw;
	glm::vec3 minBounds,maxBounds;
	vector<Light> lights;
public:
    Node(Scenegraph *graph,string name="")
    {
		this->parent = NULL;
        scenegraph = graph;
        setName(name);
		bbDraw = false;
    }

    virtual ~Node()
    {
	}

	virtual Node *getNode(string name)
	{
		if (this->name == name)
			return this;

		return NULL;
	}

	virtual bool intersect(Ray r, HitRecord& hr, stack<glm::mat4>& modelView)=0;
    virtual void draw(stack<glm::mat4>& modelView)=0;
	virtual void drawBB(stack<glm::mat4>& modelView)=0;
	virtual void updateBB()=0;
	virtual Node *clone()=0;
	void setParent(Node *parent)
	{
		this->parent = parent;
	}

    void setName(const string& name)
    {
        this->name = name;
    }

	string getName(){
		return this->name;
	}

	void setBBDraw(bool d)
	{
		bbDraw = d;
	}

	glm::vec3 getMinBounds()
	{
		return minBounds;
	}

	glm::vec3 getMaxBounds()
	{
		return maxBounds;
	}

	void addLight(const Light& l)
	{
		lights.push_back(l);
		cout << "Light added in node " << name << endl;
		
	}

	virtual void returnLights(vector<Light>& vLights,stack<glm::mat4>& modelView){

	}
	vector<Light> getLights(){
		return lights;
	}

	Node * getParent(){
		if(this->parent != NULL)
			return this->parent;

		return NULL;
	}

	virtual glm::mat4 getTransform(){
		return glm::mat4(1.0);
	}

protected:
	Node *parent;
};

#endif // NODE_H
