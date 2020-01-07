// 
// Normal Mapping
// 
// 執行前的準備工作
// 關閉 CShape.h 中的 #define PERVERTEX_LIGHTING，使用 PERPIXEL_LIGHTING 才會有作用
// 設定 #define MULTITEXTURE  (DIFFUSE_MAP|LIGHT_MAP|NORMAL_MAP)
//

#include "header/Angel.h"
#include "Common/CQuad.h"
#include "Common/CSolidCube.h"
#include "Common/CSolidSphere.h"
#include "Common/CWireSphere.h"
#include "Common/CWireCube.h"
#include "Common/CChecker.h"
#include "Common/CCamera.h"
#include "Common/CLineSegment.h"
#include "png_loader.h"
#include "Common/ModelPool.h"
#include "Common/C2DSprite.h"


#define SPACE_KEY 32
#define SCREEN_SIZE 800
#define HALF_SIZE SCREEN_SIZE /2 
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f
#define GRID_SIZE 20 // must be an even number

#define SETTING_MATERIALS 

#define RED_BUTTON   0
#define GREEN_BUTTON 1
#define BLUE_BUTTON  2
#define WHITE_BUTTON 3

// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxProjection;


// For Objects
//CChecker      *g_pCheckerBottom, *g_pCheckerTop;
//CSolidCube    *g_pCube;
//CSolidSphere  *g_pSphere;

CQuad *g_LeftWall, *g_RightWall;
CQuad *g_FrontWall, *g_BackWall;
CQuad *g_BottomWall, *g_TopWall;


// For View Point
GLfloat g_fRadius = 8.0;
GLfloat g_fTheta = 45.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;

ModelPool *g_pCat;
ModelPool *g_pDeer;
ModelPool *g_pRat;
ModelPool *g_pWolf;

// 2D 介面所需要的相關變數
C2DSprite *g_p2DBtn[4];
mat4  g_2DView = mat4(1, 0, 0, 0
	, 0, 1, 0, 0
	, 0, 0, 1, 0
	, 0, 0, -1, 1);;
mat4  g_2DProj;

bool lighting[LIGHTCOUNT] = { true };

//----------------------------------------------------------------------------
// Part 2 : for single light source
bool g_bAutoRotating = false;
float g_fElapsedTime = 0;
float g_fLightRadius = 5;
float g_fLightTheta = 0;

float g_fLightR = 1.0f;
float g_fLightG = 1.0f;
float g_fLightB = 1.0f;

point4  g_vEye(g_fRadius*sin(g_fTheta)*cos(g_fPhi), g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), 1.0);
point4  g_vAt(0.0, 0.0, 0.0, 1.0);
vec4    g_vUp(0.0, 1.0, 0.0, 0.0);

