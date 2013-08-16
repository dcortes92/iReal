#include "gl.h"
#include "alioscopy.h"
#include "Camera.cpp"
#include "clienteInternoAlambrado.cpp"

/*Comentario*/


/////////////////////////////////////////////////////////////////////////////////
// An assortment of needed classes

GLFrame             	objectFrame;

GLBatch             	pyramidBatch;
GLuint              	textureID;

M3DMatrix44f            shadowMatrix;

static const GLenum windowBuff[] = { GL_BACK_LEFT };

//////////////////////////////////////////////////////////////
//Inicio Métodos para dibujar pirámide
//////////////////////////////////////////////////////////////
void MakePyramid(GLBatch& pyramidBatch)
{
        pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
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
        pyramidBatch.Vertex3fv(vApex);          // Apex
    
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        pyramidBatch.Vertex3fv(vFrontLeft);             // Front left corner
    
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        pyramidBatch.Vertex3fv(vFrontRight);            // Front right corner
    
    
        m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
        pyramidBatch.Vertex3fv(vApex);          // Apex
    
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        pyramidBatch.Vertex3fv(vBackLeft);              // Back left corner
        
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        pyramidBatch.Vertex3fv(vFrontLeft);             // Front left corner
    
        m3dFindNormal(n, vApex, vFrontRight, vBackRight);
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
        pyramidBatch.Vertex3fv(vApex);                          // Apex
    
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        pyramidBatch.Vertex3fv(vFrontRight);            // Front right corner
    
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        pyramidBatch.Vertex3fv(vBackRight);                     // Back right cornder
    
    
        m3dFindNormal(n, vApex, vBackRight, vBackLeft);
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
        pyramidBatch.Vertex3fv(vApex);          // Apex
    
        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        pyramidBatch.Vertex3fv(vBackRight);             // Back right cornder

        pyramidBatch.Normal3fv(n);
        pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        pyramidBatch.Vertex3fv(vBackLeft);              // Back left corner

        pyramidBatch.End();
}

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
}

//////////////////////////////////////////////////////////////
//Fin de Métodos para dibujar pirámide
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//Inicio Métodos para dibujar 3D Alioscopy
//////////////////////////////////////////////////////////////

vector<Camera *> stereoCameras;

void setupAlioscopyCamera()
{
	for (int i=0;i<numStereoCameras;i++)
	{
		   Camera * camera=new Camera();
		   stereoCameras.push_back(camera);
		   camera->SetStereoscopicFrustum(i);
	}
}

void configurarAlioscopyShader()
{
	glGenTextures(1,&texName);
	glBindTexture(GL_TEXTURE_2D,texName);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SUBIMAGE_WIDTH, numStereoCameras*SUBIMAGE_HEIGHT, 0, 
	GL_RGB, GL_UNSIGNED_BYTE, NULL);

	alioscopyShader = gltLoadShaderPairWithAttributes("/root/Desktop/Alio_Mix_OpenGL.vert", "/root/Desktop/Alio_Mix_OpenGL.frag", 0);
}

//////////////////////////////////////////////////////////////
//Inicio Métodos para dibujar 3D Alioscopy
//////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context. 
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC()
{
    // Black background
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f );

	setupAlioscopyCamera();

	configurarAlioscopyShader();

    shaderManager.InitializeStockShaders();

	// De la piramide
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    LoadTGATexture("Marble.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    MakePyramid(pyramidBatch);

   cameraFrame.MoveForward(-5.0f);
    // De la piramide
}

///////////////////////////////////////////////////////////////////////////////
// Cleanup... such as deleting texture objects
void ShutdownRC(void)
{
    glDeleteTextures(1, &textureID);
	glDeleteTextures(1, &texName);     // de alioscopy
}

