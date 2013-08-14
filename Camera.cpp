//StereoFrustum
struct STEREOFUSTUM{
	float top;
	float bottom;
	float left;
	float right;
};
//Clase de la camaras
class Camera{
	struct STEREOFUSTUM stereoFrustum;
	float stereoOffset;
	float nearZ, imageZ, farZ;
	float FOV;
	float aspect;
	float stereoEyeSeparation;
public:
	void SetStereoscopicFrustum(int i){
		i = i - numStereoCameras/2;
			stereoOffset = i*stereoEyeSeparation + stereoEyeSeparation/2.0;
			float aspect = (float)windowWidth/(float)windowHeight;
			float top = nearZ*tan(FOV/(2.0*aspect));
			stereoFrustum.top = top;
			stereoFrustum.bottom = -top;
			float right = aspect*top;
			float shift = -stereoOffset*nearZ/imageZ;
			stereoFrustum.left = -right + shift;
			stereoFrustum.right = right + shift;
			glFrustum(stereoFrustum.left, stereoFrustum.right, stereoFrustum.bottom, stereoFrustum.top,
			nearZ, farZ);
	} //  void SetStereoscopicFrustum(int i){

		void SetFrustum(){
			glViewport(0,0,SUBIMAGE_WIDTH,SUBIMAGE_HEIGHT);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(stereoFrustum.left,stereoFrustum.right, stereoFrustum.bottom, stereoFrustum.top,
			nearZ, farZ);
		}  //  void SetFrustum(){

		void Transform(int i){
			float desplazamiento = 0.0f;
			float entreCamaras = 0.0001f;
			float delta = 0.0f;

			switch  (i)
			{
				case 7 : desplazamiento = -3.5f  ; 
				break ;
				case 6 : desplazamiento = -2.5f  ; 
				break ;				
				case 5 : desplazamiento = -1.5f  ; 
				break;				
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

			delta = desplazamiento * entreCamaras;

			cameraFrame.TranslateLocal(stereoEyeSeparation_General*desplazamiento/157.0,0,0); 

			//cameraFrame.TranslateLocal(desplazamiento*2.56/157*15.2,0,0); 
			//cameraFrame.SetForwardVector(-cameraFrame.GetOriginX(),-cameraFrame.GetOriginY(),-cameraFrame.GetOriginZ());
			//cameraFrame.RotateLocalY(-asin(2.56*desplazamiento/157.0));
			cameraFrame.Normalize();
			
		}  //  void Transform(){

		Camera(){
			nearZ = 1.0f; //Near clip
			imageZ = 157.0f; //Image plane  La documentación pide 157 pulgadas
			farZ = 5000.0f; //Far clip
			FOV = FOV_General  ; //Radians (13.35 degrees)
			stereoEyeSeparation = stereoEyeSeparation_General; //Inches (6.5 cm)
			aspect = (float)windowWidth/(float)windowHeight; // está para el monior pequeño 1920/1200 for Alioscopy 3D HD 24"
		}
};

