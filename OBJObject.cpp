#include "OBJObject.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
//float tempAngle = 0.0;
float xVal, yVal, zVal, degVal = 0.0f;
float sVal = 1.0f;


OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
}

/*void OBJObject::resizePixels(int width, int height) {
    delete[] pixels;
    pixels = new float[window_width * window_height * 3];
}*/

void OBJObject::parse(const char *filepath)
{
    ifstream ifs;
    float x,y,z; // vertex coordinates
    float r = 0.0;
    float g = 0.0;
    float b = 0.0; // vertex color
    //int c1,c2; // characters read from file
    char c1, c2;
    char buf[255]; // buffer for reading line for filepath
    string strbuf;
    
    ifs.open(filepath);
    if (!ifs.is_open()) {
        cerr << "error loading the file" << endl;
        exit(-1);
    } // in case the file can't be found or is corrupt
    
    while (!ifs.eof()) {
        ifs.get(c1);
        ifs.get(c2);
        getline(ifs, strbuf);
        strbuf.copy(buf, strbuf.length());
        
        if ((c1 == 'v') && (c2 == ' ')) {
            //cout << buffer << endl;
            sscanf(buf, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
            //cout << "x y z: " << x << " " << y << " " << z << endl;
            
            vertices.push_back(glm::vec3(x, y, z)); // do not store the rgb values here
            
        } else if ((c1 == 'v') && (c2 == 'n')) {
            //cout << "vn line read" << endl;
            sscanf(buf, " %f %f %f", &x, &y, &z);
            
            // map the vertex normals in the range of 0 to 1
            // and store as rgb value
            glm::vec3 normalized = glm::normalize(glm::vec3(x, y, z)); // normalize the vertex normals
            normals.push_back(glm::vec3((normalized.x+1.0f)/2.0f, (normalized.y+1.0f)/2.0f, (normalized.z+1.0f)/2.0f));
        }
    }
    ifs.close(); // close the file when done parsing
}

void OBJObject::draw() 
{
	glMatrixMode(GL_MODELVIEW);

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));

	glBegin(GL_POINTS);
    
	// Loop through all the vertices of this OBJ Object and render them
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
        glColor3f(normals[i].r, normals[i].g, normals[i].b); // xyz or rgb?
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}
    
	glEnd();

	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}

////////////


// Copied from Cube class
void OBJObject::update()
{
    spin(1.0f);
    //this->toWorld = spinMat * this->toWorld;
}

void OBJObject::spin(float deg)
{
    //this->angle += deg;
    //if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    // This creates the matrix to rotate the cube
    //this->toWorld = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    
    float tempAngle = 0.0f;
    tempAngle += deg;
    if (tempAngle > 360.0f || tempAngle < -360.0f) tempAngle = 0.0f;
    spinMat = glm::rotate(glm::mat4(1.0f), tempAngle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    this->toWorld = this->toWorld * spinMat; // object-space
}

void OBJObject::moveX(float val) {
    translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(val, 0.0f, 0.0f));
    this->toWorld = translateMat * this->toWorld; // world-space
    xVal += val;
}

void OBJObject::moveY(float val) {
    translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, val, 0.0f));
    this->toWorld = translateMat * this->toWorld;
    yVal += val;
}

void OBJObject::moveZ(float val) {
    translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, val));
    this->toWorld = translateMat * this->toWorld;
    zVal += val;
}

void OBJObject::scale(float val) {
    scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(val));
    this->toWorld = this->toWorld * scaleMat;
    sVal *= val;
}

void OBJObject::orbit(float deg) {
    this->toWorld = glm::rotate(glm::mat4(1.0f), deg/180 * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) * this->toWorld;
    degVal += deg;
}

void OBJObject::resetPosition() {
    
    /*glm::mat4 translateMat = glm::mat4(1.0f);
    translateMat = glm::translate(glm::mat4(1.0f), glm::vec3((-1)*xVal, (-1)*yVal, (-1)*zVal)); //
    this->toWorld = translateMat * this->toWorld;
    
    xVal = 0.0f;
    yVal = 0.0f;
    zVal = 0.0f;*/
    
    this->toWorld[3][0] = 0.0f;
    this->toWorld[3][1] = 0.0f;
    this->toWorld[3][2] = 0.0f;
}

//
void OBJObject::resetOrientation() {
    
//    glm::mat4 orbitMat = glm::mat4(1.0f);
//    glm::mat4 scaleMat = glm::mat4(1.0f);
//    
//    orbitMat = glm::rotate(glm::mat4(1.0f), ((-1)*degVal)/180 * glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
//    scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1/sVal));
//    
//    this->toWorld = orbitMat * this->toWorld * scaleMat;
    
    
    float tempX = this->toWorld[3][0];
    float tempY = this->toWorld[3][1];
    float tempZ = this->toWorld[3][2];
    
    this->toWorld = glm::mat4(1.0f);
    this->toWorld[3][0] = tempX;
    this->toWorld[3][1] = tempY;
    this->toWorld[3][2] = tempZ;
    // this->toWorld[3][3] = 1.0f;
    
//     sVal = 1.0f;
//     degVal = 0.0f;
}






















