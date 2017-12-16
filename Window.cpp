 #include "Window.h"
using namespace std;

glm::mat4 modelViewMat; // C^(-1)M
glm::mat4 projectionMat; // P
glm::mat4 viewPortMat(0.0f); // D
glm::vec4 pixelVec; // P', vec4

const char* window_title = "GLFW Starter Project";
// store the 3 models in advance so that the models can be switched instantly
OBJObject obj("bunny.obj");
OBJObject bunny("bunny.obj");
OBJObject bear("bear.obj");
OBJObject dragon("dragon.obj");

int Window::width;
int Window::height;
GLfloat size = 10.0f;

struct Color    // generic color class
{
    float r, g, b;  // red, green, blue
};

//static int window_width = 512, window_height = 512;
static float* pixels;

int mode = 0; // 0 if use OpenGL; 1 if use rasterization to draw object

void Window::initialize_objects()
{
}

void Window::clean_up()
{
}

GLFWwindow* Window::create_window(int width, int height)
{
    
	// Initialize GLFW.
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}


void Window::resize_callback(GLFWwindow* window, int width, int height)
{
    //if (mode == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        Window::width = width;
        Window::height = height;
        
        // Set the viewport size
        glViewport(0, 0, width, height);
        // Set the matrix mode to GL_PROJECTION to determine the proper camera properties
        glMatrixMode(GL_PROJECTION);
        // Load the identity matrix
        glLoadIdentity();
        // Set the perspective of the projection viewing frustum
        gluPerspective(60.0, double(width) / (double)height, 1.0, 1000.0);
        // Move camera back 20 units so that it looks at the origin (or else it's in the origin)
        glTranslatef(0, 0, -20); //eye

        if(pixels != NULL) delete[] pixels;
        pixels = new float[width * height * 3];
    //} else if (mode == 1) {
        //resizeCallback(window, width, height);
    //}
}

void Window::idle_callback()
{
	// Perform any updates as necessary. Here, we will spin the cube/obj slightly.
	//cube.update();
    obj.update(); // spinMat
}

void Window::display_callback(GLFWwindow* window)
{
    if (mode == 0) {
        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Set the matrix mode to GL_MODELVIEW
        glMatrixMode(GL_MODELVIEW);
        // Load the identity matrix
        glLoadIdentity();
        
        // Render objects
        obj.draw();
        
        // Gets events, including input such as keyboard and mouse or window resizing
        glfwPollEvents();
        // Swap buffers
        glfwSwapBuffers(window);
    } else if (mode == 1){
        displayCallback(window); //rasterizer
    }
}

// Clear frame buffer
void Window::clearBuffer()
{
    
    Color clearColor = { 0.29f, 0.58f, 0.72f };
    
    for (int i = 0; i < Window::width * Window::height; ++i) {
        pixels[i * 3] = clearColor.r;
        pixels[i * 3 + 1] = clearColor.g;
        pixels[i * 3 + 2] = clearColor.b;
    }
}

// Draw a point into the frame buffer
void Window::drawPoint(int x, int y, float r, float g, float b, float size)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if ((x+i >= 0) && (x+i < Window::width) && (y+j >= 0) && (y+j < Window::height)) {
                int offset = (y+j) * Window::width * 3 + (x+i) * 3;
                pixels[offset] = r;
                pixels[offset + 1] = g;
                pixels[offset + 2] = b;
            }
        }
    }
}

void Window::rasterize() {
    
    // go over the point model and call drawPoint for every point in it
    for (int i = 0; i < obj.vertices.size(); ++i) {
        
        //cout << "obj.vertices[i].x y z: " << obj.vertices[i].x << ", " << obj.vertices[i].y << ", " << obj.vertices[i].z << endl;
        
        glm::vec4 p = glm::vec4(obj.vertices[i].x, obj.vertices[i].y, obj.vertices[i].z, 1.0f);
        
        setModelview();
        setProjection(Window::width, Window::height);
        setViewPort(Window::width, Window::height);
        
        // D * P * C^-1 * M * p
        pixelVec = viewPortMat * projectionMat * modelViewMat * obj.toWorld * p; // p'
        
        //cout << "pixelVec[0][1][2][3]: " << pixelVec[0] << ", " << pixelVec[1] << ", " << pixelVec[2] << ", " << pixelVec[3] << endl;
        
        float x = pixelVec[0]/pixelVec[3];
        float y = pixelVec[1]/pixelVec[3];
        
        drawPoint(x, y, obj.normals[i].x, obj.normals[i].y, obj.normals[i].z, size);
        
    }
}