///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{    
		static GLfloat vLightPos [] = { 1.0f, 1.0f, 0.0f };
		static GLfloat vWhite [] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
        // Clear the window with current clearing color
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        /******NUEVO***********/
		for(int i=0;i<numStereoCameras;i++)
		{
			Camera *camera = stereoCameras[i];
			camera->SetFrustum();
			
			glMatrixMode(GL_MODELVIEW);		
			glPushMatrix();

			copiaDeCameraFrame = cameraFrame;
			camera->Transform(i); // hace los cálculos de la cámara
			modelViewMatrix.LoadIdentity();
			
			/****/
			//glPushMatrix();

			M3DVector3f vCameraPos;
			M3DVector3f vCameraForward;
			cameraFrame.GetOrigin(vCameraPos);
			cameraFrame.GetForwardVector(vCameraForward);
		
			// Reset FBO. Draw world again from the real cameras perspective
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glDrawBuffers(1, windowBuff);
			glViewport(0, 0, screenWidth, screenHeight);
			//modelViewMatrix.PushMatrix();	
			/*  **/
			
			/******NUEVO***********/


			/******DIBUJAR PIRAMIDE***********/
			M3DMatrix44f mCamera;
			cameraFrame.GetCameraMatrix(mCamera);
			
			modelViewMatrix.MultMatrix(mCamera);

			M3DMatrix44f mObjectFrame;
			objectFrame.GetMatrix(mObjectFrame);
			modelViewMatrix.MultMatrix(mObjectFrame);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_TEXTURE_2D);
			
			glBindTexture(GL_TEXTURE_2D, textureID);
			shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, 
										 transformPipeline.GetModelViewMatrix(),
										 transformPipeline.GetProjectionMatrix(), 
										 vLightPos, vWhite, 0);

			pyramidBatch.Draw();
			/******DIBUJAR PIRAMIDE***********/

			glMatrixMode(GL_MODELVIEW);		
			glPopMatrix();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,texName);

			glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,(numStereoCameras - i - 1)*SUBIMAGE_HEIGHT,
			(shiftAncho)*SUBIMAGE_WIDTH,(shiftAlto)*SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH,SUBIMAGE_HEIGHT);

			glViewport(0,0,windowWidth,windowHeight);
			cameraFrame = copiaDeCameraFrame;
		}


	/******NUEVO***********/
	//LA PARTE DEL SHADERMANAGER
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV_General,(float)windowWidth/(float)windowHeight,0.999, 1.001);
	
	//gluLookAt(0.0f, 0.0f, 25.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 3.0f);                  SE QUITO NO HACE NADA

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vColor[] =  {0.1f, 0.1f, 1.f, 1.0f};

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
	/******NUEVO***********/

    // Do the buffer Swap
    glutSwapBuffers();
        
    // Do it again
    glutPostRedisplay();

}


// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
        if(key == GLUT_KEY_UP)
                objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
        if(key == GLUT_KEY_DOWN)
                objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
        
        if(key == GLUT_KEY_LEFT)
                objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
        if(key == GLUT_KEY_RIGHT)
                objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
                
        if(key == GLUT_KEY_F2)
			cameraFrame.RotateLocalX(m3dDegToRad(3.0f));
			
		if(key == GLUT_KEY_F3)
			cameraFrame.RotateLocalX(m3dDegToRad(-3.0f));

		if(key == GLUT_KEY_F4)
			cameraFrame.RotateLocalY(m3dDegToRad(-3.0f));

		if(key == GLUT_KEY_F5)
			cameraFrame.RotateLocalY(m3dDegToRad(3.0f));
			    
        glutPostRedisplay();
    }


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
		//COMENTARLOS CUANDO SE COMPILA PARA LOS NODOS
		clienteInternoAlambrado *cliente00 = new clienteInternoAlambrado();
		clienteInternoAlambrado *cliente01 = new clienteInternoAlambrado();
		clienteInternoAlambrado *cliente10 = new clienteInternoAlambrado();
		clienteInternoAlambrado *cliente11 = new clienteInternoAlambrado();
		
		cliente00->conectarServidor(55555, "compute-0-0");
		cliente01->conectarServidor(55556, "compute-0-1");
		cliente10->conectarServidor(55557, "compute-1-0");
		cliente11->conectarServidor(55558, "compute-1-1");
		
		cliente00->enviarMensaje("./Pyramid 0 1");
		cliente01->enviarMensaje("./Pyramid 0 0");
		cliente10->enviarMensaje("./Pyramid 1 1");
		cliente11->enviarMensaje("./Pyramid 1 0");
		
		//cliente->cerrarConexion();
		//cliente01->cerrarConexion();
		//cliente10->cerrarConexion();
		//cliente11->cerrarConexion();
		
		screenWidth = windowWidth;
		screenHeight = windowHeight;
        
        gltSetWorkingDirectory(argv[0]);
		//shiftAncho = atoi (argv[1]);
		//shiftAlto = atoi (argv[2]);
        
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
        glutInitWindowSize(windowWidth, windowHeight);
        glutCreateWindow("Pyramid");
		glutReshapeFunc(ChangeSize);
		glutSpecialFunc(SpecialKeys);
		glutDisplayFunc(RenderScene);
        
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
			fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
			return 1;
        }
        

        SetupRC();
        glutMainLoop();
		ShutdownRC();
        return 0;
}
