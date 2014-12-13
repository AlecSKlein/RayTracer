#ifndef _LEAF_H_
#define _LEAF_H_
#include "Node.h"
#include "Object.h"
#include "Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include "Ray.h"
#include "HitRecord.h"

class LeafNode : public Node
{
public:

	LeafNode(Object *instanceOf,Scenegraph *graph,string name="")
		:Node(graph,name)
	{
		this->instanceOf = instanceOf;
		//default material
		material.setAmbient(0.5f,0.5f,0.5f);
		material.setDiffuse(0.5f,0.5f,0.5f);
		material.setSpecular(0.2f,0.2f,0.2f);
		material.setShininess(1);
		tex = NULL;
	}

	~LeafNode(void)
	{

	}

	Node *clone()
	{
		LeafNode *newclone = new LeafNode(instanceOf,scenegraph,name);
		newclone->setMaterial(material);

		return newclone;
	}

	virtual void draw(stack<glm::mat4> &modelView)
    {
		GLuint a;
        if (instanceOf!=NULL)
		{
			a = glGetError();

			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
			glUniformMatrix4fv(scenegraph->normalMatrixLocation,1,GL_FALSE,glm::value_ptr(glm::transpose(glm::inverse(modelView.top()))));
			glUniform3fv(scenegraph->mat_ambientLocation,1,glm::value_ptr(material.getAmbient()));
			glUniform3fv(scenegraph->mat_diffuseLocation,1,glm::value_ptr(material.getDiffuse()));
			glUniform3fv(scenegraph->mat_specularLocation,1,glm::value_ptr(material.getSpecular()));
			glUniform1f(scenegraph->mat_shininessLocation,material.getShininess());
			
			if(tex != NULL){
				glUniformMatrix4fv(scenegraph->textureMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex->getTextureID());
				glUniform1i(scenegraph->textureLocation, 0);
			}

			a = glGetError();
			instanceOf->draw();        
			a = glGetError();
		}
    }

