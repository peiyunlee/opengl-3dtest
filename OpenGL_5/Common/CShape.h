#ifndef CSHAPE_H
#define CSHAPE_H
#include "../Header/Angel.h"
#include "TypeDefine.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define FLAT_SHADING    0
#define GOURAUD_SHADING 1

// GPU ���p�⥲���ǧ�h���Ѽƶi�J Shader

//#define LIGHTING_WITHCPU
#define LIGHTING_WITHGPU
//#define PERVERTEX_LIGHTING

// ��ҫ������� non-uniform scale ���ާ@�ɡA�����z�L�p��ϯx�}�ӱo�쥿�T�� Normal ��V
// �}�ҥH�U���w�q�Y�i�A�ثe CPU �p�⪺������
// GPU �������h�O�]�w������

// #define GENERAL_CASE 1 

#define SPOT_LIGHT 1

#define LIGHTCOUNT 4

#define Type_3DMax 'M'
#define Type_Blender 'B'
#define Type_Cinema_4D 'C'


class CShape 
{
protected:
	vec4 *m_pPoints;
	vec3 *m_pNormals;
	vec4 *m_pColors;
	vec2 *m_pTex;
	int  m_iNumVtx;

	GLfloat m_fColor[4]; // Object's color
	// For shaders' name
	char *m_pVXshader, *m_pFSshader;

	// For VAO
	GLuint m_uiVao;

	// For Shader
	GLuint  m_uiModelView, m_uiProjection, m_uiColor;
	GLuint  m_uiProgram;
	GLuint  m_uiBuffer;

#ifdef LIGHTING_WITHGPU
	point4  m_vLightInView[LIGHTCOUNT];	 // �����b�@�ɮy�Ъ���m
	GLuint  m_uiLightInView[LIGHTCOUNT];	 // �����b shader ����m
	GLuint  m_uiAmbient[LIGHTCOUNT];		 // light's ambient  �P Object's ambient  �P ka �����n
	GLuint  m_uiDiffuse[LIGHTCOUNT];
	GLuint  m_uiSpecular[LIGHTCOUNT];	 // light's specular �P Object's specular �P ks �����n
	
	GLuint  m_uiShininess;
	GLuint  m_uiLighting[LIGHTCOUNT];

	point4  m_vSpotTarget[LIGHTCOUNT];
	GLuint  m_uivSpotTarget[LIGHTCOUNT];// �����ؼЦb shader ����m
	
	GLuint  m_uiSpotCosCutoff[LIGHTCOUNT];
	//GLuint  m_uiSpotExponent[LIGHTCOUNT];
	GLuint  m_uiLightType[LIGHTCOUNT];

	color4 m_AmbientProduct[LIGHTCOUNT];
	color4 m_DiffuseProduct[LIGHTCOUNT];
	color4 m_SpecularProduct[LIGHTCOUNT];
	//float m_SpotExponent[LIGHTCOUNT];
	float m_spotCosCutoff[LIGHTCOUNT];
	int  m_iLighting[LIGHTCOUNT];	// �]�w�O�_�n���O
	int lightType[LIGHTCOUNT];
#endif

	// For Matrices
	mat4    m_mxView, m_mxProjection, m_mxTRS;
	mat4    m_mxMVFinal;
	mat3    m_mxMV3X3Final, m_mxITMV;	// �ϥΦb�p�� �������᪺�s Normal
	mat3		m_mxITView;		// View Matrix �� Inverse Transport 
	bool    m_bProjUpdated, m_bViewUpdated, m_bTRSUpdated;

	// For materials
	Material m_Material;

	// For Shading Mode
	// 0: Flat shading, 1: Gouraud shading, 0 for default
	// �n�ܧ�W��Ҧ��A�Q�� SetShadingMode �ӧ���
	int m_iMode;		

	void CreateBufferObject();
	void DrawingSetShader();
	void DrawingWithoutSetShader();
	void SetAPI();


public:
	CShape();
	virtual ~CShape();
	virtual void Draw() = 0;
	virtual void DrawW() = 0; // Drawing without setting shaders
	virtual void Update(float dt, point4 vLightPos, color4 vLightI) = 0;
	virtual	void Update(float dt, const LightSource &Lights) = 0;
	virtual	void Update(const LightSource *Lights , float dt) = 0;
	virtual void Update(float dt) = 0;

	void SetShaderName(const char vxShader[], const char fsShader[]);
	void SetShader(GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	void SetColor(vec4 vColor);
	void SetViewMatrix(mat4 &mat);
	void SetProjectionMatrix(mat4 &mat);
	void SetTRSMatrix(mat4 &mat);

	// For setting materials 
	void SetMaterials(color4 ambient, color4 diffuse, color4 specular);
	void SetKaKdKsShini(float ka, float kd, float ks, float shininess); // ka kd ks shininess

	// For Lighting Calculation
	void SetShadingMode(int iMode) {m_iMode = iMode;}
	vec4 PhongReflectionModel(vec4 vPoint, vec3 vNormal, vec4 vLightPos, color4 vLightI);
	vec4 PhongReflectionModel(vec4 vPoint, vec3 vNormal, const LightSource &Lights);

	void SetShader_2DUI(GLuint uiShaderHandle = MAX_UNSIGNED_INT);
	void CreateBufferObject_2DUI();
	void DrawingSetShader_2DUI();

#ifdef LIGHTING_WITHGPU
	void SetLightingDisable(int num) {m_iLighting[num] = 0;}
#endif

};

#endif
