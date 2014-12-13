#ifndef _SCENEXMLREADER_H_
#define _SCENEXMLREADER_H_
#include "LeafNode.h"
#include "Material.h"
#include "Object.h"
#include "TransformNode.h"
#include "GroupNode.h"
#include "Scenegraph.h"
#include "Light.h"
#include <fstream>
#include <sstream>
#include <map>
using namespace std;
#include <rapidxml/rapidxml.hpp>
#include "OBJImporter.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"

class SceneXMLReader
{
public:

	bool importScenegraph(string filename,Scenegraph& sgraph)
	{
		ifstream file(filename);

		if (!file.is_open())
		{
			return false;
		}

		rapidxml::xml_document<> doc;
		rapidxml::xml_node<> * root_node;

		vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		buffer.push_back('\0');
		file.close();

		// Parse the buffer using the xml file parsing library into doc 
		try
		{
			doc.parse<0>(&buffer[0]);
		}
		catch (rapidxml::parse_error e)
		{
			cout << "Parsing unsuccessful: " << e.what() << endl;
			return false;
		}
		// Find our root node
		root_node = doc.first_node("scene");

		Node * root = NULL;
		bool bbDraw = false;

		int level = 0;

		string name;

		if (root_node->first_attribute("name")!=NULL)
		{
			name = root_node->first_attribute("name")->value();
		}
		else
		{
			stringstream s;
			s << "scene-";
			name = s.str();
		}

		if (root_node->first_attribute("BB")!=NULL)
		{
			string bb = root_node->first_attribute("BB")->value();
			if (bb=="true")
			{
				bbDraw = true;
			}
		}

		root = createGroupTree(root_node->first_node(NULL),name,&sgraph,level);



		if (root ==  NULL)
			return false;
		root->setBBDraw(bbDraw);

		sgraph.makeScenegraph(root);

		return true;
	}

private:
	Node * createGroupTree(rapidxml::xml_node<> *first_node,string nameOfGroup,Scenegraph *sgraph,int level)
	{
		vector<Node *> nodes;
		string name;
		int i;
		rapidxml::xml_node<> *node;
		Node *n = new GroupNode(sgraph,nameOfGroup);
		nodeMap[nameOfGroup] = n;


		for (i=1,node = first_node;node!=NULL;node = node->next_sibling(NULL),i++)
		{
			if (strcmp(node->name(),"group")==0)
			{
				bool bbDraw = false;

				if (node->first_attribute("name")!=NULL)
				{
					name = node->first_attribute("name")->value();
				}
				else
				{
					stringstream s;
					s << nameOfGroup << "." << level << "." << i;
					name = s.str();
				}

				if (node->first_attribute("BB")!=NULL)
				{
					string bb = node->first_attribute("BB")->value();
					if (bb=="true")
					{
						bbDraw = true;
					}
				}

				Node *child;

				if (node->first_attribute("copyof")!=NULL)
				{
					string copyof = node->first_attribute("copyof")->value();
					if (nodeMap.count(copyof)>0)
					{
						child = nodeMap[copyof]->clone();
						child->setName(name);
						nodeMap[name] = child;
					}
					else
					{
						return NULL; //no original exists
					}
				}
				else
				{
					child = createGroupTree(node->first_node(NULL),name,sgraph,level+1);
				}

				
				if (child!=NULL)
				{
					child->setBBDraw(bbDraw);						
					((GroupNode *)n)->addChild(child);
				}
				else
				{
					return NULL; //error
				}
			}
			else if (strcmp(node->name(),"transform")==0)
			{
				bool bbDraw = false;

				if (node->first_attribute("name")!=NULL)
				{
					name = node->first_attribute("name")->value();
				}
				else
				{
					stringstream s;
					s << nameOfGroup << "." << level;
					name = s.str();
				}

				if (node->first_attribute("BB")!=NULL)
				{
					string bb = node->first_attribute("BB")->value();
					if (bb=="true")
					{
						bbDraw = true;
					}
				}
				
				Node * child = createTransformTree(node->first_node(NULL),name,sgraph,level+1);
				if (child!=NULL)
				{
					child->setBBDraw(bbDraw);
					((GroupNode *)n)->addChild(child);
				}
				else
				{
					return NULL; //error
				}
				
			}
			else if (strcmp(node->name(),"object")==0)
			{
				if (node->first_attribute("name")!=NULL)
				{
					name = node->first_attribute("name")->value();
				}
				else
				{
					stringstream s;
					s << nameOfGroup << "." << level;
					name = s.str();
				}

				Node *child = createObjectTree(node,sgraph,name);
				
				if (child!=NULL)
				{
					((GroupNode *)n)->addChild(child);
				}
				else
				{
					return NULL; //error
				}

			}
			else if (strcmp(node->name(),"instance")==0)
			{
				string path;
				if (node->first_attribute("name")!=NULL)
				{
					name = node->first_attribute("name")->value();
				}
				else
				{
					return NULL;
				}

				if (node->first_attribute("path")!=NULL)
				{
					path = node->first_attribute("path")->value();
				}
				else
				{
					return NULL;
				}

				TriangleMesh mesh;
				OBJImporter::importFile(mesh,path,false);
				Object * obj = new Object(name);
				obj->init(mesh);
				sgraph->addInstance(obj);				
			}
			else if (strcmp(node->name(),"image")==0)
			{
				string path;
				if (node->first_attribute("name")!=NULL)
				{
					name = node->first_attribute("name")->value();
				}
				else
				{
					return NULL;
				}

				if (node->first_attribute("path")!=NULL)
				{
					path = node->first_attribute("path")->value();
				}
				else
				{
					return NULL;
				}

				//create texture
				Texture *tex = new Texture();
				tex->createImage(path);
				tex->setName(name);
				sgraph->addTexture(tex);
			}
			//lights
			else if (strcmp(node->name(),"light")==0)
			{
				Light l;
				l = getLight(node->first_node(NULL));

				n->addLight(l);
			}
			
		}
		return n;
	}

