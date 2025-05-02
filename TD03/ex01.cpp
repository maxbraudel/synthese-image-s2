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
static float bgGreen = 0.2f;
static float bgBlue = 0.2f;

/* OpenGL Engine */
GLBI_Engine myEngine;

/* Points to draw */
GLBI_Set_Of_Points axesLines; // For drawing coordinate axes

/* Shapes to draw */
GLBI_Convex_2D_Shape carre;         // Carré de base
GLBI_Convex_2D_Shape cercle;        // Cercle de base
GLBI_Convex_2D_Shape trapeze;       // Trapèze pour le bras

/* Global variables */
bool showAxes = true;

/* Function prototypes */
void initScene();
void renderScene();
void initAxes();
void initShapes();
void drawFirstArm();

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
    
    // Activer le mode filaire ou plein
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
 * Initialize shapes for mechanical arm components
 */
void initShapes() {
    // Initialize circle (rayon = 1)
    std::vector<float> circleCoordinates;
    const int numSegments = 32;
    const float radius = 1.0f; 
    
    for (int i = 0; i < numSegments; i++) {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        circleCoordinates.push_back(x);
        circleCoordinates.push_back(y);
    }
    
    cercle.initShape(circleCoordinates);
    cercle.changeNature(GL_TRIANGLE_FAN);
    
    // Initialize square (côté = 1)
    std::vector<float> squareCoordinates = {
        -0.5f, -0.5f, 
        0.5f, -0.5f, 
        0.5f, 0.5f,
        -0.5f, 0.5f
    };
    
    carre.initShape(squareCoordinates);
    carre.changeNature(GL_TRIANGLE_FAN); 
    
    // Initialize trapezoid for arm
    std::vector<float> trapezeCoordinates = {
        -1.0f, -2.0f,  // Bas gauche
        1.0f, -2.0f,   // Bas droite
        0.5f, 2.0f,   // Haut droite
        -0.5f, 2.0f   // Haut gauche
    };
    
    trapeze.initShape(trapezeCoordinates);
    trapeze.changeNature(GL_TRIANGLE_FAN);
}

void initScene() {
    // Initialize the coordinate axes
    initAxes();
    
    // Initialize the shapes
    initShapes();
}

/**
 * Draws the first arm of the mechanical robot
 * - Large circle at the base (radius 1.5)
 * - Trapezoid body
 * - Small circle at the top (radius 0.75)
 */
void drawFirstArm() {
    // Reset transformation matrix to identity
    myEngine.mvMatrixStack.loadIdentity();
    
    // Position everything relative to the large circle
    
    // Draw the large circle (base)
    myEngine.setFlatColor(0.6f, 0.6f, 0.6f); // Gris
    
    // Use the matrix stack to scale the base circle to radius 1.5
    myEngine.mvMatrixStack.pushMatrix();
    Vector3D circleScale{1.5f, 1.5f, 1.0f};
    myEngine.mvMatrixStack.addHomothety(circleScale);
    myEngine.updateMvMatrix();
    cercle.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the trapezoid body
    myEngine.setFlatColor(0.5f, 0.5f, 0.5f); // Gris plus foncé
    
    // Trapezoid is positioned above the center of the large circle
    myEngine.mvMatrixStack.pushMatrix();
    Vector3D trapezePos{0.0f, 1.5f, 0.0f}; // 1.5 units up from the large circle center
    myEngine.mvMatrixStack.addTranslation(trapezePos);
    myEngine.updateMvMatrix();
    trapeze.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Draw the small circle (top)
    myEngine.setFlatColor(0.6f, 0.6f, 0.6f); // Même couleur que le cercle de base
    
    // Place the small circle at the top of the trapezoid
    myEngine.mvMatrixStack.pushMatrix();
    Vector3D smallCirclePos{0.0f, 3.5f, 0.0f}; // 3.5 units up from large circle center
    myEngine.mvMatrixStack.addTranslation(smallCirclePos);
    
    // Scale the circle to be smaller (radius 0.75)
    Vector3D smallCircleScale{0.75f, 0.75f, 1.0f};
    myEngine.mvMatrixStack.addHomothety(smallCircleScale);
    
    myEngine.updateMvMatrix();
    cercle.drawShape();
    myEngine.mvMatrixStack.popMatrix();
    
    // Reset matrix after drawing
    myEngine.mvMatrixStack.loadIdentity();
    myEngine.updateMvMatrix();
}

void renderScene() {
    if (showAxes) {
        glLineWidth(2.0);
        axesLines.drawSet();
    }
    
    // Draw the first arm
    drawFirstArm();
    
    // Reset transformation matrix to identity after all drawing
    myEngine.mvMatrixStack.loadIdentity();
    myEngine.updateMvMatrix();
}

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure error callback
    glfwSetErrorCallback(onError);
    
    // Configure OpenGL version (3.3 core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create a window
    int windowWidth = 500;
    int windowHeight = 500;
    aspectRatio = (float)windowWidth / (float)windowHeight;
    
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "TD03 Ex01 - Bras mécanique", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Set up viewport
    glViewport(0, 0, windowWidth, windowHeight);
    
    // Set up callbacks
    glfwSetKeyCallback(window, clavier);
    glfwSetWindowSizeCallback(window, onWindowResized);
    
    // Initialize the engine
    myEngine.init(windowWidth, windowHeight);
    
    // Set the projection matrix for the virtual world
    if (aspectRatio > 1.0) {
        myEngine.set2DProjection(-GL_VIEW_SIZE * aspectRatio / 2.0f, GL_VIEW_SIZE * aspectRatio / 2.0f, -GL_VIEW_SIZE / 2.0f, GL_VIEW_SIZE / 2.0f);
    } else {
        myEngine.set2DProjection(-GL_VIEW_SIZE / 2.0f, GL_VIEW_SIZE / 2.0f, -GL_VIEW_SIZE / (2.0f * aspectRatio), GL_VIEW_SIZE / (2.0f * aspectRatio));
    }
    
    // Initialize the scene
    initScene();
    
    double startTime = 0.0;
    double endTime = 0.0;
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Time management
        startTime = glfwGetTime();
        
        // Clear the screen
        glClearColor(bgRed, bgGreen, bgBlue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render the scene
        renderScene();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Framerate control
        endTime = glfwGetTime();
        double elapsed = endTime - startTime;
        if (elapsed < FRAMERATE_IN_SECONDS) {
            glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS - elapsed);
        }
    }
    
    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
