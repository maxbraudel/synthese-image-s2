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
}

/* Gestion des clics de souris */
static void sourisClics(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Récupération de la position du curseur
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Calcul des composantes de couleur (x mod 256, y mod 256, 0)
        int redComponent = (int)xpos % 256;
        int greenComponent = (int)ypos % 256;
        
        // Conversion des valeurs entre 0 et 255 vers des flottants entre 0 et 1
        bgRed = (float)redComponent / 255.0f;
        bgGreen = (float)greenComponent / 255.0f;
        bgBlue = 0.0f;
        
        std::cout << "Clique - x: " << xpos << " y: " << ypos ;
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
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TD 01 Ex 04", nullptr, nullptr);
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
	
	// Exercise 5: Initialize points
	std::vector<float> pointCoordinates = {
		0.0f, 0.0f,  // Origin point
		0.5f, 0.0f,  // Point at (0.5, 0.0)
		0.0f, 0.5f,  // Point at (0.0, 0.5)
		-0.5f, -0.5f // Point at (-0.5, -0.5)
	};
	
	// Define colors for each point (RGB values)
	std::vector<float> pointColors = {
		1.0f, 1.0f, 1.0f,  // White for origin
		1.0f, 0.0f, 0.0f,  // Red for second point
		0.0f, 1.0f, 0.0f,  // Green for third point
		1.0f, 0.0f, 1.0f   // Violet for fourth point
	};
	
	// Initialize the set of points with coordinates and colors
	thePoints.initSet(pointCoordinates, pointColors);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Render here */
		glClearColor(bgRed, bgGreen, bgBlue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        // Exercise 5: Render points
		glPointSize(4.0);
		thePoints.drawSet();

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