	Node * createObjectTree(rapidxml::xml_node<> *node,Scenegraph *sgraph,string nameOfObject)
	{
		string name,instanceOf;
		bool bbDraw = false;
		Texture *tex=NULL;

		if (node->first_attribute("instanceof")!=NULL)
		{
			instanceOf = node->first_attribute("instanceof")->value();
		}
		else
		{
			return NULL;
		}

		if (node->first_attribute("name")!=NULL)
		{
			name = node->first_attribute("name")->value();
		}
		else
		{
			name = nameOfObject;
		}

		if (node->first_attribute("BB")!=NULL)
		{
			string bb = node->first_attribute("BB")->value();
			if (bb=="true")
			{
				bbDraw = true;
			}
		}
		
		if (node->first_attribute("texture")!=NULL)
		{
			name = node->first_attribute("texture")->value();
			//attach texture
			tex = sgraph->getTexture(name);
		}
		else{
			name = "white";
			tex = sgraph->getTexture(name);
		}

		Object *obj = sgraph->getInstance(instanceOf);
		if (obj==NULL)
			return NULL;

		Node *child = new LeafNode(obj,sgraph,name);
		child->setBBDraw(bbDraw);
		//SET TEXTURE
		if (tex!=NULL)
			((LeafNode *)child)->setTexture(tex);
		nodeMap[name] = child;

		//get the material
		rapidxml::xml_node<> *mat_node = node->first_node("material");
		Material mat;
		for (rapidxml::xml_node<> *mat_prop = mat_node->first_node(NULL);mat_prop!=NULL;mat_prop = mat_prop->next_sibling())
		{
			if (strcmp(mat_prop->name(),"ambient")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				mat.setAmbient(r,g,b);
			}
			else if (strcmp(mat_prop->name(),"diffuse")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				mat.setDiffuse(r,g,b);
			}
			else if (strcmp(mat_prop->name(),"specular")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				mat.setSpecular(r,g,b);
			}			
			else if (strcmp(mat_prop->name(),"color")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				mat.setAmbient(r,g,b);
				mat.setDiffuse(r,g,b);
				mat.setSpecular(r,g,b);
				mat.setShininess(1.0f);
			}
			else if (strcmp(mat_prop->name(),"shininess")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float shininess;

				s >> shininess;
				mat.setShininess(shininess);
			}
			else if (strcmp(mat_prop->name(),"absorption")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float absorption;

				s >> absorption;
				mat.setAbsorption(absorption);
			}
			else if (strcmp(mat_prop->name(),"reflection")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float reflection;

				s >> reflection;
				mat.setReflection(reflection);
			}
			else if (strcmp(mat_prop->name(),"transparency")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float transparency;

				s >> transparency;
				mat.setTransparency(transparency);
			}
			else if (strcmp(mat_prop->name(),"refractive")==0)
			{
				stringstream s;
				s << mat_prop->value();
				float t;

				s >> t;
				mat.setRefractiveIndex(t);
			}
		}
		((LeafNode *)child)->setMaterial(mat);

		//lights
		for (rapidxml::xml_node<> *light_node = node->first_node("light");light_node!=NULL;light_node = light_node->next_sibling("light"))
		{
			Light l;
			l = getLight(light_node->first_node(NULL));

			child->addLight(l);
		}

		return child;
	}

