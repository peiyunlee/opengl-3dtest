// 
// Normal Mapping
// 
// ����e���ǳƤu�@
// ���� CShape.h ���� #define PERVERTEX_LIGHTING�A�ϥ� PERPIXEL_LIGHTING �~�|���@��
// �]�w #define MULTITEXTURE  (DIFFUSE_MAP|LIGHT_MAP|NORMAL_MAP)
//

#include "header/Angel.h"
#include "Common/CQuad.h"
#include "Common/CSolidCube.h"
#include "Common/CSolidSphere.h"
#include "Common/CWireCube.h"
#include "Common/CChecker.h"
#include "Common/CCamera.h"
#include "png_loader.h"
#include "Common/ModelPool.h"
#include "Common/C2DSprite.h"
#include "Common/Room.h"


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

// For View Point
GLfloat g_fRadius = 8.0;
GLfloat g_fTheta = 45.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;


// 2D �����һݭn�������ܼ�
C2DSprite *g_p2DBtn[4];
mat4  g_2DView = mat4(1, 0, 0, 0
	, 0, 1, 0, 0
	, 0, 0, 1, 0
	, 0, 0, -1, 1);;
mat4  g_2DProj;

point4  g_vEye(g_fRadius*sin(g_fTheta)*cos(g_fPhi), g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), 1.0);
point4  g_vAt(0.0, 0.0, 0.0, 1.0);
vec4    g_vUp(0.0, 1.0, 0.0, 0.0);


//----------------------------------------------------------------------------
Room *room1;

//----------------------------------------------------------------------------
// �禡���쫬�ŧi
extern void IdleProcess();
void UIGenerator();
void UIAction(vec2 pt);

void init( void )
{
	mat4 mxT;
	vec4 vT, vColor;

	//point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4  at(0.0f, 0.0f, 0.0f, 1.0f);
	point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	point4  at(0.0f, 2.0f, 0.0f, 1.0f);

	auto camera = CCamera::create();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	room1 = new Room1();

	UIGenerator();

	bool bPDirty;
	mat4 mpx = camera->getProjectionMatrix(bPDirty);

	room1->SetProjectionMatrix(mpx);
	
}

//----------------------------------------------------------------------------
void GL_Display( void )
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window

	room1->Draw();

	for (int i = 0; i <4; i++) g_p2DBtn[i]->Draw();

	glutSwapBuffers();	// �洫 Frame Buffer
}

void onFrameMove(float delta)
{

	mat4 mvx;	// view matrix & projection matrix
	bool bVDirty;	// view �P projection matrix �O�_�ݭn��s������
	auto camera = CCamera::getInstance();
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) {
		room1->SetViewMatrix(mvx);
	}

	room1->Update(delta);

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
		room1->g_bAutoRotating = !room1->g_bAutoRotating;
		break;
	case 82: // R key
		if(room1->g_fLightR <= 0.95f ) room1->g_fLightR += 0.05f;
		room1->g_Light[0].diffuse.x = room1->g_fLightR;
		room1->g_pLight[0].SetColor(room1->g_Light[0].diffuse);
		break;
	case 114: // r key
		if(room1->g_fLightR >= 0.05f ) room1->g_fLightR -= 0.05f;
		room1->g_Light[0].diffuse.x = room1->g_fLightR;
		room1->g_pLight[0].SetColor(room1->g_Light[0].diffuse);
		break;
	case 71: // G key
		if(room1->g_fLightG <= 0.95f ) room1->g_fLightG += 0.05f;
		room1->g_Light[0].diffuse.y = room1->g_fLightG;
		room1->g_pLight[0].SetColor(room1->g_Light[0].diffuse);
		break;
	case 103: // g key
		if(room1->g_fLightG >= 0.05f ) room1->g_fLightG -= 0.05f;
		room1->g_Light[0].diffuse.y = room1->g_fLightG;
		room1->g_pLight[0].SetColor(room1->g_Light[0].diffuse);
		break;
	case 66: // B key
		if(room1->g_fLightB <= 0.95f ) room1->g_fLightB += 0.05f;
		room1->g_Light[0].diffuse.z = room1->g_fLightB;
		room1->g_pLight[0].SetColor(room1->g_Light[0].diffuse);
		break;
	case 98: // b key
		if(room1->g_fLightB >= 0.05f ) room1->g_fLightB -= 0.05f;
		room1->g_Light[0].diffuse.z = room1->g_fLightB;
		room1->g_pLight[0].SetColor(room1->g_Light[0].diffuse);
		break;
