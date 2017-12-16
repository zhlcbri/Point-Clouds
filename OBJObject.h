#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OBJObject
{
private:
std::vector<unsigned int> indices;
/*std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
glm::mat4 toWorld; */

glm::mat4 spinMat;
glm::mat4 translateMat;
glm::mat4 scaleMat;
   
/*
glm::mat4 modelViewMat; // C^(-1)M
glm::mat4 projectionMat; // P
glm::mat4 viewPortMat; // D
glm::vec4 pixelVec; // P', vec4
*/
    
float angle;
    
//static float* pixels;
    
//static float* pixels /*= new float[Window::width * Window::height * 3]*/;
    
public:
	OBJObject(const char* filepath);
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    glm::mat4 toWorld;
    
	void parse(const char* filepath);
	void draw();

    void update();
    //void multiplyMat();
    void spin(float);
    void moveX(float);
    void moveY(float);
    void moveZ(float);
    void scale(float);
    void orbit(float);
    void resetPosition();
    void resetOrientation();

};

#endif




