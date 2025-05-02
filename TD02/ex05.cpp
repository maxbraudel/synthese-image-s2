#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include "glbasimac/glbi_convex_2D_shape.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;

/* Espace virtuel */
static const float GL_VIEW_SIZE = 6.0;
// L'univers 2D visible a une taille de 6.0 en x et en y

/* Variables for background color */
static float bgRed = 0.2f;
static float bgGreen = 0.0f;
static float bgBlue = 0.0f;

/* OpenGL Engine */
GLBI_Engine myEngine;

/* Points to draw */
GLBI_Set_Of_Points axesLines; // For drawing coordinate axes

/* Shapes to draw */
GLBI_Convex_2D_Shape head;       // Circle for cat's head
GLBI_Convex_2D_Shape ear;        // Triangle for cat's ear
GLBI_Convex_2D_Shape eye;        // Circle for cat's eye

/* Global variables */
bool showAxes = true;
bool showEyes = true;

/* Function prototypes */
void initScene();
void renderScene();
void initAxes();
void initShapes();

/* Error handling function */
void onError(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << std::endl;
}

/* Window resize handler */
void onWindowResized(GLFWwindow* window, int width, int height) {
    // Update viewport dimensions
    glViewport(0, 0, width, height);
    
    // Update aspect ratio
    aspectRatio = (float)width / (float)height;
    
    // Adjust projection based on new aspect ratio
    if (aspectRatio > 1.0) {
        // Landscape mode
        myEngine.set2DProjection(-GL_VIEW_SIZE * aspectRatio / 2.0f, GL_VIEW_SIZE * aspectRatio / 2.0f, -GL_VIEW_SIZE / 2.0f, GL_VIEW_SIZE / 2.0f);
    } else {
        // Portrait mode
        myEngine.set2DProjection(-GL_VIEW_SIZE / 2.0f, GL_VIEW_SIZE / 2.0f, -GL_VIEW_SIZE / (2.0f * aspectRatio), GL_VIEW_SIZE / (2.0f * aspectRatio));
    }
}

/* Keyboard input handler */
void clavier(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Vérifier que Q est pressé
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        // Obtenir le caractère représenté par cette touche
        const char* keyName = glfwGetKeyName(key, scancode);
        if (keyName) {
            std::cout << "Touche: " << keyName << std::endl;
        }
        
        // Fermeture de la fenêtre
        std::cout << "Fermeture de la fenêtre" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Toggle axes display
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        showAxes = !showAxes;
        std::cout << (showAxes ? "Showing" : "Hiding") << " coordinate axes" << std::endl;
    }
    
    // Toggle eyes display
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        showEyes = !showEyes;
        std::cout << (showEyes ? "Showing" : "Hiding") << " cat eyes" << std::endl;
    }
}

/**
 * Initialize coordinate axes (X: red, Y: green)
 */
void initAxes() {
    // Create coordinates for X and Y axes (scaled for GL_VIEW_SIZE = 6.0)
    std::vector<float> axesCoordinates = {
        0.0f, 0.0f,  // Origin
        3.0f, 0.0f,  // X-axis end
        0.0f, 0.0f,  // Origin again
        0.0f, 3.0f   // Y-axis end
    };
    
    // Red color for X-axis, Green color for Y-axis
    std::vector<float> axesColors = {
        1.0f, 0.0f, 0.0f,  // Red for X-axis start
        1.0f, 0.0f, 0.0f,  // Red for X-axis end
        0.0f, 1.0f, 0.0f,  // Green for Y-axis start
        0.0f, 1.0f, 0.0f   // Green for Y-axis end
    };
    
    // Initialize the set of lines
    axesLines.initSet(axesCoordinates, axesColors);
    axesLines.changeNature(GL_LINES); // Draw as lines
}

/**
 * Initialize shapes for cat face components
 */
void initShapes() {
    // Initialize head
    std::vector<float> circleCoordinates;
    const int numSegments = 32;
    const float radius = 0.5f; 
    
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        circleCoordinates.push_back(x);
        circleCoordinates.push_back(y);
    }
    
    head.initShape(circleCoordinates);
    head.changeNature(GL_TRIANGLE_FAN);
    
    // Initialize ear 
    std::vector<float> triangleCoordinates = {
        -0.5f, -0.5f, 
        0.5f, -0.5f, 
        0.0f, 0.5f  
    };
    
    ear.initShape(triangleCoordinates);
    ear.changeNature(GL_TRIANGLE_FAN); 
    
    // Initialize eye
    std::vector<float> eyeCoordinates;
    const int eyeSegments = 16;
    const float eyeRadius = 0.1f;
    
    // eye circle
    for (int i = 0; i < eyeSegments; i++) {
        float angle = 2.0f * M_PI * i / eyeSegments;
        float x = eyeRadius * cos(angle);
        float y = eyeRadius * sin(angle);
        eyeCoordinates.push_back(x);
        eyeCoordinates.push_back(y);
    }
    
    eye.initShape(eyeCoordinates);
    eye.changeNature(GL_TRIANGLE_FAN);
}