	Node * createTransformTree(rapidxml::xml_node<> *first_node,string nameOfTransform,Scenegraph *sgraph,int level)
	{
		rapidxml::xml_node<> * start = first_node;
		if (strcmp(start->name(),"set")!=0)
		{
			return NULL; //transform node should begin with set
		}
		glm::mat4 transform;

		Node *transformNode = new TransformNode(sgraph,nameOfTransform);
		nodeMap[nameOfTransform] = transformNode;

		for (rapidxml::xml_node<> *t = start->first_node(NULL);t!=NULL;t = t->next_sibling(NULL))
		{
			if (strcmp(t->name(),"translate")==0)
			{
				stringstream s;
				s << t->value();
				float tx,ty,tz;

				s >> tx >> ty >> tz;
						
				transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz)) * transform;
			}
			else if (strcmp(t->name(),"rotate")==0)
			{
				stringstream s;
				s << t->value();
				float angle,ax,ay,az;

				s >> angle >> ax >> ay >> az;
						
				transform = glm::rotate(glm::mat4(1.0),glm::radians(angle),glm::vec3(ax,ay,az)) * transform;
			}
			else if (strcmp(t->name(),"scale")==0)
			{
				stringstream s;
				s << t->value();
				float sx,sy,sz;

				s >> sx >> sy >> sz;
						
				transform = glm::scale(glm::mat4(1.0),glm::vec3(sx,sy,sz)) * transform;
			}
			
		}

		((TransformNode *)transformNode)->setTransform(transform);
		
		
		for (start = start->next_sibling(NULL);start!=NULL;start = start->next_sibling(NULL))
		{
			string name;

			if ((strcmp(start->name(),"group")==0) || (strcmp(start->name(),"object")==0) || (strcmp(start->name(),"transform")==0))  
			{
				if (start->first_attribute("name")!=NULL)
				{
					name = start->first_attribute("name")->value();
				}
				else
				{
					stringstream s;
					s << nameOfTransform << "." << level;
					name = s.str();
				}
				Node *child;
		
				if (strcmp(start->name(),"group")==0)
				{
					bool bbDraw = false;
					if (start->first_attribute("copyof")!=NULL)
					{
						string copyof = start->first_attribute("copyof")->value();
						if (nodeMap.count(copyof)>0)
						{
							child = nodeMap[copyof]->clone();
							child->setName(name);
							nodeMap[name] = child;
						}
					}
					else
					{
						child = createGroupTree(start->first_node(NULL),name,sgraph,level+1);
					}

			
					if (start->first_attribute("BB")!=NULL)
					{
						string bb = start->first_attribute("BB")->value();
						if (bb=="true")
						{
							bbDraw = true;
						}
					}

					if (child!=NULL)
					{
						child->setBBDraw(bbDraw);
					}
				}
				else if (strcmp(start->name(),"object")==0)
					child = createObjectTree(start,sgraph,name);
				else if (strcmp(start->name(),"transform")==0)
					child = createTransformTree(start->first_node(NULL),name,sgraph,level+1);

				((TransformNode *)transformNode)->setChild(child);
			}
			else if (strcmp(start->name(),"light")==0) 
			{
				//light
				Light l;
				l = getLight(start->first_node(NULL));

				transformNode->addLight(l);
			}
		}
		return transformNode;
	}

	Light getLight(rapidxml::xml_node<> *light_node)
	{
		Light l;

		for (rapidxml::xml_node<> *light_prop = light_node;light_prop!=NULL;light_prop = light_prop->next_sibling())
		{
//			cout << "light name: " << light_prop->name() << endl; 
			if (strcmp(light_prop->name(),"ambient")==0)
			{
				stringstream s;
				s << light_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				l.setAmbient(r,g,b);
			}
			else if (strcmp(light_prop->name(),"diffuse")==0)
			{
				stringstream s;
				s << light_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				l.setDiffuse(r,g,b);
			}
			else if (strcmp(light_prop->name(),"specular")==0)
			{
				stringstream s;
				s << light_prop->value();
				float r,g,b;

				s >> r >> g >> b;
				l.setSpecular(r,g,b);
			}			
			else if (strcmp(light_prop->name(),"position")==0)
			{
				stringstream s;
				s << light_prop->value();
				glm::vec3 pos;

				s >> pos[0] >> pos[1] >> pos[2];
				l.setPosition(pos);
			}
			else if (strcmp(light_prop->name(),"direction")==0)
			{
				stringstream s;
				s << light_prop->value();
				glm::vec3 dir;

				s >> dir[0] >> dir[1] >> dir[2];
			//	l.setDirection(dir);
			}
			else if (strcmp(light_prop->name(),"spotdirection")==0)
			{
				stringstream s;
				s << light_prop->value();
				glm::vec3 spotdir;

				s >> spotdir[0] >> spotdir[1] >> spotdir[2];
				l.setSpotDirection(spotdir);
			}
			else if (strcmp(light_prop->name(),"spotangle")==0)
			{
				stringstream s;
				s << light_prop->value();
				float spot_angle;

				s >> spot_angle;
				l.setSpotAngle(spot_angle);
			}			
		}
		return l;
	}

	private:
		map<string,Node *> nodeMap;

};
#endif
