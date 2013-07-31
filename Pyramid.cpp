// Pyramid.cpp 
// Ejemplo para correr en los monitores Alioscopy
#include "clienteInternoAlambrado.cpp"

#include <GLTools.h>	// OpenGL toolkit
#include <GLMatrixStack.h>   // Pila de matrices
#include <GLFrame.h>        
#include <GLFrustum.h>
#include <GLBatch.h>     // usado para construir la pirámide
#include <GLGeometryTransform.h>


#include <vector>    // para el vector de cámaras

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

using namespace std;


// de Alioscopy
GLuint alioscopyShader;   // para el Shader

GLint locMVP;    // posición del ModelViewProjection matrix uniform
GLint locColor;  // posición del color value uniform


#define IMAGE_WIDTH 1920*2    // ancho de la imagen
#define IMAGE_HEIGHT 1080*2    // altura de la imagen
#define SUBIMAGE_WIDTH 1920///2    // ancho de la imagen
#define SUBIMAGE_HEIGHT 1080///2    // altura de la imagen

int shiftAncho = 0;
int shiftAlto = 0;

int numStereoCameras = 8; 	//  número de camara para Alioscopy
int windowWidth = 1920;	  	// está para el monitor pequeño
int windowHeight = 1080;   	//  está para el monitor pequeño

float stereoEyeSeparation_General = 2.56;    // la separación entre los ojos. Se recomienda 2.56

GLuint texName;    // Donde va la textura de Alioscopy

float FOV_General = 0.233;   // Inicialmente Alioscopy recomienda 0.233
float imageZ_General = 157.0f;

GLsizei	 screenWidth;			// Desired window or desktop width
GLsizei  screenHeight;			// Desired window or desktop height

// Esto me parece que no es necesario
// float _angle = 30.0f;     // ver su uso  ????
// float _camera = 0.0f;     // ver su uso     ????


// De la Piramide
GLShaderManager		shaderManager;    // Cuando no se use el Shader de Alioscopy
GLMatrixStack		modelViewMatrix;   // matriz de modelación y transformación
GLMatrixStack		projectionMatrix;    // matriz de proyección
GLFrame			cameraFrame;      // para ser la cámara central
GLFrame			copiaDecameraFrame;
GLFrame             	objectFrame;
GLFrustum		viewFrustum;
GLBatch             	pyramidBatch;
GLBatch             	pyramidBatch2;

GLBatch             	rectangulo;  // para renderizar usando Alioscopy Shader

GLuint              	textureID;
GLGeometryTransform	transformPipeline;
M3DMatrix44f		shadowMatrix;


// De Alioscpy
struct STEREOFRUSTUM{
		float top;
		float bottom;
		float left;
		float right;
};

class Camera {
	struct STEREOFRUSTUM stereoFrustum;
	float stereoOffset;
	float nearZ, imageZ, farZ;
	float FOV;
	float aspect;
	float stereoEyeSeparation;
	
	public: 
		void SetStereoscopicFrustum(int i){
			i = i - numStereoCameras/2;
			stereoOffset = i*stereoEyeSeparation + stereoEyeSeparation/2;
			float aspect = (float)IMAGE_WIDTH/(float)IMAGE_HEIGHT;
			float top = nearZ*tan(FOV/(2*aspect));
			stereoFrustum.top = top;
			stereoFrustum.bottom = -top;
			float right = aspect*top;
			float shift = -stereoOffset*nearZ/imageZ;
			stereoFrustum.left = -right + shift;
			stereoFrustum.right = right + shift;
			glFrustum(stereoFrustum.left, stereoFrustum.right, stereoFrustum.bottom, stereoFrustum.top,
			nearZ, farZ);
		}  //  void SetStereoscopicFrustum(int i){