	virtual void drawBB(stack<glm::mat4>& modelView)
	{
		if (bbDraw)
		{
			GLuint a;
			glm::mat4 bbTransform;
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			bbTransform = glm::translate(glm::mat4(1.0),0.5f*(minBounds+maxBounds)) * glm::scale(glm::mat4(1.0),maxBounds-minBounds);
			glm::vec4 color = glm::vec4(1,1,1,1);
			glUniform3fv(scenegraph->objectColorLocation,1,glm::value_ptr(color));
			glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top() * bbTransform));
			a = glGetError();
			scenegraph->getInstance("box")->draw();        		
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}
	}
	
	virtual void updateBB()
	{
		minBounds = instanceOf->getMinimumWorldBounds().xyz();
		maxBounds = instanceOf->getMaximumWorldBounds().xyz();
	}

	glm::vec4 getColor()
	{
		return material.getAmbient();
	}

	virtual void setMaterial(Material& mat)
	{
		material = mat;
	}

	Material getMaterial()
	{
		return material;
	}

	void setTexture(Texture *tex)
	{
		this->tex = tex;
	}

	Texture * getTexture(){
		return tex;
	}

	void addLight(const Light &l)
	{
		 lights.push_back(l);	
	}
	virtual void returnLights(vector<Light>& vLights, stack<glm::mat4>& modelView){
		for(Light l: lights){
			l.setPosition( modelView.top()*l.getPosition());
			 vLights.push_back(l);
		}
	}

	virtual glm::mat4 getTransform(){
		return glm::mat4(1.0);
	}

	virtual bool intersect(Ray ray, HitRecord& hr, stack<glm::mat4>& modelview)
	{
		bool result = false;
		float epsilon = 0.05;
		double a, b, c;
		double t1, t2;
		float t;

		ray.setPoint(glm::inverse(modelview.top())*ray.getPoint());
		ray.setDirection(glm::inverse(modelview.top())*ray.getRay());

		float rayRX = ray.getRay().x;
		float rayRY = ray.getRay().y;
		float rayRZ = ray.getRay().z;

		float rayPX = ray.getPoint().x;
		float rayPY = ray.getPoint().y;
		float rayPZ = ray.getPoint().z;

		if(!instanceOf->getName().compare("sphere") || !instanceOf->getName().compare("box"))
		{
			if(instanceOf->getName().compare("box") == 0){
				float maxDistance = -FLT_MAX;
				float minDistance  = FLT_MAX;

				if(rayRX != 0.0){
					float x1 = (-0.5 - rayPX)/rayRX;
					float x2 = (0.5 - rayPX)/rayRX;
					minDistance = min(minDistance, max(x1,x2));
					maxDistance = max(maxDistance, min(x1,x2));
				}

				if(rayRY != 0.0){
					float y1 = (-0.5 - rayPY)/rayRY;
					float y2 = (0.5 - rayPY)/rayRY;
					minDistance = min(minDistance, max(y1,y2));
					maxDistance = max(maxDistance, min(y1,y2));
				}

				if(rayRZ != 0.0){
					float z1 = (-0.5 - rayPZ)/rayRZ;
					float z2 = (0.5 - rayPZ)/rayRZ;
					minDistance = min(minDistance, max(z1,z2));
					maxDistance = max(maxDistance, min(z1,z2));
				}
	
				if(minDistance >= maxDistance)
				{
					hr.setTime(maxDistance);
					glm::vec4 poi = ray.getPoint() + maxDistance*ray.getRay(); //poi = point of intersection
					hr.setIntersection(modelview.top() * poi);
					float s, t;
					if(glm::abs(poi.x+.5)<epsilon)
					{
						hr.setNormal(modelview.top()*glm::vec4(-1,0,0,0));

						hr.setTexture(tex);
						s = (0.5f + poi.y);
						t = (0.5f + poi.z);
					}
					else if(glm::abs(poi.x-.5)<epsilon)
					{
						hr.setNormal(modelview.top()*glm::vec4(1,0,0,0));

						hr.setTexture(tex);
						s = (0.5f + poi.y);
						t = (0.5f + poi.z);
					}
					else if(glm::abs(poi.y+.5)<epsilon)
					{
						hr.setNormal(modelview.top()*glm::vec4(0,-1,0,0));

						hr.setTexture(tex);
						t = (0.5f + poi.x);
						s = (0.5f + poi.z);
					}
					else if(glm::abs(poi.y-.5)<epsilon)
					{
						hr.setNormal(modelview.top()*glm::vec4(0,1,0,0));

						hr.setTexture(tex);
						t = (0.5f + poi.x);
						s = (0.5f + poi.z);
					}
					else if(glm::abs(poi.z+.5)<epsilon)
					{
						hr.setNormal(modelview.top()*glm::vec4(0,0,-1,0));

						hr.setTexture(tex);
						t = (0.5f + poi.x);
						s = (0.5f + poi.y);
					}
					else if(glm::abs(poi.z-.5)<epsilon)
					{
						hr.setNormal(modelview.top()*glm::vec4(0,0,1,0));

						hr.setTexture(tex);
						t = (0.5f + poi.x);
						s = (0.5f + poi.y);
					}
					hr.setMaterial(material);
			
					if(s > 1-epsilon)
					{
						s = 1;
					}
					if(s < epsilon)
					{
						s = 0;
					}
					if(t > 1-epsilon)
					{
						t = 1;
					}
					if(t < epsilon)
					{
						t = 0;
					}
					hr.setS(s);
					hr.setT(t);
					result = true;
				}
			
				return minDistance >= maxDistance;
			}
			else if(instanceOf->getName().compare("sphere") == 0)
			{

				a = (pow(rayRX, 2) + pow(rayRY, 2) + pow(rayRZ, 2));
				b = (2*rayPX * rayRX) + (2*rayPY * rayRY) + (2*rayPZ * rayRZ);
				c = (pow(rayPX, 2) + pow(rayPY, 2) + pow(rayPZ, 2) - 1);
			
				double quadratic = b*b-4*a*c;

				if (quadratic < 0)
				{
					return false;
				}
				else
				{
					t1 = (-b + sqrt(quadratic))/(2*a);
					t2 = (-b - sqrt(quadratic))/(2*a);

					if (t1>t2)
					{
						double temp = t1;
						t1 = t2;
						t2 = temp;
					}

					if(t2 < 0)
					{
						return false;
					}
					else
					{
						result = true;
						if (t1<0)
							t = t2;
						else
							t = t1;
					}
				}
				if(t1>0||t2>0)
					result = true;
				if(result){
					if(hr.getTime() < 0 || hr.getTime() > t)
					{
						hr.setTime(t);
						hr.setIntersection(modelview.top() * (ray.getPoint() + t*ray.getRay()));
						hr.setNormal(glm::transpose(glm::inverse(modelview.top())) * 
						glm::vec4(
							(rayPX + t*rayRX),(rayPY + t*rayRY),(rayPZ + t*rayRZ),0)
						);
						hr.setMaterial(material);

						//Textures
						hr.setTexture(tex);
						glm::mat4 normalMatrix= glm::transpose(glm::inverse(modelview.top()));
						glm::vec4 normal = glm::vec4((rayPX +(t*rayRX)), (rayPY +(t*rayRY)), (rayPZ +(t*rayRZ)), 0.0f);
						hr.setNormal(normalMatrix * glm::vec4(normal.x,normal.y,normal.z,0.0f)); 
						hr.setNormal(normalMatrix * glm::vec4(normal.x,normal.y,normal.z,0.0f)); 
						hr.setIntersection(modelview.top() * glm::vec4(normal.x,normal.y,normal.z,1.0f));
						
						if(normal.x > 1)
							normal.x = 1;
						if(normal.y > 1)
							normal.y = 1;
						if(normal.z > 1)
							normal.z = 1;

						float PI = 3.14159f;
						float phi = asin(normal.y);
						float theta = atan2(normal.z, normal.x) + 2*PI;
						float s = 1-theta/(2*PI);
						float t = (phi+PI/2)/PI;
						if(s < 0)
							s++;
						hr.setS(s);
						hr.setT(t);

						hr.setTexture(tex);
					}
				}
			}
		}
		return result;
	}

protected:
	Object *instanceOf;
	Material material;
	Texture *tex;
};
#endif
