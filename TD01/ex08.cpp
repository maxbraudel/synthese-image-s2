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
static const float GL_VIEW_SIZE = 4.0;
// L'univers 2D visible a une taille de 1.0 en x et en y

/* Variables for background color */
static float bgRed = 0.2f;
static float bgGreen = 0.0f;
static float bgBlue = 0.0f;

/* OpenGL Engine */
GLBI_Engine myEngine;

/* Points to draw */
GLBI_Set_Of_Points axesLines; // For drawing coordinate axes

/* Shapes to draw */
GLBI_Convex_2D_Shape square;
GLBI_Convex_2D_Shape triangle;
GLBI_Convex_2D_Shape circle;

/* Global variables */
bool showAxes = true;
enum ShapeType { SQUARE, TRIANGLE, CIRCLE };
ShapeType currentShape = SQUARE;
bool filledMode = false;

/* Function prototypes */
void initScene();
void renderScene();
void initAxes();
void drawSquare();
void drawTriangle();
void drawCircle();
void initSquare();
void initTriangle();
void initCircle();

/* Error handling function */
void onError(int error, const char* description) {
	std::cout << "GLFW Error ("<<error<<") : " << description << std::endl;
}

void onWindowResized(GLFWwindow* /*window*/, int width, int height)
{
    aspectRatio = width / (float) height;
    glViewport(0, 0, width, height);
    if(aspectRatio > 1.0)
    {
        myEngine.set2DProjection(-GL_VIEW_SIZE * aspectRatio/ 2.,
        GL_VIEW_SIZE * aspectRatio / 2. ,
        -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.);
    }
    else
    {
        myEngine.set2DProjection(-GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.,
        -GL_VIEW_SIZE / (2. * aspectRatio),
        GL_VIEW_SIZE / (2. * aspectRatio));
    }
}

/* Gestion actions clavier */
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
    
    // Toggle filled mode
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        filledMode = !filledMode;
        std::cout << (filledMode ? "Filled" : "Outline") << " mode" << std::endl;
        
        // Update shape rendering mode
        if (filledMode) {
            square.changeNature(GL_TRIANGLE_FAN);
            triangle.changeNature(GL_TRIANGLE_FAN);
            circle.changeNature(GL_TRIANGLE_FAN);
        } else {
            square.changeNature(GL_LINE_LOOP);
            triangle.changeNature(GL_LINE_LOOP);
            circle.changeNature(GL_LINE_LOOP);
        }
    }
    
    // Cycle through shapes with space bar
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        // Cycle through shapes: SQUARE -> TRIANGLE -> CIRCLE -> SQUARE
        switch (currentShape) {
            case SQUARE:
                currentShape = TRIANGLE;
                std::cout << "Showing triangle" << std::endl;
                break;
            case TRIANGLE:
                currentShape = CIRCLE;
                std::cout << "Showing circle" << std::endl;
                break;
            case CIRCLE:
                currentShape = SQUARE;
                std::cout << "Showing square" << std::endl;
                break;
        }
    }

    // Toggle axes display
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        showAxes = !showAxes;
        std::cout << (showAxes ? "Showing" : "Hiding") << " coordinate axes" << std::endl;
    }
}

/* Gestion des clics de souris */
static void sourisClics(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Récupération de la position du curseur
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Get window dimensions
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        
        // Convert screen coordinates to virtual coordinates
        float worldX, worldY;
        if (aspectRatio > 1.0) {
            // Landscape mode
            worldX = ((float)xpos / windowWidth) * (GL_VIEW_SIZE * aspectRatio) - (GL_VIEW_SIZE * aspectRatio / 2.0f);
            worldY = (1.0f - (float)ypos / windowHeight) * GL_VIEW_SIZE - (GL_VIEW_SIZE / 2.0f);
        } else {
            // Portrait mode
            worldX = ((float)xpos / windowWidth) * GL_VIEW_SIZE - (GL_VIEW_SIZE / 2.0f);
            worldY = (1.0f - (float)ypos / windowHeight) * (GL_VIEW_SIZE / aspectRatio) - (GL_VIEW_SIZE / (2.0f * aspectRatio));
        }
        
        // Print the converted coordinates
        std::cout << "Clicked at world coordinates: (" << worldX << ", " << worldY << ")" << std::endl;
    }
}

/**
 * Initialize coordinate axes (X: red, Y: green)
 */
void initAxes() {
    // Create coordinates for X and Y axes (scaled by 2 for the new GL_VIEW_SIZE)
    std::vector<float> axesCoordinates = {
        0.0f, 0.0f,  // Origin
        2.0f, 0.0f,  // X-axis end
        0.0f, 0.0f,  // Origin again
        0.0f, 2.0f   // Y-axis end
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
 * Initialize a square with side length 1, centered at origin
 */
void initSquare() {
    std::vector<float> squareCoordinates = {
        -0.5f, -0.5f,  // Bottom-left corner
        0.5f, -0.5f,   // Bottom-right corner
        0.5f, 0.5f,    // Top-right corner
        -0.5f, 0.5f    // Top-left corner
    };
    
    square.initShape(squareCoordinates);
}

/**
 * Initialize a triangle with specified coordinates
 */
void initTriangle() {
    std::vector<float> triangleCoordinates = {
        -0.5f, -0.5f,  // Bottom-left corner
        0.5f, -0.5f,   // Bottom-right corner
        0.0f, 0.5f     // Top center
    };
    
    triangle.initShape(triangleCoordinates);
}

/**
 * Initialize a circle approximated with 32 points
 */
void initCircle() {
    std::vector<float> circleCoordinates;
    const int numSegments = 32;
    const float radius = 0.5f;
    
    // Generate points on the circle
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        circleCoordinates.push_back(x);
        circleCoordinates.push_back(y);
    }
    
    circle.initShape(circleCoordinates);
}

/**
 * Draw a red square
 */
void drawSquare() {
    myEngine.setFlatColor(1.0f, 0.0f, 0.0f);  // Red
    square.drawShape();
}

/**
 * Draw a yellow triangle
 */
void drawTriangle() {
    myEngine.setFlatColor(1.0f, 1.0f, 0.0f);  // Yellow
    triangle.drawShape();
}

/**
 * Draw a green circle
 */
void drawCircle() {
    myEngine.setFlatColor(0.0f, 1.0f, 0.0f);  // Green
    circle.drawShape();
}

/**
 * Initialize the scene with shapes
 */
void initScene() {
    // Initialize the coordinate axes
    initAxes();
    
    // Initialize the shapes
    initSquare();
    initTriangle();
    initCircle();
}

/**
 * Render the scene
 */
void renderScene() {
    // Draw coordinate axes if enabled
    if (showAxes) {
        glLineWidth(2.0);
        axesLines.drawSet();
    }
    
    // Draw the current shape
    glLineWidth(2.0);
    switch (currentShape) {
        case SQUARE:
            drawSquare();
            break;
        case TRIANGLE:
            drawTriangle();
            break;
        case CIRCLE:
            drawCircle();
            break;
    }
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
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TD 01 Ex 08", nullptr, nullptr);
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
    
    // Callback pour les clics de souris
    glfwSetMouseButtonCallback(window, sourisClics);

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
