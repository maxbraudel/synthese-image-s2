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
    
    // Toggle between filled and wireframe mode
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        std::cout << "Wireframe mode" << std::endl;
    }
    
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        std::cout << "Filled mode" << std::endl;
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
    // Initialize head (circle with diameter 1.0)
    std::vector<float> circleCoordinates;
    const int numSegments = 32;
    const float radius = 0.5f; // Radius 0.5 for diameter 1.0
    
    // Generate points on the circle
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        circleCoordinates.push_back(x);
        circleCoordinates.push_back(y);
    }
    
    head.initShape(circleCoordinates);
    head.changeNature(GL_TRIANGLE_FAN);
    
    // Initialize ear (triangle for cat ears)
    std::vector<float> triangleCoordinates = {
        -0.5f, -0.5f,  // Bottom-left corner
        0.5f, -0.5f,   // Bottom-right corner
        0.0f, 0.5f     // Top center
    };
    
    ear.initShape(triangleCoordinates);
    ear.changeNature(GL_TRIANGLE_FAN);
    
    // Initialize eye (small circle for cat eyes)
    std::vector<float> eyeCoordinates;
    const int eyeSegments = 16;
    const float eyeRadius = 0.1f;
    
    // Generate points for eye circle
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


/**
 * Draw a square with rounded corners, side length 1, centered at origin
 * The rounded part extends 0.1 units into each corner
 */
void drawRoundedSquare() {
    // Create static objects that persist between function calls
    static GLBI_Convex_2D_Shape cornerCircle;
    static bool cornerCircleInitialized = false;
    
    // Initialize the corner circle if not already done
    if (!cornerCircleInitialized) {
        std::vector<float> circleCoordinates;
        const int numSegments = 16;  // Fewer segments for the small corner
        const float radius = 0.1f;   // Radius of the rounded corner
        
        // Generate points on the circle
        for (int i = 0; i < numSegments; i++) {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            circleCoordinates.push_back(x);
            circleCoordinates.push_back(y);
        }
        
        cornerCircle.initShape(circleCoordinates);
        cornerCircle.changeNature(GL_TRIANGLE_FAN);
        cornerCircleInitialized = true;
    }
    
    // Save current matrix state
    myEngine.mvMatrixStack.pushMatrix();
    
    // Draw the main square (slightly smaller to account for rounded corners)
    myEngine.setFlatColor(0.5f, 0.5f, 0.5f);  // Gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addHomothety(Vector3D(0.8f, 0.8f, 1.0f));  // Scale to 0.8 to leave room for corners
    myEngine.updateMvMatrix();
    ear.drawShape();  // Using the existing square
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the four rounded corners
    myEngine.setFlatColor(0.5f, 0.5f, 0.5f);  // Gray
    
    // Top-right corner
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(0.4f, 0.4f, 0.0f));
    myEngine.updateMvMatrix();
    cornerCircle.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Top-left corner
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(-0.4f, 0.4f, 0.0f));
    myEngine.updateMvMatrix();
    cornerCircle.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Bottom-right corner
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(0.4f, -0.4f, 0.0f));
    myEngine.updateMvMatrix();
    cornerCircle.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Bottom-left corner
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(-0.4f, -0.4f, 0.0f));
    myEngine.updateMvMatrix();
    cornerCircle.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Restore original matrix state
    myEngine.mvMatrixStack.popMatrix();
}

/**
 * Draw the second arm (manipulator)
 * The repère (reference point) is at the center of the left rounded square
 */
void drawSecondArm() {
    // Save current matrix state
    myEngine.mvMatrixStack.pushMatrix();
    
    // Draw the left rounded square
    myEngine.setFlatColor(0.6f, 0.6f, 0.8f);  // Bluish gray
    drawRoundedSquare();
    
    // Draw the connecting rectangle
    myEngine.setFlatColor(0.5f, 0.5f, 0.7f);  // Darker bluish gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(1.0f, 0.0f, 0.0f));  // Move to the right
    myEngine.mvMatrixStack.addHomothety(Vector3D(1.0f, 0.4f, 1.0f));    // Scale to make a rectangle
    myEngine.updateMvMatrix();
    ear.drawShape();  // Using the existing square as a base
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the right rounded square
    myEngine.setFlatColor(0.6f, 0.6f, 0.8f);  // Bluish gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(2.0f, 0.0f, 0.0f));  // Move to the right end
    myEngine.updateMvMatrix();
    drawRoundedSquare();
    myEngine.mvMatrixStack.popMatrix();
    
    // Restore original matrix state
    myEngine.mvMatrixStack.popMatrix();
}

/**
 * Draw the third arm (beater)
 * The repère (reference point) is at the center of the left square
 */
