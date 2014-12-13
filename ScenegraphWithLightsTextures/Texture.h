#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SFML/Graphics/Image.hpp>
#include <string>
using namespace std;
class Texture
{
  public:
    Texture();
    virtual ~Texture();
    unsigned int getTextureID();
    void setTextureID(unsigned int);
    bool createImage(string& filename);
    string getName();
    void setName(string& name);
    void lookup(float x,float y,float& r,float& g,float& b);
    int getWidth()
    {
      if (image!=NULL)
		  return image->getSize().x;
	  else
		  return 0;
    }
    int getHeight()
    {
      if (image!=NULL)
		  return image->getSize().y;
	  else
		  return 0;
    }
  protected:
    unsigned int texID;

	sf::Image *image;
    string name;
    void deleteImage();
    void saveRawBitmap(char *pixels,int w,int h);
};
#endif
