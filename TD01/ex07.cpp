#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glbasimac/glbi_engine.hpp"
#include "glbasimac/glbi_set_of_points.hpp"
#include <iostream>
#include <vector>

using namespace glbasimac;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static float aspectRatio = 1.0f;

/* Espace virtuel */
static const float GL_VIEW_SIZE = 1.;
// L'univers 2D visible a une taille de 1.0 en x et en y

/* Variables for background color */
static float bgRed = 0.2f;
static float bgGreen = 0.0f;
static float bgBlue = 0.0f;

/* OpenGL Engine */
GLBI_Engine myEngine;

/* Points to draw */
GLBI_Set_Of_Points thePoints;
GLBI_Set_Of_Points axesLines; // For drawing coordinate axes

/* Global variables */
bool showAxes = true;

/* Function prototypes */
void initScene();
void renderScene();
void initAxes();

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
    
    // Switch between points and line loop
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        std::cout << "Switching to GL_POINTS mode" << std::endl;
        thePoints.changeNature(GL_POINTS);
    }
    
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        std::cout << "Switching to GL_LINE_LOOP mode" << std::endl;
        thePoints.changeNature(GL_LINE_LOOP);
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
        
        // Create point coordinate and color arrays
        float newCoord[2] = {worldX, worldY};
        float newColor[3] = {1.0f, 1.0f, 1.0f}; // White point by default
        
        // Add the point to our set
        thePoints.addAPoint(newCoord, newColor);
        
        // Print the converted coordinates
        std::cout << "Added point at world coordinates: (" << worldX << ", " << worldY << ")" << std::endl;
    }
}

/**
 * Initialize coordinate axes (X: red, Y: green)
 */
void initAxes() {
    // Create coordinates for X and Y axes
    std::vector<float> axesCoordinates = {
        0.0f, 0.0f,  // Origin
        1.0f, 0.0f,  // X-axis end
        0.0f, 0.0f,  // Origin again
        0.0f, 1.0f   // Y-axis end
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
 * Initialize the scene with points and their colors
 */
void initScene() {
    // Initialize with empty set (we'll add points on clicks)
    std::vector<float> emptyCoords;
    std::vector<float> emptyColors;
    
    // Initialize the set of points
    thePoints.initSet(emptyCoords, emptyColors);
    
    // Initialize the coordinate axes
    initAxes();
}

/**
 * Render the scene
 */
void renderScene() {
    // Set point size and draw points
    glPointSize(6.0);
    thePoints.drawSet();
    
    // Draw coordinate axes if enabled
    if (showAxes) {
        glLineWidth(2.0);
        axesLines.drawSet();
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
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TD 01 Ex 07", nullptr, nullptr);
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