		void SetFrustum(){
			glViewport(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
			//glViewport(0,0,SUBIMAGE_WIDTH,SUBIMAGE_HEIGHT);           //PUEDE QUE VAYA ACA LO D MAS GRANDE
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(stereoFrustum.left,stereoFrustum.right, stereoFrustum.bottom, stereoFrustum.top,
			nearZ, farZ);
		}  //  void SetFrustum(){

		void Transform(int i){
			float desplazamiento = 0.0f;
			float entreCamaras = 0.0005f;
		
			switch  (i)
			{
				case 7 : desplazamiento = -3.5f  ; 
				break ;
				case 6 : desplazamiento = -2.5f  ; 
				break ;				
				case 5 : desplazamiento = -1.5f  ; 
				break ;				
				case 4 : desplazamiento = -0.5f  ; 
				break ;				
				case 3 : desplazamiento = 0.5f  ; 
				break ;				
				case 2 : desplazamiento = 1.5f  ; 
				break;				
				case 1 : desplazamiento = 2.5f  ; 
				break ;				
				case 0 : desplazamiento = 3.5f  ; 
				break ;				
			}	// switch

			cameraFrame.TranslateLocal(stereoEyeSeparation_General*desplazamiento/157.0,0,0); 

			//cameraFrame.TranslateLocal(desplazamiento*2.56/157*15.2,0,0); 
			//cameraFrame.SetForwardVector(-cameraFrame.GetOriginX(),-cameraFrame.GetOriginY(),-cameraFrame.GetOriginZ());
			//cameraFrame.RotateLocalY(-asin(2.56*desplazamiento/157.0));
			cameraFrame.Normalize();
			
		}  //  void Transform(){

		
		Camera(){
			nearZ = 1.0f; //Near clip
			imageZ = 157; //Image plane  La documentación pide 157 pulgadas
			farZ = 5000.0f; //Far clip
			FOV = FOV_General  ; //Radians (13.35 degrees)
			stereoEyeSeparation = 2.56; //Inches (6.5 cm)
			aspect = (float)IMAGE_WIDTH/(float)IMAGE_HEIGHT; // está para el monior pequeño 1920/1200 for Alioscopy 3D HD 24"
		}  //  Camera(){
};  //  class Camera {


// De la Pirámide
void MakePyramid(GLBatch& pyramidBatch)
{
	pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
	//  void GLBatch::Begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits = 0);
    
	
	/*
	The Normal3f method adds a surface normal to the batch. The MultiTexCoord2f adds a
	texture coordinate, and finally, Vertex3f adds the vertex position. An important rule for
	the GLBatch that does not apply to the old-style immediate mode is that if you specify
	normals or texture coordinates for any vertex, you must do so for every vertex. This
	removes some of the flexibility of the old style, but does make it run a bit faster. The
	Normal3f and Vertex3f functions are pretty self-explanatory, but MultiTexCord2f has
	three parameters, and the first is an integer:

	void GLBatch::MultiTexCoord2f(GLuint texture, GLclampf s, GLclamp t);

	*/
	
	// Bottom of pyramid
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3f(1.0f, -1.0f, -1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	pyramidBatch.Vertex3f(-1.0f, -1.0f, 1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
	pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
	
	M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
	M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
	M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
	M3DVector3f vBackLeft = { -1.0f, -1.0f, -1.0f };
	M3DVector3f vBackRight = { 1.0f, -1.0f, -1.0f };
	M3DVector3f n;
	
	// Front of Pyramid
	m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);		// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontLeft);		// Front left corner
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontRight);		// Front right corner
    
    
	m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);		// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackLeft);		// Back left corner
	
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontLeft);		// Front left corner
    
	m3dFindNormal(n, vApex, vFrontRight, vBackRight);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);				// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vFrontRight);		// Front right corner
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackRight);			// Back right cornder
    
    
	m3dFindNormal(n, vApex, vBackRight, vBackLeft);
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
	pyramidBatch.Vertex3fv(vApex);		// Apex
    
	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackRight);		// Back right cornder

	pyramidBatch.Normal3fv(n);
	pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	pyramidBatch.Vertex3fv(vBackLeft);		// Back left corner

	pyramidBatch.End();

}  //  void MakePyramid(GLBatch& pyramidBatch)