void Window::displayCallback(GLFWwindow* window)
{
    clearBuffer();
    rasterize();
    
    // glDrawPixels writes a block of pixels to the framebuffer
    glDrawPixels(Window::width, Window::height, GL_RGB, GL_FLOAT, pixels);
    
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window);
}

///////////

// C^(-1)M: OpenGL GL_MODELVIEW matrix
void Window::setModelview() {
    // eye, center, up
    modelViewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

// P: OpenGL GL_PROJECTION matrix
void Window::setProjection(int window_width, int window_height) {
    Window::width = window_width;
    Window::height = window_height;
    // just converted 60 degree to radian
    projectionMat = glm::perspective(glm::radians(60.0f), float(Window::width) / (float)Window::height, 1.0f, 1000.0f);
}

// D: viewport matrix
void Window::setViewPort(float width, float height) {
    Window::width = width;
    Window::height = height;
    
    viewPortMat[0] = glm::vec4((width - 0.0f)/2, 0.0f, 0.0f, 0.0f);
    viewPortMat[1] = glm::vec4(0.0f, (height - 0.0f)/2, 0.0f, 0.0f);
    viewPortMat[2] = glm::vec4(0.0f, 0.0f, 0.5f, 0.0f);
    viewPortMat[3] = glm::vec4((width+0.0f)/2, (height+0.0f)/2, 0.5f, 1.0f);
}


void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        switch (key) {
            case GLFW_KEY_F1:
                obj = bunny;
                break;
                
            case GLFW_KEY_F2:
                obj = bear;
                break;
            case GLFW_KEY_F3:
                obj = dragon;
                break;
                
            case GLFW_KEY_P:
                if (mods == GLFW_MOD_SHIFT) {
                    // uppercase P increases point size by 3.0f
                    size += 0.5f;
                } else if (size - 2.0f > 0) {
                    //lowercase p decrease point size by 3.0f
                    size -= 0.5f;
                }
                glPointSize(size);
                break;
            
            case GLFW_KEY_X:
                //move left/right (along the x axis) by a small amount
                if (mods == GLFW_MOD_SHIFT) {
                    obj.moveX(0.5f);
                } else {
                    obj.moveX(-0.5f); // make sure it doesn't go out of the window
                }
                break;
                
            case GLFW_KEY_Y:
                //move left/right (along the x axis) by a small amount
                if (mods == GLFW_MOD_SHIFT) {
                    obj.moveY(0.5f);
                } else {
                    obj.moveY(-0.5f); // make sure it doesn't go out of the window
                }
                break;
                
            case GLFW_KEY_Z:
                //move left/right (along the x axis) by a small amount
                if (mods == GLFW_MOD_SHIFT) {
                    obj.moveZ(0.5f);
                } else {
                    obj.moveZ(-0.5f); // make sure it doesn't go out of the window
                }
                break;
                
            case GLFW_KEY_S:
                // scale down/up (about the model's center, not the center of the screen)
                if (mods == GLFW_MOD_SHIFT) {
                    obj.scale(1.5f);
                } else {
                    obj.scale(0.5f); // make sure it doesn't go out of the window
                }
                break;
                
            case GLFW_KEY_O:
                if (mods == GLFW_MOD_SHIFT) {
                    obj.orbit(10.0f);
                } else {
                    obj.orbit(-10.0f);
                }
                break;
                
            case GLFW_KEY_R:
                if (mods == GLFW_MOD_SHIFT) {
                    obj.resetOrientation(); // error
                } else {
                    obj.resetPosition(); // error
                }
                break;
                
            case GLFW_KEY_M:
                if (mode == 0) {
                    mode = 1;
                } else if (mode == 1) {
                    mode = 0;
                }
                break;
                
            default:
                break;
        }
	}
}