void initScene() {
    // Initialize the coordinate axes
    initAxes();
    
    // Initialize the shapes
    initShapes();
}

void renderScene() {
    if (showAxes) {
        glLineWidth(2.0);
        axesLines.drawSet();
    }
    
    // Set color and draw cat head (circle at the center)
    myEngine.setFlatColor(0.8f, 0.8f, 0.8f);
    head.drawShape();
    
    // Draw right ear with transformations
    myEngine.setFlatColor(0.7f, 0.7f, 0.7f);
    
    // Reset transformation matrix to identity
    myEngine.mvMatrixStack.loadIdentity();
    
    // Apply translation for right ear
    float sqrt2_2 = 0.7071f; 
    Vector3D rightEarPos{sqrt2_2 * 0.5f, sqrt2_2 * 0.5f, 0.0f};
    myEngine.mvMatrixStack.addTranslation(rightEarPos);
    
    // Apply rotation 
    Vector3D rotationAxis{0.0f, 0.0f, 1.0f};
    myEngine.mvMatrixStack.addRotation(M_PI/3, rotationAxis);
    
    // Apply scaling (0.5 on both x and y)
    Vector3D earScale{0.5f, 0.5f, 1.0f}; 
    myEngine.mvMatrixStack.addHomothety(earScale);
    
    // Send transformations to pipeline
    myEngine.updateMvMatrix();
    
    ear.drawShape();
    
    // Reset transformation matrix to identity
    myEngine.mvMatrixStack.loadIdentity();
    
    // Apply translation for left ear
    Vector3D leftEarPos{-sqrt2_2 * 0.5f, sqrt2_2 * 0.5f, 0.0f};
    myEngine.mvMatrixStack.addTranslation(leftEarPos);
    
    // Apply rotation (-60 degrees = -PI/3)
    myEngine.mvMatrixStack.addRotation(-M_PI/3, rotationAxis);
    
    // Apply scaling (0.5 on both x and y)
    myEngine.mvMatrixStack.addHomothety(earScale);
    
    // Send transformations to pipeline
    myEngine.updateMvMatrix();
    
    // Draw the left ear
    ear.drawShape();
    
    // Draw eyes if enabled
    if (showEyes) {
        // Set color for eyes
        myEngine.setFlatColor(0.0f, 0.0f, 0.0f);
        
        // Draw right eye
        myEngine.mvMatrixStack.loadIdentity();
        Vector3D rightEyePos{0.2f, 0.1f, 0.0f};
        myEngine.mvMatrixStack.addTranslation(rightEyePos);
        myEngine.updateMvMatrix();
        eye.drawShape();
        
        // Draw left eye
        myEngine.mvMatrixStack.loadIdentity();
        Vector3D leftEyePos{-0.2f, 0.1f, 0.0f};
        myEngine.mvMatrixStack.addTranslation(leftEyePos);
        myEngine.updateMvMatrix();
        eye.drawShape();
    }
    
    // Reset transformation matrix to identity after all drawing
    myEngine.mvMatrixStack.loadIdentity();
    myEngine.updateMvMatrix();
}

int main() {
    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    /* Callback to a function if an error is rised by GLFW */
	glfwSetErrorCallback(onError);

    // Create a windowed mode window and its OpenGL context
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 800;
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TD 01 Ex 09", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Set the window resize callback
    glfwSetWindowSizeCallback(window, onWindowResized);
    
    // Set the keyboard input callback
    glfwSetKeyCallback(window, clavier);

	// Intialize glad (loads the OpenGL functions)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}

	// Initialize Rendering Engine
	myEngine.initGL();
	
	// Initial window resize to set up projection
	onWindowResized(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	// Initialize the scene
	initScene();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Render here */
		glClearColor(bgRed, bgGreen, bgBlue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        // Render the scene
		renderScene();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		/* Elapsed time computation from loop begining */
		double elapsedTime = glfwGetTime() - startTime;
		/* If to few time is spend vs our wanted FPS, we wait */
		while(elapsedTime < FRAMERATE_IN_SECONDS)
		{
			glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS-elapsedTime);
			elapsedTime = glfwGetTime() - startTime;
		}
	}

    glfwTerminate();
    return 0;
}