// De la Pirámide
// Load a TGA as a 2D Texture. Completely initialize the state
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
	return true;

}  //  bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)


//  De Alioscpy 
vector<Camera *> stereoCameras;    // vector para mantener las 8 cámaras

///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context. 
void SetupRC()
{
	// esto es de Alioscopy
	for (int i=0;i<numStereoCameras;i++){
		Camera *camera = new Camera();
		stereoCameras.push_back(camera);
		camera->SetStereoscopicFrustum(i);
	}  // for


	// FONDO DE LA VENTANA
	//  Explica el uso de la función void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

	// Si la quiere en gris
    	glClearColor(0.7f, 0.7f, 0.7f, 1.0f );

	// Si la quiere en negro
    	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	// Si la quiere en azul
    	//glClearColor(0.0f, 0.0f, 1.0f, 1.0f );
	
	//  De Alioscopy
	glGenTextures(1,&texName);
	glBindTexture(GL_TEXTURE_2D,texName);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SUBIMAGE_WIDTH, numStereoCameras*SUBIMAGE_HEIGHT, 0, 
	GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	// de Alioscopy
	
	// Cargar archivos frag y vert 
	alioscopyShader = gltLoadShaderPairWithAttributes("Alio_Mix_OpenGL.vert", "Alio_Mix_OpenGL.frag", 0);
	
	// De la Pirámide	
	shaderManager.InitializeStockShaders();    // este es sin Alioscopy
	
	// De la piramide
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
    	glGenTextures(1, &textureID);     
	glBindTexture(GL_TEXTURE_2D, textureID);
	LoadTGATexture("stone.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    	MakePyramid(pyramidBatch);
//	MakePyramid2(pyramidBatch2);



	// Esto que sigue son pruebas para mover la cámara, se puede luego eliminar
	cameraFrame.MoveForward(10.0f);   
	//cameraFrame.Normalize();
	cameraFrame.TranslateLocal(-2.5f, 2.0f, -19.0f);
	

}   //  SetupRC()

///////////////////////////////////////////////////////////////////////////////
// Cleanup... such as deleting texture objects
void ShutdownRC(void)
{   
    	glDeleteTextures(1, &textureID);    // de la pirámide
	glDeleteTextures(1, &texName);     // de alioscopy
}  //   void ShutdownRC(void)

///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{  
	// de la Pirámide
    	static GLfloat vLightPos [] = { 1.0f, 1.0f, 0.0f };
    	static GLfloat vWhite [] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
	// Clear the window with current clearing color
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	/* de Alioscopy */
	
	
	for (int i=0;i<numStereoCameras;i++){
		
			//modelViewMatrix.PushMatrix();  // guarda estado
		
			Camera *camera = stereoCameras[i];
			camera->SetFrustum();
			
			glMatrixMode(GL_MODELVIEW);		
			glPushMatrix();

			copiaDecameraFrame = cameraFrame;
			camera->Transform(i); // hace los cálculos de la cámara

			modelViewMatrix.LoadIdentity();
			M3DMatrix44f mCamera;
			cameraFrame.GetCameraMatrix(mCamera);
				
			modelViewMatrix.MultMatrix(mCamera);
			/*
			M3DMatrix44f mObjectFrame;
			objectFrame.GetMatrix(mObjectFrame);
			modelViewMatrix.MultMatrix(mObjectFrame);
			*/			
				
			// drawScene(); //Pinta el cuadro    
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, textureID);

			shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, 
										 transformPipeline.GetModelViewMatrix(),
										 transformPipeline.GetProjectionMatrix(), 
										 vLightPos, vWhite, 0);
			pyramidBatch.Draw();
							
			glMatrixMode(GL_MODELVIEW);		
			glPopMatrix();
			
			// se pone acá solo por prueba, hay que quitarlo luego este swapbuffers
			//glutSwapBuffers(); 

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,texName);			glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,(i)*SUBIMAGE_HEIGHT,
			//(numStereoCameras - i - 1)*SUBIMAGE_HEIGHT,
			(shiftAncho)*SUBIMAGE_WIDTH + SUBIMAGE_WIDTH * 0.5,
			(shiftAlto) * SUBIMAGE_HEIGHT + SUBIMAGE_HEIGHT * 0.5, SUBIMAGE_WIDTH ,SUBIMAGE_HEIGHT );
			glViewport(0,0,SUBIMAGE_WIDTH,SUBIMAGE_HEIGHT);
			cameraFrame = copiaDecameraFrame;

	}  // for
	
	
	//LA PARTE DEL SHADERMANAGER

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_General,(float)SUBIMAGE_WIDTH/(float)SUBIMAGE_HEIGHT,0.999, 1.001);

	// el primer parámetro anterior es FOV  
	
	//gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vColor[] =  {0.1f, 0.1f, 1.f, 1.0f};

	////////////////
	// Opción uno
	////////////////
	

	rectangulo.Begin(GL_TRIANGLE_FAN, 4, 1);
		// Upper left hand corner
		rectangulo.MultiTexCoord2f(0, 1.0f, 1.0f);
		rectangulo.Vertex3f(-1.0, 1.0, 0.0);
		// Lower left hand corner
		rectangulo.MultiTexCoord2f(0, -1.0f, -1.0f);
		rectangulo.Vertex3f(-1.0, -1.0, 0.0f);
		// Lower right hand corner
		rectangulo.MultiTexCoord2f(0, 1.0f, -1.0f);
		rectangulo.Vertex3f(1.0, -1.0, 0.0f);
		// Upper right hand corner
		rectangulo.MultiTexCoord2f(0, 1.0f, 1.0f);
		rectangulo.Vertex3f(1.0, 1.0, 0.0f);
	rectangulo.End();
			
	glUseProgram(alioscopyShader);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texName);
	rectangulo.Draw();

	
	//////////////////////
	//  Fin opción uno
	/////////////////////


	////////////////
	// Opción Dos
	////////////////
	/*
	
	glUniform4fv(locColor,1,vColor);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texName);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	modelViewMatrix.LoadIdentity();
	rectangulo.Begin(GL_QUADS,4);
	{
		//rectangulo.Normal3f(0.0f, -1.0f, 0.0f);
		rectangulo.MultiTexCoord2f(texName, 0.0f, 1.0f);
		glVertex3f(0.0, -1.0, 0.0);

		//rectangulo.Normal3f(0.0f, -1.0f, 0.0f);
		rectangulo.MultiTexCoord2f(texName, 0.0f, 0.0f);
		glVertex3f(0.0, 0.0, 0.0);

		//rectangulo.Normal3f(0.0f, -1.0f, 0.0f);
		rectangulo.MultiTexCoord2f(texName, 1.0f, 0.0f);
		glVertex3f(-1.0, 0.0, 0.0);
		
		//rectangulo.Normal3f(0.0f, -1.0f, 0.0f);
		rectangulo.MultiTexCoord2f(texName, 1.0f, 1.0f);
		glVertex3f(-1.0, -1.0, 0.0);
	}  
	glEnd();
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texName);
	
	shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, 
											 transformPipeline.GetModelViewMatrix(),
											 transformPipeline.GetProjectionMatrix(), 
											 vLightPos, vWhite, 0);
	
	rectangulo.Draw();

	modelViewMatrix.PopMatrix();
	

	*/
	//////////////////////
	//  Fin opción Dos
	/////////////////////
			
	// Flush drawing commands
	glutSwapBuffers();    // pasa del back buffer al front buffer, y que el usuario la vea
	glutPostRedisplay();   // re despliega


}   // void RenderScene(void)


// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
    {
	if(key == GLUT_KEY_UP)
	{
		cameraFrame.RotateLocalX(m3dDegToRad(3.0f));
		//cameraFrame.RotateWorld(m3dDegToRad(-3.0f), 1.0f, 0.0f, 0.0f);
    } //  if(key == GLUT_KEY_UP)

	if(key == GLUT_KEY_DOWN)
	{
		cameraFrame.RotateLocalX(m3dDegToRad(-3.0f));
		//cameraFrame.RotateWorld(m3dDegToRad(3.0f), 1.0f, 0.0f, 0.0f);	
	}  //  if(key == GLUT_KEY_DOWN)

	if(key == GLUT_KEY_LEFT)
	{
		cameraFrame.RotateLocalY(m3dDegToRad(-3.0f));
		// objectFrame.RotateWorld(m3dDegToRad(-3.0f), 0.0f, 1.0f, 0.0f);
    } // if(key == GLUT_KEY_LEFT)


	if(key == GLUT_KEY_RIGHT)
	{
		cameraFrame.RotateLocalY(m3dDegToRad(3.0f));
		//objectFrame.RotateWorld(m3dDegToRad(3.0f), 0.0f, 1.0f, 0.0f);
	} // if(key == GLUT_KEY_RIGHT)

	if(key == GLUT_KEY_F2)
	{
			// Estas son pruebas para ver si se ve realmente en 3D, luego se pueden elimiar
				float desplazar = 0.0f;
				float avance = 0.01f;
				int total = 30;
				for (int i=0;i<total;++i){
					cameraFrame.TranslateLocal(desplazar/4, 0.0f, desplazar);
					desplazar = desplazar + avance;
					glutPostRedisplay();	
					//Sleep(50);
				}  // for
	}  // IF

	if(key == GLUT_KEY_F3)
	{
			// Estas son pruebas para ver si se ve realmente en 3D, luego se pueden elimiar
				float desplazar = -0.0f;
				float avance = -0.01f;
				int total = 30;
				for (int i=0;i<total;++i){
					cameraFrame.TranslateLocal(desplazar/4, 0.0f, desplazar);
					desplazar = desplazar + avance;
					glutPostRedisplay();	
					//Sleep(50);
				}  // for
	}  // IF
    
	glutPostRedisplay();

    }  //  void SpecialKeys(int key, int x, int y)



