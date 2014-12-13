#include "Texture.h"
#include <GL/glew.h>
#include <GL/GLU.h>
#include <iostream>
Texture::Texture()
{
  image = NULL;
  texID = -1;
}
Texture::~Texture()
{
  deleteImage();
}
unsigned int Texture::getTextureID()
{
  return texID;
}
bool Texture::createImage(string& filename)
{
  deleteImage();

  image = new sf::Image();
  image->loadFromFile(filename);
  image->flipVertically();

  glEnable(GL_TEXTURE_2D);
  glGenTextures(1,&this->texID);
  glBindTexture(GL_TEXTURE_2D,texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->getSize().x, image->getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->getPixelsPtr());
  //gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,image->getSize().x, image->getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, image->getPixelsPtr());
 
  //opengl4 way of generating mipmaps, no GLU dependency!
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glBindTexture(GL_TEXTURE_2D,0);
  glDisable(GL_TEXTURE_2D);
  return true;
}

void Texture::setName(string& name)
{
  this->name = name;
}

string Texture::getName()
{
  return name;
}
void Texture::lookup(float x,float y,float& r,float& g,float& b)
{
  float c1[3],c2[3];
  int i,j;
  x = x-(int)x;
  y = y-(int)y;
  x *= image->getSize().x;
  y *= image->getSize().y;
  i = (int)y;
  j = (int)x;
  if (j<0)
  {
    j = 0;
  }


  for (int k=0;k<3;k++)
  {
    switch (k)
	{
	case 0:
		c1[k] = (1-(x-j))*image->getPixel(j,i).r + (x-j)*image->getPixel(j+1,i).r;
		c2[k] = (1-(x-j))*image->getPixel(j,i+1).r + (x-j)*image->getPixel(j+1,i+1).r;
		break;
	case 1:
		c1[k] = (1-(x-j))*image->getPixel(j,i).g + (x-j)*image->getPixel(j+1,i).g;
		c2[k] = (1-(x-j))*image->getPixel(j,i+1).g + (x-j)*image->getPixel(j+1,i+1).g;
		break;
	case 2:
		c1[k] = (1-(x-j))*image->getPixel(j,i).b + (x-j)*image->getPixel(j+1,i).b;
		c2[k] = (1-(x-j))*image->getPixel(j,i+1).b + (x-j)*image->getPixel(j+1,i+1).b;
		break;		
	}
  }
  r = (1-(y-i))*c1[0] + (y-i)*c2[0];
  g = (1-(y-i))*c1[1] + (y-i)*c2[1];
  b = (1-(y-i))*c1[2] + (y-i)*c2[2];


  r/=255;
  g/=255;
  b/=255;
}
void Texture::deleteImage()
{
  if (image)
  {
    delete []image;
  }
  if (texID!=-1)
  {
    glDeleteTextures(1,&texID);
  }
}
void Texture::setTextureID(unsigned int ID)
{
  this->texID = ID;
}
