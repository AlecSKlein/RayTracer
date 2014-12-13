#ifndef OBJECTXMLREADER_H
#define OBJECTXMLREADER_H

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <GL/gl.h>
#include <vector>
#include <iostream>
#include "OBJImporter.h"
#include <sstream>
using namespace std;
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Object.h"
#include "Plane.h"
#include "Cone.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Box.h"





class ObjectXMLReader
{
public:
	ObjectXMLReader(){}
	~ObjectXMLReader(){}
    static vector<Object *> readObjectXMLFile(string filename) //should instead return the list of objects
	{
		vector<Object *> list;
		xercesc::SAXParser * parser;

		try 
		{
            xercesc::XMLPlatformUtils::Initialize();


        }
        catch (const xercesc::XMLException& toCatch) 
		{
            char* message = xercesc::XMLString::transcode(toCatch.getMessage());
            cout << "Error during initialization! :\n"
                 << message << "\n";
            xercesc::XMLString::release(&message);
            return list;
        }
		
		parser = new xercesc::SAXParser();
		
		parser->setDoNamespaces(true);

		ObjectXMLFileHandler *handler = new ObjectXMLFileHandler();
		xercesc::ErrorHandler *errHandler = (xercesc::ErrorHandler *)handler;

		parser->setDocumentHandler(handler);
		parser->setErrorHandler(errHandler);
		
	    try
		{
			parser->parse(filename.c_str());
		}
		catch (const xercesc::XMLException& e)
		{
			char *message = xercesc::XMLString::transcode(e.getMessage());
			cout << "Exception message is: " << message << endl;
			xercesc::XMLString::release(&message);
			return list;
		}

	    list = handler->getObjectList();	    

		delete parser;
		delete handler;
	    
	    return list;

	}

	class ObjectXMLFileHandler: public xercesc::HandlerBase
	{
		public:
		    void startDocument()
		    {
                obj = NULL;
		    }

		    void characters(const XMLCh* const chars, const XMLSize_t length)
		    {
		    	int c;
		    	float f;
				char *m = xercesc::XMLString::transcode(chars);

			    for (int i=0;i<length;i++)     
				{
                    data = data + m[i];
				}
				xercesc::XMLString::release(&m);
		    }

		    void startElement(const XMLCh* const name,xercesc::AttributeList& attributes)
		    {
				char *n = xercesc::XMLString::transcode(name);
				string qName(n);
				xercesc::XMLString::release(&n);
                cout << "Started element: " << qName << endl;
                if (qName.compare("plane")==0)
		    	{
                    string name;
                    
					if (attributes.getValue("name")!=NULL)
					{
						n = xercesc::XMLString::transcode(attributes.getValue("name"));
						name = string(n);
						xercesc::XMLString::release(&n);
					}                    
                   TriangleMesh m;
				   OBJImporter::importFile(m,string("models/plane"));
				   obj = new Object(name);
				   obj->init(m);
		    	}
		    	else if (qName.compare("sphere")==0)
		    	{
                    string name;

                    if (attributes.getValue("name")!=NULL)
					{
						n = xercesc::XMLString::transcode(attributes.getValue("name"));
						name = string(n);
						xercesc::XMLString::release(&n);
					}
                    obj = new Sphere(name);
                }
		    	else if (qName.compare("cylinder")==0)
		    	{
					stringstream ss;
					bool found = false;

                    
                    string name;

                    if (attributes.getValue("name")!=NULL)
					{
						n = xercesc::XMLString::transcode(attributes.getValue("name"));
						name = string(n);
						xercesc::XMLString::release(&n);
					}

                    obj = new Cylinder(name);
		    	}
		    	else if (qName.compare("cone")==0)
		    	{
                    string name;

                    if (attributes.getValue("name")!=NULL)
					{
						n = xercesc::XMLString::transcode(attributes.getValue("name"));
						name = string(n);
						xercesc::XMLString::release(&n);
					}
                    obj = new Cone(name);
		    	}
                else if (qName.compare("box")==0)
                {
                    string name;

                    if (attributes.getValue("name")!=NULL)
					{
						n = xercesc::XMLString::transcode(attributes.getValue("name"));
						name = string(n);
						xercesc::XMLString::release(&n);
					}
                    obj = new Box(name);
                }
                else if (qName.compare("light")==0)
		    	{
		    	}
		    	else if (qName.compare("mesh")==0)
                {
                }
		    	else if (qName.compare("image")==0)
                {
                }
		    	else if (qName.compare("texture")==0)
		    	{		    		
		    	}
		    }


