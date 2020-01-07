#include "C2DSprite.h"

C2DSprite::C2DSprite()
{
	m_iNumVtx = QUAD_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL;

	m_pPoints = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];

	// ���B�令���������V�� Z ��V 
	m_pPoints[0] = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	m_pPoints[1] = vec4(0.5f, -0.5f, 0.0f, 1.0f);
	m_pPoints[2] = vec4(0.5f, 0.5f, 0.0f, 1.0f);
	m_pPoints[3] = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	m_pPoints[4] = vec4(0.5f, 0.5f, 0.0f, 1.0f);
	m_pPoints[5] = vec4(-0.5f, 0.5f, 0.0f, 1.0f);

	m_pNormals[0] = vec3(0, 0, 1.0f);  // Normal Vector �� W �� 0
	m_pNormals[1] = vec3(0, 0, 1.0f);
	m_pNormals[2] = vec3(0, 0, 1.0f);
	m_pNormals[3] = vec3(0, 0, 1.0f);
	m_pNormals[4] = vec3(0, 0, 1.0f);
	m_pNormals[5] = vec3(0, 0, 1.0f);

	// Set shader's name
	SetShaderName("vsVtxNormal.glsl", "fsVtxNormal.glsl");

	// Create and initialize a buffer object 
	//CreateBufferObject_2DUI();

	// �w�]�� -1.0f, �� Normal �M�w�C��
	m_fColor[0] = -1.0f; m_fColor[1] = -1.0f; m_fColor[2] = -1.0f; m_fColor[3] = 1;

	// Sprite �����U�P�k�W���� local �y��
	m_OriginalLf = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	m_OriginalTR = vec4(0.5f, 0.5f, 0.0f, 1.0f);

	m_bPushed = false;

}

C2DSprite::~C2DSprite()
{

}

void C2DSprite::Update(float dt, point4 vLightPos, color4 vLightI)
{

}

void C2DSprite::Update(float dt, const LightSource &Lights)
{

}

void C2DSprite::Update(float dt)
{
}

void C2DSprite::SetTRSMatrix(mat4 &mat)
{
	m_mxTRS = mat;
	m_bTRSUpdated = true;
	// �p�� Sprite �b�@�ɮy�Ъ����U���P�k�W������m
	m_Lf = m_mxTRS*m_OriginalLf;
	m_TR = m_mxTRS*m_OriginalTR;
}

// �ǤJ���I�w�g�O������ (-1,-1) (1,1) ������ 2D �y��
bool C2DSprite::OnTouches(const vec2 &tp)
{
	if (m_Lf.x <= tp.x && tp.x <= m_TR.x && m_Lf.y <= tp.y && tp.y <= m_TR.y) // �ӫ��s�Q����
	{
		if (!m_bPushed) {
			// �ܦ��Ƕ�
			SetColor(vec4(0.5, 0.5, 0.5, 1.0f));
			m_bPushed = true;
		}
		else {
			m_bPushed = false;
			SetColor(m_DefaultColor);
		}
		return true;		// �Ǧ^���I����s
	}
	else return false;	// �Ǧ^�S���I����s
}

void C2DSprite::SetDefaultColor(vec4 vColor)
{
	m_DefaultColor = vColor;
	SetColor(vColor);
}

void C2DSprite::Draw()
{
	DrawingSetShader_2DUI();
	glDrawArrays(GL_TRIANGLES, 0, QUAD_NUM);
}

void C2DSprite::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays(GL_TRIANGLES, 0, QUAD_NUM);
}