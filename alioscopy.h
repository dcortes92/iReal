#include <math.h>
#include <vector>
#include <stdio.h>
using namespace std;

#define SUBIMAGE_WIDTH 960//1920  //  360
#define SUBIMAGE_HEIGHT 540//1080   // 360

int numStereoCameras=8;

/*int windowWidth = 960;
int windowHeight = 540;*/

int windowWidth = 1920;
int windowHeight = 1080;

int shiftAncho = 0;
int shiftAlto = 0;

GLuint texName;

float FOV_General = 0.233;
float stereoEyeSeparation_General = 2.56;

GLuint alioscopyShader;

GLsizei	 screenWidth;			// Desired window or desktop width
GLsizei  screenHeight;			// Desired window or desktop height

GLboolean bAnimated;			// Request for continual updates

GLShaderManager		shaderManager;			// Shader Manager
GLMatrixStack		modelViewMatrix;		// Modelview Matrix
GLMatrixStack		projectionMatrix;		// Projection Matrix
GLFrustum			viewFrustum;			// View Frustum
GLGeometryTransform	transformPipeline;		// Geometry Transform Pipeline
GLFrame				cameraFrame;			// Camera frame

GLuint              fboName;
GLuint				textures[1];
GLuint				mirrorTexture;
GLuint              depthBufferName; 
GLFrame				copiaDeCameraFrame;
GLBatch             rectangulo;