		    void endElement(const XMLCh* const name)
		    {
				char *n = xercesc::XMLString::transcode(name);
				string qName(n);
				xercesc::XMLString::release(&n);
                cout << "Ended element: " << qName << endl;

				vector<float> param;
				float num;
				stringstream ss;

				ss << data;
				data = "";

				while (ss >> num)
				{
					param.push_back(num);
				}

		    	if (qName.compare("plane")==0)
		    	{
                    list.push_back(obj);
                    obj = NULL;
		    	}
		    	else if (qName.compare("sphere")==0)
		    	{
                    list.push_back(obj);
                    obj = NULL;
                }
		    	else if (qName.compare("cylinder")==0)
		    	{
                    list.push_back(obj);
                    obj = NULL;
		    	}
		    	else if (qName.compare("cone")==0)
		    	{
                    list.push_back(obj);
                    obj = NULL;
		    	}
                else if (qName.compare("box")==0)
                {
                    list.push_back(obj);
                    obj = NULL;
                }
                else if (qName.compare("mesh")==0)
		    	{
		    	}
		    	else if (qName.compare("image")==0)
		    	{
		    	
		    	}
		    	else if (qName.compare("texture")==0)
		    	{
		    	
		    	}
		    	else if (qName.compare("transform")==0)
		    	{
                    if (obj!=NULL)
                            obj->setTransform(m);
                    m = glm::mat4(1.0f);
		    	}
		    	else if (qName.compare("translate")==0)
		    	{
					if (param.size()!=3)
						return;

                    m = glm::translate(glm::mat4(1.0f),glm::vec3(param[0],param[1],param[2])) * m;
		    	}
		    	else if (qName.compare("rotate")==0)
                {
                    if (param.size()!=4)
						return;

                    m = glm::rotate(glm::mat4(1.0f),glm::radians(param[0]),glm::vec3(param[1],param[2],param[3])) * m;
                }
		    	else if (qName.compare("scale")==0)
		    	{
                    if (param.size()!=3)
						return;

                    m = glm::scale(glm::mat4(1.0f),glm::vec3(param[0],param[1],param[2])) * m;                    
		    	}
		    	else if (qName.compare("color")==0)
		    	{
                    if (param.size()!=3)
                        return;

                    if (obj!=NULL)
                        obj->setColor(param[0],param[1],param[2]); //set the object's color
                    data.clear();
		    	}
                else if (qName.compare("ambient")==0)
		    	{
		    		
		    	}
		    	else if (qName.compare("diffuse")==0)
		    	{
		    		
		    	}
		    	else if (qName.compare("specular")==0)
		    	{
		    		
		    	}
		    	else if (qName.compare("shininess")==0)
		    	{
		    		
		    	}
		    	else if (qName.compare("position")==0)
		    	{
                    if (param.size()!=3)
                        return;                    
		    	}
		    	else if (qName.compare("direction")==0)
		    	{
                    if (param.size()!=3)
                        return;                    
		    	}
		    	else if (qName.compare("spotdirection")==0)
		    	{
                    if (param.size()!=3)
                        return;               
		    	}
		    	else if (qName.compare("spotangle")==0)
		    	{
                    if (param.size()!=1)
						return;
		    	}
		    	else if (qName.compare("light")==0)
		    	{
		    	}
		    	else if (qName.compare("material")==0)
		    	{
		    	}		    	
		    }

		    vector<Object *> getObjectList()
		    {
		    	return list;
		    }

		private:

		    string data;
		    int index;
            Object *obj;
            glm::mat4 m;
		    vector<Object *>list;
		};



};



#endif /* OBJECTXMLREADER_H_ */