///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int nWidth, int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);
	viewFrustum.SetPerspective(35.0f, float(nWidth) / float(nHeight), 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
   	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

	modelViewMatrix.LoadIdentity();

	// update screen sizes
	screenWidth = nWidth;
	screenHeight = nHeight;
}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{

	clienteInternoAlambrado *cliente = new clienteInternoAlambrado();
	
	/*cliente->enviarMensaje("./fbo_textures 0 1", 55555, "compute-0-0");
	cliente->enviarMensaje("./fbo_textures 0 0", 55556, "compute-0-1");
	cliente->enviarMensaje("./fbo_textures 1 1", 55557, "compute-1-0");
	cliente->enviarMensaje("./fbo_textures 1 0", 55558, "compute-1-1");*/

	screenWidth = windowWidth;
    	screenHeight = windowHeight;
   	// bFullScreen = true; 
  	//  bAnimated = true;
   	// fboName = 0;
   	// depthBufferName = 0;


	gltSetWorkingDirectory(argv[0]);
	shiftAncho = atoi (argv[1]);
	shiftAlto = atoi (argv[2]);
	//trasladoY = atof (argv[3]);


	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(screenWidth, screenHeight);
	glutCreateWindow("Proyecto: iReal - Prueba monitores Alioscopy");
    	glutReshapeFunc(ChangeSize);
    	glutSpecialFunc(SpecialKeys);
    	glutDisplayFunc(RenderScene);
        
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
		}
	

	SetupRC();

	glutMainLoop();
    
    	ShutdownRC();
    
	return 0;
	}