LightSource g_Light[LIGHTCOUNT] = {
	{
		0,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(g_fLightRadius, g_fLightRadius+2.0, 0.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(0.0f, 0.0f, 0.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		1.0f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(1, 0, 0.0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(-5.0f, 8.0f, -5.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(-7.0f, 2.0f, -7.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0, 0, 0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(7.0f, 3.0f, -7.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(7.0f, 0.0f, -7.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0, 0, 0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(-5.0f, 3.0f, 5.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(-10.0f, 0.0f, 10.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
};

CWireSphere *g_pLight[LIGHTCOUNT];
CLineSegment *g_LightLine[LIGHTCOUNT];
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// 函式的原型宣告
extern void IdleProcess();
void RoomObjGenerator();
void UIGenerator();
void UIAction(vec2 pt);

void init( void )
{
	mat4 mxT;
	vec4 vT, vColor;
	// 產生所需之 Model View 與 Projection Matrix

	//point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4  at(0.0f, 0.0f, 0.0f, 1.0f);
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	point4  at(0.0f, 2.0f, 0.0f, 1.0f);

	auto camera = CCamera::create();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	RoomObjGenerator();
	UIGenerator();

	

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		// 設定 代表 Light 的 WireSphere
		g_pLight[i] = new CWireSphere(0.25f, 6, 3);
		g_pLight[i]->SetShader();
		mxT = Translate(g_Light[i].position);
		g_pLight[i]->SetTRSMatrix(mxT);
		g_pLight[i]->SetColor(g_Light[i].diffuse);
		//g_pLight[i]->SetLightingDisable();
	}


	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_Light[i].UpdateDirection();
		g_LightLine[i] = new CLineSegment(g_Light[i].position, g_Light[i].spotTarget, vec4(1, 0, 0, 1));
		g_LightLine[i]->SetShader();
	}


	// 因為本範例不會動到 Projection Matrix 所以在這裡設定一次即可
	// 就不寫在 OnFrameMove 中每次都 Check
	bool bPDirty;
	mat4 mpx = camera->getProjectionMatrix(bPDirty);


	g_pCat->SetProjectionMatrix(mpx);
	g_pDeer->SetProjectionMatrix(mpx);
	g_pWolf->SetProjectionMatrix(mpx);
	g_pRat->SetProjectionMatrix(mpx);

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_pLight[i]->SetProjectionMatrix(mpx);
		g_LightLine[i]->SetProjectionMatrix(mpx);
	}

	g_BottomWall->SetProjectionMatrix(mpx);
	g_TopWall->SetProjectionMatrix(mpx);
	g_LeftWall->SetProjectionMatrix(mpx);
	g_RightWall->SetProjectionMatrix(mpx);
	g_FrontWall->SetProjectionMatrix(mpx);
	g_BackWall->SetProjectionMatrix(mpx);
}

//----------------------------------------------------------------------------
void GL_Display( void )
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window

	g_pCat->Draw();
	g_pDeer->Draw();
	g_pWolf->Draw();
	g_pRat->Draw();

	g_BottomWall->Draw();
	g_LeftWall->Draw();
	g_RightWall->Draw();
	g_FrontWall->Draw();
	g_BackWall->Draw();
	g_TopWall->Draw();

	for (int i = 1; i < 1; i++)
	{
		g_pLight[i]->Draw();
		g_LightLine[i]->Draw();
	}

	for (int i = 0; i <4; i++) g_p2DBtn[i]->Draw();

	glutSwapBuffers();	// 交換 Frame Buffer
}

//----------------------------------------------------------------------------
// Part 2 : for single light source
void UpdateLightPosition(float dt)
{
	mat4 mxT;
	// 每秒繞 Y 軸轉 90 度
	g_fElapsedTime += dt;
	g_fLightTheta = g_fElapsedTime*(float)M_PI_2;
	if (g_fLightTheta >= (float)M_PI*2.0f) {
		g_fLightTheta -= (float)M_PI*2.0f;
		g_fElapsedTime -= 4.0f;
	}

	g_Light[0].position.x = g_fLightRadius * cosf(g_fLightTheta);
	g_Light[0].position.z = g_fLightRadius * sinf(g_fLightTheta);
	mxT = Translate(g_Light[0].position);
	g_pLight[0]->SetTRSMatrix(mxT);
	g_LightLine[0]->UpdatePosition(g_Light[0].position, g_Light[0].spotTarget);

}
//----------------------------------------------------------------------------

void onFrameMove(float delta)
{

	mat4 mvx;	// view matrix & projection matrix
	bool bVDirty;	// view 與 projection matrix 是否需要更新給物件
	auto camera = CCamera::getInstance();
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) {
		g_pCat->SetViewMatrix(mvx);
		g_pDeer->SetViewMatrix(mvx);
		g_pWolf->SetViewMatrix(mvx);
		g_pRat->SetViewMatrix(mvx);

		g_BottomWall->SetViewMatrix(mvx);
		g_TopWall->SetViewMatrix(mvx);
		g_LeftWall->SetViewMatrix(mvx);
		g_RightWall->SetViewMatrix(mvx);
		g_FrontWall->SetViewMatrix(mvx);
		g_BackWall->SetViewMatrix(mvx);
		for (int i = 0; i < LIGHTCOUNT; i++)
		{
			g_pLight[i]->SetViewMatrix(mvx);
			g_LightLine[i]->SetViewMatrix(mvx);

		}

	}

	if (g_bAutoRotating) { // Part 2 : 重新計算 Light 的位置
		UpdateLightPosition(delta);
	}



	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		g_pLight[i]->Update(delta);
		g_LightLine[i]->UpdatePosition(g_Light[i].position, g_Light[i].spotTarget);
	}

	// 如果需要重新計算時，在這邊計算每一個物件的顏色
	g_pCat->Update(g_Light, delta);
	g_pDeer->Update(g_Light, delta);
	g_pWolf->Update(g_Light, delta);
	g_pRat->Update(g_Light, delta);

	g_BottomWall->Update(g_Light, delta);
	g_TopWall->Update(g_Light, delta);
	g_LeftWall->Update(g_Light, delta);
	g_RightWall->Update(g_Light, delta);
	g_FrontWall->Update(g_Light, delta);
	g_BackWall->Update(g_Light, delta);

	//g_pCube->Update(delta , g_Light[0]);
	//g_pSphere->Update(delta , g_Light[0]);
	//g_BottomWall->Update(delta , g_Light[0]);
	//g_TopWall->Update(delta , g_Light[0]);
	//g_LeftWall->Update(delta , g_Light[0]);
	//g_RightWall->Update(delta , g_Light[0]);
	//g_FrontWall->Update(delta , g_Light[0]);
	//g_BackWall->Update(delta , g_Light[0]);

	GL_Display();
}

//----------------------------------------------------------------------------

void Win_Keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
	case  SPACE_KEY:

		break;
//----------------------------------------------------------------------------
// Part 2 : for single light source
	case 65: // A key
	case 97: // a key
		//g_bAutoRotating = !g_bAutoRotating;
		break;
	case 82: // R key
		if( g_fLightR <= 0.95f ) g_fLightR += 0.05f;
		g_Light[0].diffuse.x = g_fLightR;
		g_pLight[0]->SetColor(g_Light[0].diffuse);
		break;
	case 114: // r key
		if( g_fLightR >= 0.05f ) g_fLightR -= 0.05f;
		g_Light[0].diffuse.x = g_fLightR;
		g_pLight[0]->SetColor(g_Light[0].diffuse);
		break;
	case 71: // G key
		if( g_fLightG <= 0.95f ) g_fLightG += 0.05f;
		g_Light[0].diffuse.y = g_fLightG;
		g_pLight[0]->SetColor(g_Light[0].diffuse);
		break;
	case 103: // g key
		if( g_fLightG >= 0.05f ) g_fLightG -= 0.05f;
		g_Light[0].diffuse.y = g_fLightG;
		g_pLight[0]->SetColor(g_Light[0].diffuse);
		break;
	case 66: // B key
		if( g_fLightB <= 0.95f ) g_fLightB += 0.05f;
		g_Light[0].diffuse.z = g_fLightB;
		g_pLight[0]->SetColor(g_Light[0].diffuse);
		break;
	case 98: // b key
		if( g_fLightB >= 0.05f ) g_fLightB -= 0.05f;
		g_Light[0].diffuse.z = g_fLightB;
		g_pLight[0]->SetColor(g_Light[0].diffuse);
		break;
//----------------------------------------------------------------------------
    case 033:
		glutIdleFunc( NULL );
		delete g_pCat;
		delete g_pDeer;
		delete g_pWolf;
		delete g_pRat;

		//delete g_pCheckerBottom;
		for (int i = 0; i < LIGHTCOUNT; i++)
		{
			delete g_pLight[i];
			delete g_LightLine[i];
		}
		delete g_LeftWall;
		delete g_RightWall;
		delete g_FrontWall;
		delete g_BackWall;
		delete g_TopWall;
		delete g_BottomWall;
		CCamera::getInstance()->destroyInstance();
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
inline void ScreenToUICoordinate(int x, int y, vec2 &pt)
{
	pt.x = 2.0f*(float)x / SCREEN_SIZE - 1.0f;
	pt.y = 2.0f*(float)(SCREEN_SIZE - y) / SCREEN_SIZE - 1.0f;
}
//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	vec2 pt;
	switch(button) {
		case GLUT_LEFT_BUTTON:   // 目前按下的是滑鼠左鍵
			if (state == GLUT_DOWN) {
				ScreenToUICoordinate(x, y, pt);
				UIAction(pt);
			}
			break;
		case GLUT_MIDDLE_BUTTON:  // 目前按下的是滑鼠中鍵 ，換成 Y 軸
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // 目前按下的是滑鼠右鍵
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_LEFT:		// 目前按下的是向左方向鍵

			break;
		case GLUT_KEY_RIGHT:	// 目前按下的是向右方向鍵

			break;
		case GLUT_KEY_UP:	// 目前按下的是向右方向鍵
			CCamera::getInstance()->moveForward();
			break;
		case GLUT_KEY_DOWN:	// 目前按下的是向右方向鍵
			CCamera::getInstance()->moveBackward();
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {
	//g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE);  // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	//g_fTheta = (float)M_PI*(float)y / SCREEN_SIZE;;
	//point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4  at(0.0f, 0.0f, 0.0f, 1.0f);
	//auto camera = CCamera::getInstance();
	//camera->updateViewLookAt(eye, at);


	g_fPhi = (float)-M_PI*(x-HALF_SIZE)/ (HALF_SIZE);   // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)-M_PI*y / (SCREEN_SIZE);

	point4  at(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	CCamera::getInstance()->updateLookAt(at);
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	//g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE); // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	//g_fTheta = (float)M_PI*(float)y / SCREEN_SIZE;
	//point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4  at(0.0f, 0.0f, 0.0f, 1.0f);
	//auto camera = CCamera::getInstance();
	//camera->updateViewLookAt(eye, at);


	g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE);   // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)-M_PI*y / (SCREEN_SIZE);

	point4  at(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	CCamera::getInstance()->updateLookAt(at);
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
	glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    
	glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( SCREEN_SIZE, SCREEN_SIZE );

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );

    glutCreateWindow("Compositing and Blending");

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE; 
    glewInit();  

    init();

	glutMouseFunc(Win_Mouse);
	glutMotionFunc(Win_MouseMotion);
	glutPassiveMotionFunc(Win_PassiveMotion);  
    glutKeyboardFunc( Win_Keyboard );	// 處理 ASCI 按鍵如 A、a、ESC 鍵...等等
	glutSpecialFunc( Win_SpecialKeyboard);	// 處理 NON-ASCI 按鍵如 F1、Home、方向鍵...等等
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}

void RoomObjGenerator() {
	mat4 mxT;
	vec4 vT;

	vT.x = 0.0f; vT.y = 0.0f; vT.z = 0;
	mxT = Translate(vT);
	g_BottomWall = new CQuad;
	//g_BottomWall->SetMaterials(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 1.0f));
	g_BottomWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_BottomWall->SetColor(vec4(0.6f));
	g_BottomWall->SetTRSMatrix(mxT*Scale(20.0f, 1, 20.0f));
	g_BottomWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_BottomWall->SetShadingMode(GOURAUD_SHADING);
	g_BottomWall->SetShader();

	vT.x = 0.0f; vT.y = 20.0f; vT.z = 0;
	mxT = Translate(vT);
	g_TopWall = new CQuad;
	//g_TopWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_TopWall->SetColor(vec4(0.6f));
	g_TopWall->SetTRSMatrix(mxT*RotateZ(180.0f)*Scale(20.0f, 1, 20.0f));
	g_TopWall->SetMaterials(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 1.0f));
	g_TopWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_TopWall->SetShadingMode(GOURAUD_SHADING);
	g_TopWall->SetShader();

	vT.x = -10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_LeftWall = new CQuad;
	//g_LeftWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_LeftWall->SetColor(vec4(0.6f));
	g_LeftWall->SetTRSMatrix(mxT*RotateZ(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_LeftWall->SetMaterials(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 1.0f));
	g_LeftWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_LeftWall->SetShadingMode(GOURAUD_SHADING);
	g_LeftWall->SetShader();

	vT.x = 10.0f; vT.y = 10.0f; vT.z = 0;
	mxT = Translate(vT);
	g_RightWall = new CQuad;
	g_RightWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_RightWall->SetColor(vec4(0.6f));
	g_RightWall->SetTRSMatrix(mxT*RotateZ(90.0f)*Scale(20.0f, 1, 20.0f));
	g_RightWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_RightWall->SetShadingMode(GOURAUD_SHADING);
	g_RightWall->SetShader();

	vT.x = 0.0f; vT.y = 10.0f; vT.z = 10.0f;
	mxT = Translate(vT);
	g_FrontWall = new CQuad;
	g_FrontWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_FrontWall->SetColor(vec4(0.6f));
	g_FrontWall->SetTRSMatrix(mxT*RotateX(-90.0f)*Scale(20.0f, 1, 20.0f));
	g_FrontWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_FrontWall->SetShadingMode(GOURAUD_SHADING);
	g_FrontWall->SetShader();

	vT.x = 0.0f; vT.y = 10.0f; vT.z = -10.0f;
	mxT = Translate(vT);
	g_BackWall = new CQuad;
	g_BackWall->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85, 0.85f, 0.85, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_BackWall->SetColor(vec4(0.6f));
	g_BackWall->SetTRSMatrix(mxT*RotateX(90.0f)*Scale(20.0f, 1, 20.0f));
	g_BackWall->SetKaKdKsShini(0, 0.8f, 0.5f, 1);
	g_BackWall->SetShadingMode(GOURAUD_SHADING);
	g_BackWall->SetShader();

//	g_pCube = new CSolidCube;
////#ifdef SETTING_MATERIALS
////	g_pCube->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0.85f, 0, 0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
////	g_pCube->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);
////#endif
//	g_pCube->SetShader();
//	vT.x = 7.0; vT.y = 0.5; vT.z = -6.0;
//	mxT = Translate(vT);
//	g_pCube->SetTRSMatrix(mxT);
//	g_BackWall->SetColor(vec4(0.78f, 0.21f, 0.22f, 1.0f));
//	g_pCube->SetShadingMode(GOURAUD_SHADING);

	vT.x = 7.0; vT.y = 0.5; vT.z = -6.0;
	mxT = Translate(vT);
	g_pCat = new ModelPool("Model/cat.obj", Type_3DMax);
	g_pCat->SetTRSMatrix(mxT*RotateY(225.0f)*Scale(0.004f, 0.004f, 0.004f));
	g_pCat ->SetMaterials(vec4(0), vec4(0.75f, 0.75f, 0.75f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pCat->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);


	vT.x = -6.0; vT.y = 0.5; vT.z = -6.0;
	mxT = Translate(vT);
	g_pDeer = new ModelPool("Model/deer.obj", Type_3DMax);
	g_pDeer->SetTRSMatrix(mxT*RotateY(-45.0f)*Scale(0.004f, 0.004f, 0.004f));
	g_pDeer->SetMaterials(vec4(0), vec4(0.75f, 0.75f, 0.75f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pDeer->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	vT.x = -6.0; vT.y = 0.5; vT.z = 6.0;
	mxT = Translate(vT);
	g_pWolf = new ModelPool("Model/wolf.obj", Type_3DMax);
	g_pWolf->SetTRSMatrix(mxT*RotateY(45.0f)*Scale(0.007f, 0.007f, 0.007f));
	g_pWolf->SetMaterials(vec4(0), vec4(0.75f, 0.75f, 0.75f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pWolf->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	vT.x = 0.0; vT.y = 0.5; vT.z = -0.0;
	mxT = Translate(vT);
	g_pRat = new ModelPool("Model/rat.obj", Type_3DMax);
	g_pRat->SetTRSMatrix(mxT*RotateY(45.0f)*Scale(0.015f, 0.015f, 0.015f));
	g_pRat->SetMaterials(vec4(0), vec4(0.75f, 0.75f, 0.75f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	g_pRat->SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

//	g_pSphere = new CSolidSphere(1, 16, 16);
//	// Part 3 : materials
////#ifdef SETTING_MATERIALS
////	g_pSphere->SetMaterials(vec4(0.15f, 0.15f, 0.15f, 1.0f), vec4(0, 0, 0.85f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
////	g_pSphere->SetKaKdKsShini(0.15f, 0.45f, 0.55f, 5);
////#endif
//	g_pSphere->SetShader();
//	vT.x = -1.5; vT.y = 1.0; vT.z = 1.5;
//	mxT = Translate(vT);
//	g_pSphere->SetTRSMatrix(mxT);
//	g_pSphere->SetShadingMode(GOURAUD_SHADING);
}


void UIGenerator() {
	// 以下為利用平行投影產生 2D 的介面
	// 範圍在 X/Y 平面的  -1 到 1 之間，介面都放在 Z = 0 
	mat4 mxT2D, mxS2D;
	vec4 vColor2D = vec4(0, 0, 0, 1);

	//白色按鈕
	g_p2DBtn[0] = new C2DSprite; g_p2DBtn[0]->SetShader_2DUI();
	vColor2D.x = 1; vColor2D.y = 0; vColor2D.z = 0; g_p2DBtn[0]->SetDefaultColor(vColor2D);
	mxS2D = Scale(0.1f, 0.1f, 1.0f);
	mxT2D = Translate(0.2f, -0.85f, 0);
	g_p2DBtn[0]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[0]->SetViewMatrix(g_2DView);
	g_p2DBtn[0]->SetViewMatrix(g_2DProj);

	//藍色按鈕
	g_p2DBtn[1] = new C2DSprite; g_p2DBtn[1]->SetShader_2DUI();
	vColor2D.x = 0; vColor2D.y = 1; vColor2D.z = 0; g_p2DBtn[1]->SetDefaultColor(vColor2D);
	mxT2D = Translate(0.4f, -0.85f, 0);
	g_p2DBtn[1]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[1]->SetViewMatrix(g_2DView);
	g_p2DBtn[1]->SetViewMatrix(g_2DProj);

	//綠色按鈕
	g_p2DBtn[2] = new C2DSprite; g_p2DBtn[2]->SetShader_2DUI();
	vColor2D.x = 0; vColor2D.y = 0; vColor2D.z = 1; g_p2DBtn[2]->SetDefaultColor(vColor2D);
	mxT2D = Translate(0.6f, -0.85f, 0);
	g_p2DBtn[2]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[2]->SetViewMatrix(g_2DView);
	g_p2DBtn[2]->SetViewMatrix(g_2DProj);

	//紅色按鈕
	g_p2DBtn[3] = new C2DSprite; g_p2DBtn[3]->SetShader_2DUI();
	vColor2D.x = 1; vColor2D.y = 1; vColor2D.z = 1; g_p2DBtn[3]->SetDefaultColor(vColor2D);
	mxT2D = Translate(0.8f, -0.85f, 0);
	g_p2DBtn[3]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[3]->SetViewMatrix(g_2DView);
	g_p2DBtn[3]->SetViewMatrix(g_2DProj);
}

void UIAction(vec2 pt) {
	if (g_p2DBtn[RED_BUTTON]->OnTouches(pt)) {
		if (g_p2DBtn[0]->getButtonStatus()) {
			printf("紅色關\n");
			lighting[1] = false;
			g_Light[1].ambient = color4(0, 0, 0, 0);
			g_Light[1].diffuse = color4(0, 0, 0, 0);
			g_Light[1].specular = color4(0, 0, 0, 0);
		}
		else {
			printf("紅色開\n");
			lighting[1] = true;
			g_Light[1].diffuse.x = 1;
			g_Light[1].ambient = color4(g_fLightR, g_fLightG, g_fLightB, 1.0f);
			g_Light[1].specular = color4(g_fLightR, g_fLightG, g_fLightB, 1.0f);
		}
	}
	//藍色按鈕→控制主燈光的旋轉On/Off
	if (g_p2DBtn[GREEN_BUTTON]->OnTouches(pt)) {
		if (g_p2DBtn[2]->getButtonStatus()) {
			printf("綠色關\n");
			lighting[2] = false;

			//if (g_p2DBtn[1]->getButtonStatus()) g_bAutoRotating = !g_bAutoRotating;
			//else g_bAutoRotating = !g_bAutoRotating;
		}
		else {
			printf("綠色開\n");
			lighting[2] = true;
		}
	}
	//綠色按鈕→控制所有側燈光的照明On/Off
	if (g_p2DBtn[BLUE_BUTTON]->OnTouches(pt)) {
		if (g_p2DBtn[2]->getButtonStatus()) {
			printf("藍色關\n");
			lighting[3] = false;
			/*b_Light2 = false;
			b_Light3 = false;
			b_Light4 = false;

			g_Light2.diffuse = color4(0, 0, 0, 0);
			g_Light2.ambient = color4(0, 0, 0, 0);
			g_Light2.specular = color4(0, 0, 0, 0);

			g_Light3.diffuse = color4(0, 0, 0, 0);
			g_Light3.ambient = color4(0, 0, 0, 0);
			g_Light3.specular = color4(0, 0, 0, 0);

			g_Light4.diffuse = color4(0, 0, 0, 0);
			g_Light4.ambient = color4(0, 0, 0, 0);
			g_Light4.specular = color4(0, 0, 0, 0);*/
		}
		else {
			printf("藍色開\n");
			lighting[3] = true;
			/*b_Light2 = true;
			b_Light3 = true;
			b_Light4 = true;

			g_Light2.diffuse.x = g_fLightR2;
			g_Light2.diffuse.y = g_fLightG2;
			g_Light2.diffuse.z = g_fLightB2;
			g_Light2.ambient = color4(0.95f, 0.95f, 0.95f, 1.0f);
			g_Light2.specular = color4(0.95f, 0.95f, 0.95f, 1.0f);

			g_Light3.diffuse.x = g_fLightR3;
			g_Light3.diffuse.y = g_fLightG3;
			g_Light3.diffuse.z = g_fLightB3;
			g_Light3.ambient = color4(0.95f, 0.95f, 0.95f, 1.0f);
			g_Light3.specular = color4(0.95f, 0.95f, 0.95f, 1.0f);

			g_Light4.diffuse.x = g_fLightR4;
			g_Light4.diffuse.y = g_fLightG4;
			g_Light4.diffuse.z = g_fLightB4;
			g_Light4.ambient = color4(0.95f, 0.95f, 0.95f, 1.0f);
			g_Light4.specular = color4(0.95f, 0.95f, 0.95f, 1.0f);*/
		}
	}
	//紅色按鈕→控制所有側燈光聚集於一點
	if (g_p2DBtn[3]->OnTouches(pt)) {
		if (g_p2DBtn[3]->getButtonStatus()) {
			printf("白色官\n");
			lighting[0] = false;
			g_bAutoRotating = !g_bAutoRotating;
		}
		else {
			printf("白色開\n");
			lighting[0] = true;
			g_bAutoRotating = !g_bAutoRotating;
		}
	}
}