void drawThirdArm() {
    // Save current matrix state
    myEngine.mvMatrixStack.pushMatrix();
    
    // Draw the left square
    myEngine.setFlatColor(0.8f, 0.6f, 0.6f);  // Reddish gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.updateMvMatrix();
    ear.drawShape();  // Using the existing square
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the connecting rectangle
    myEngine.setFlatColor(0.7f, 0.5f, 0.5f);  // Darker reddish gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(0.75f, 0.0f, 0.0f));  // Move to the right
    myEngine.mvMatrixStack.addHomothety(Vector3D(0.5f, 0.3f, 1.0f));     // Scale to make a rectangle
    myEngine.updateMvMatrix();
    ear.drawShape();  // Using the existing square as a base
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the right circle (beater head)
    myEngine.setFlatColor(0.8f, 0.6f, 0.6f);  // Reddish gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(1.5f, 0.0f, 0.0f));  // Move to the right end
    myEngine.mvMatrixStack.addHomothety(Vector3D(0.5f, 0.5f, 1.0f));    // Scale to match diagram
    myEngine.updateMvMatrix();
    head.drawShape();  // Using the existing circle
    myEngine.mvMatrixStack.popMatrix();
    
    // Restore original matrix state
    myEngine.mvMatrixStack.popMatrix();
}

void initScene() {
    // Initialize the coordinate axes
    initAxes();
    
    // Initialize the shapes
    initShapes();
    
    // Add the filaire/plein toggle functionality to the keyboard handler
    // This is already implemented in the clavier function
}


void renderScene() {
    // Reset transformation matrix to identity
    myEngine.mvMatrixStack.loadIdentity();
    
    // Draw coordinate axes if enabled
    if (showAxes) {
        axesLines.drawSet();
    }
    
    // Draw the complete mechanical arm
    // First arm (main arm)
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(0.0f, -1.0f, 0.0f));  // Position at bottom of screen
    myEngine.updateMvMatrix();
    drawFirstArm();
    
    // Second arm (manipulator) - attached to the top of the first arm
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(0.0f, 2.0f, 0.0f));  // Position at top of first arm
    myEngine.mvMatrixStack.addRotation(30.0f, Vector3D(0.0f, 0.0f, 1.0f));  // Rotate 30 degrees
    myEngine.updateMvMatrix();
    drawSecondArm();
    
    // Third arm (beater) - attached to the right end of the second arm
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(2.0f, 0.0f, 0.0f));  // Position at end of second arm
    myEngine.mvMatrixStack.addRotation(45.0f, Vector3D(0.0f, 0.0f, 1.0f));  // Rotate 45 degrees
    myEngine.updateMvMatrix();
    drawThirdArm();
    myEngine.mvMatrixStack.popMatrix();  // Pop third arm
    
    myEngine.mvMatrixStack.popMatrix();  // Pop second arm
    myEngine.mvMatrixStack.popMatrix();  // Pop first arm
    
    // Reset transformation matrix to identity after all drawing
    myEngine.mvMatrixStack.loadIdentity();
    myEngine.updateMvMatrix();
}

/**
 * Draw the main arm with two discs and a trapezoid
 * The repère (reference point) is at the center of the large disc
 */
void drawFirstArm() {
    // Create trapezoid if not already created
    static GLBI_Convex_2D_Shape trapezoid;
    static bool trapezoidInitialized = false;
    
    if (!trapezoidInitialized) {
        // Define trapezoid vertices
        std::vector<float> trapezoidCoordinates = {
            -0.5f, -1.0f,  // Bottom-left
            0.5f, -1.0f,   // Bottom-right
            0.25f, 1.0f,   // Top-right
            -0.25f, 1.0f   // Top-left
        };
        
        trapezoid.initShape(trapezoidCoordinates);
        trapezoid.changeNature(GL_TRIANGLE_FAN);
        trapezoidInitialized = true;
    }
    
    // Save the current matrix state
    myEngine.mvMatrixStack.pushMatrix();
    
    // Draw the large disc at the base (radius 1)
    myEngine.setFlatColor(0.7f, 0.7f, 0.7f);  // Light gray
    head.drawShape();  // Using the existing circle with radius 0.5
    
    // Draw the trapezoid body
    myEngine.setFlatColor(0.6f, 0.6f, 0.6f);  // Medium gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addHomothety(Vector3D(1.0f, 2.0f, 1.0f));  // Scale to match the diagram
    myEngine.updateMvMatrix();
    trapezoid.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the small disc at the top
    myEngine.setFlatColor(0.7f, 0.7f, 0.7f);  // Light gray
    myEngine.mvMatrixStack.pushMatrix();
    myEngine.mvMatrixStack.addTranslation(Vector3D(0.0f, 2.0f, 0.0f));  // Move to top of trapezoid
    myEngine.mvMatrixStack.addHomothety(Vector3D(0.5f, 0.5f, 1.0f));  // Half the size of the base disc
    myEngine.updateMvMatrix();
    head.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Restore the original matrix state
    myEngine.mvMatrixStack.popMatrix();
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