//----------------------------------------------------------------------------
    case 033:
		glutIdleFunc( NULL );
		delete room1;

		for (int i = 0; i < 4; i++)
		{
			delete g_p2DBtn[i];
		}
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
		case GLUT_LEFT_BUTTON:   // �ثe���U���O�ƹ�����
			if (state == GLUT_DOWN) {
				ScreenToUICoordinate(x, y, pt);
				UIAction(pt);
			}
			break;
		case GLUT_MIDDLE_BUTTON:  // �ثe���U���O�ƹ����� �A���� Y �b
			//if ( state == GLUT_DOWN ) ; 
			break;
		case GLUT_RIGHT_BUTTON:   // �ثe���U���O�ƹ��k��
			//if ( state == GLUT_DOWN ) ;
			break;
		default:
			break;
	} 
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {

	switch(key) {
		case GLUT_KEY_LEFT:		// �ثe���U���O�V����V��

			break;
		case GLUT_KEY_RIGHT:	// �ثe���U���O�V�k��V��

			break;
		case GLUT_KEY_UP:	// �ثe���U���O�V�k��V��
			CCamera::getInstance()->moveForward();
			break;
		case GLUT_KEY_DOWN:	// �ثe���U���O�V�k��V��
			CCamera::getInstance()->moveBackward();
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {
	//g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE);  // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	//g_fTheta = (float)M_PI*(float)y / SCREEN_SIZE;;
	//point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4  at(0.0f, 0.0f, 0.0f, 1.0f);
	//auto camera = CCamera::getInstance();
	//camera->updateViewLookAt(eye, at);


	g_fPhi = (float)-M_PI*(x-HALF_SIZE)/ (HALF_SIZE);   // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	g_fTheta = (float)-M_PI*y / (SCREEN_SIZE);

	point4  at(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	CCamera::getInstance()->updateLookAt(at);
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	//g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE); // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
	//g_fTheta = (float)M_PI*(float)y / SCREEN_SIZE;
	//point4  eye(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	//point4  at(0.0f, 0.0f, 0.0f, 1.0f);
	//auto camera = CCamera::getInstance();
	//camera->updateViewLookAt(eye, at);


	g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE);   // �ഫ�� g_fPhi ���� -PI �� PI ���� (-180 ~ 180 ����)
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
    glutKeyboardFunc( Win_Keyboard );	// �B�z ASCI ����p A�Ba�BESC ��...����
	glutSpecialFunc( Win_SpecialKeyboard);	// �B�z NON-ASCI ����p F1�BHome�B��V��...����
    glutDisplayFunc( GL_Display );
	glutReshapeFunc( GL_Reshape );
	glutIdleFunc( IdleProcess );
	
    glutMainLoop();
    return 0;
}


void UIGenerator() {
	// �H�U���Q�Υ����v���� 2D ������
	// �d��b X/Y ������  -1 �� 1 �����A��������b Z = 0 
	mat4 mxT2D, mxS2D;
	vec4 vColor2D = vec4(0, 0, 0, 1);

	//�զ���s
	g_p2DBtn[0] = new C2DSprite; g_p2DBtn[0]->SetShader_2DUI();
	vColor2D.x = 1; vColor2D.y = 0; vColor2D.z = 0; g_p2DBtn[0]->SetDefaultColor(vColor2D);
	mxS2D = Scale(0.1f, 0.1f, 1.0f);
	mxT2D = Translate(0.2f, -0.85f, 0);
	g_p2DBtn[0]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[0]->SetViewMatrix(g_2DView);
	g_p2DBtn[0]->SetViewMatrix(g_2DProj);

	//�Ŧ���s
	g_p2DBtn[1] = new C2DSprite; g_p2DBtn[1]->SetShader_2DUI();
	vColor2D.x = 0; vColor2D.y = 1; vColor2D.z = 0; g_p2DBtn[1]->SetDefaultColor(vColor2D);
	mxT2D = Translate(0.4f, -0.85f, 0);
	g_p2DBtn[1]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[1]->SetViewMatrix(g_2DView);
	g_p2DBtn[1]->SetViewMatrix(g_2DProj);

	//�����s
	g_p2DBtn[2] = new C2DSprite; g_p2DBtn[2]->SetShader_2DUI();
	vColor2D.x = 0; vColor2D.y = 0; vColor2D.z = 1; g_p2DBtn[2]->SetDefaultColor(vColor2D);
	mxT2D = Translate(0.6f, -0.85f, 0);
	g_p2DBtn[2]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[2]->SetViewMatrix(g_2DView);
	g_p2DBtn[2]->SetViewMatrix(g_2DProj);

	//������s
	g_p2DBtn[3] = new C2DSprite; g_p2DBtn[3]->SetShader_2DUI();
	vColor2D.x = 0.85; vColor2D.y = 0.85; vColor2D.z = 0.85; g_p2DBtn[3]->SetDefaultColor(vColor2D);
	mxT2D = Translate(0.8f, -0.85f, 0);
	g_p2DBtn[3]->SetTRSMatrix(mxT2D*mxS2D);
	g_p2DBtn[3]->SetViewMatrix(g_2DView);
	g_p2DBtn[3]->SetViewMatrix(g_2DProj);
}

void UIAction(vec2 pt) {
	if (g_p2DBtn[RED_BUTTON]->OnTouches(pt)) {
		if (g_p2DBtn[0]->getButtonStatus()) {
			printf("������\n");
			room1->g_Light[1].isLighting = false;
		}
		else {
			printf("����}\n");
			room1->g_Light[1].isLighting = true;
		}
	}
	//�Ŧ���s������D�O��������On/Off
	if (g_p2DBtn[GREEN_BUTTON]->OnTouches(pt)) {
		if (g_p2DBtn[1]->getButtonStatus()) {
			printf("�����\n");
			room1->g_Light[2].isLighting = false;
		}
		else {
			printf("���}\n");
			room1->g_Light[2].isLighting = true;
		}
	}
	//�����s������Ҧ����O�����ө�On/Off
	if (g_p2DBtn[BLUE_BUTTON]->OnTouches(pt)) {
		if (g_p2DBtn[2]->getButtonStatus()) {
			printf("�Ŧ���\n");
			room1->g_Light[3].isLighting = false;
		}
		else {
			printf("�Ŧ�}\n");
			room1->g_Light[3].isLighting = true;
		}
	}
	//������s������Ҧ����O���E����@�I
	if (g_p2DBtn[3]->OnTouches(pt)) {
		if (g_p2DBtn[3]->getButtonStatus()) {
			printf("�զ�x\n");
			room1->g_Light[0].isLighting = false;
		}
		else {
			printf("�զ�}\n");
			room1->g_Light[0].isLighting = true;
		}
	}
}